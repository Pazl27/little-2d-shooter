#include "game.hpp"

#include <raylib.h>

#include <cmath>
#include <stdexcept>
#include <string>

#include "core/constants.hpp"
#include "network/network_manager.hpp"

Game::Game(bool hostFlag) : isRunning(false), isHost(hostFlag), remotePlayerConnected(false) {
    InitWindow(Constants::SCREEN_WIDTH, Constants::SCREEN_HEIGHT, windowTitle.c_str());
    SetTargetFPS(60);

    // Initialize the game map
    gameMap = new Map();

    network = new NetworkManager(isHost);
    if (!network->init()) {
        delete gameMap;
        CloseWindow();
        throw std::runtime_error("Failed to initialize network");
    }

    // Only create local player initially
    if (isHost) {
        players.emplace_back(5, BLUE, 10, PlayerShape::CIRCLE);  // Host player
    } else {
        players.emplace_back(5, RED, 10, PlayerShape::CIRCLE);  // Client player
    }

    // Set initial spawn position based on role
    int margin = 50;  // Safe distance from walls and obstacles
    if (isHost) {
        players[0].setPosition({margin, margin});  // Host spawns top-left
    } else {
        players[0].setPosition({Constants::SCREEN_WIDTH - margin, Constants::SCREEN_HEIGHT - margin});  // Client spawns bottom-right
    }
}

Game::~Game() {
    delete network;
    delete gameMap;
    players.clear();
    CloseWindow();
}

void Game::start() {
    isRunning = true;

    while (isRunning) {
        BeginDrawing();
        ClearBackground(RAYWHITE);

        int localIndex = 0;   // Local player is always index 0
        int remoteIndex = 1;  // Remote player is always index 1 (if connected)

        // === PLAYER MOVEMENT ===
        players[localIndex].move(gameMap);
        Position localPos = players[localIndex].getPosition();
        network->sendPosition(localPos.x, localPos.y);

        float rx, ry;
        if (network->receivePosition(rx, ry)) {
            // Create remote player if not already created
            if (!remotePlayerConnected && network->isConnected()) {
                createRemotePlayer();
            }

            if (remotePlayerConnected && players.size() > 1) {
                players[remoteIndex].setPosition({static_cast<int>(rx), static_cast<int>(ry)});
            }
        }

        // === BULLET SYNCING ===
        // Send local bullets
        network->sendBullets(players[localIndex].getBullets());

        // Receive remote bullets
        std::vector<Bullet> remoteBullets;
        if (network->receiveBullets(remoteBullets)) {
            // Create remote player if not already created
            if (!remotePlayerConnected && network->isConnected()) {
                createRemotePlayer();
            }

            if (remotePlayerConnected && players.size() > 1) {
                players[remoteIndex].setBullets(remoteBullets);
            }
        }

        // === DAMAGE HANDLING ===
        // All damage is handled locally through bullet collision detection
        // No network damage messages needed

        // === RESET SYNCHRONIZATION ===
        // Check for incoming reset from remote player
        if (network->receiveReset()) {
            reset();
        }

        // === UPDATE BULLETS ===
        players[localIndex].updateBullets(gameMap);
        if (remotePlayerConnected && players.size() > 1) {
            players[remoteIndex].updateBullets(gameMap);
        }

        // === COLLISION DETECTION ===
        if (remotePlayerConnected && players.size() > 1) {
            checkBulletCollisions(localIndex, remoteIndex);
        }

        // === HEALTH SYNCING FOR DISPLAY ===
        // Send our health if it changed (after collision detection)
        if (players[localIndex].hasHealthChanged()) {
            network->sendHealth(players[localIndex].getHealth());
            players[localIndex].clearHealthChangeFlag();
        }

        // Send remote player health if we applied damage to them locally
        if (remotePlayerConnected && players.size() > 1 && players[remoteIndex].hasHealthChanged()) {
            network->sendHealth(players[remoteIndex].getHealth());
            players[remoteIndex].clearHealthChangeFlag();
        }

        // Receive remote player's health for display
        int remoteHealth;
        if (network->receiveHealth(remoteHealth)) {
            if (remotePlayerConnected && players.size() > 1) {
                players[remoteIndex].setHealth(remoteHealth);
                players[remoteIndex].clearHealthChangeFlag();  // Don't trigger another sync
            }
        }

        // === DRAW MAP ===
        gameMap->draw();

        // === DRAW PLAYERS AND BULLETS ===
        for (auto& player : players) {
            player.draw();
        }

        // === DRAW CONNECTION STATUS ===
        if (!remotePlayerConnected) {
            std::string waitingText;
            if (isHost) {
                waitingText = "Waiting for client to connect...";
            } else {
                waitingText = network->isConnected() ? "Connected! Waiting for game data..." : "Connecting to host...";
            }
            int textWidth = MeasureText(waitingText.c_str(), 20);
            DrawText(waitingText.c_str(), Constants::SCREEN_WIDTH / 2 - textWidth / 2, Constants::SCREEN_HEIGHT / 2 - 100, 20, DARKGRAY);
        }

        // === DRAW HEALTH ===
        drawHealth();

        // === CHECK GAME OVER ===
        if (!players[localIndex].isAlive()) {
            DrawText("YOU DIED! Press R to restart or ESC to quit", Constants::SCREEN_WIDTH / 2 - 200, Constants::SCREEN_HEIGHT / 2, 20,
                     RED);
            if (IsKeyPressed(KEY_R)) {
                network->sendReset();
                reset();
            }
        }
        if (remotePlayerConnected && players.size() > 1 && !players[remoteIndex].isAlive()) {
            DrawText("YOU WIN! Press R to restart or ESC to quit", Constants::SCREEN_WIDTH / 2 - 200, Constants::SCREEN_HEIGHT / 2, 20,
                     GREEN);
            if (IsKeyPressed(KEY_R)) {
                network->sendReset();
                reset();
            }
        }

        EndDrawing();

        if (WindowShouldClose()) {
            stop();
        }
    }
}

void Game::stop() {
    isRunning = false;
}

void Game::createRemotePlayer() {
    if (!remotePlayerConnected && players.size() == 1) {
        if (isHost) {
            players.emplace_back(5, RED, 10, PlayerShape::CIRCLE);  // Client player
        } else {
            players.emplace_back(5, BLUE, 10, PlayerShape::CIRCLE);  // Host player
        }

        // Set spawn position for remote player (opposite corner from local player)
        int margin = 50;  // Safe distance from walls and obstacles
        if (isHost) {
            players[1].setPosition({Constants::SCREEN_WIDTH - margin, Constants::SCREEN_HEIGHT - margin});  // Client spawns bottom-right
        } else {
            players[1].setPosition({margin, margin});  // Host spawns top-left
        }
        remotePlayerConnected = true;
    }
}

void Game::reset() {
    // Reset player health
    for (auto& player : players) {
        player.setHealth(100);
        player.clearHealthChangeFlag();
    }

    // Clear all bullets
    std::vector<Bullet> emptyBullets;
    players[0].setBullets(emptyBullets);
    if (players.size() > 1) {
        players[1].setBullets(emptyBullets);
    }

    // Reset positions (opposite corners, safe from obstacles)
    int margin = 50;  // Safe distance from walls and obstacles
    if (isHost) {
        players[0].setPosition({margin, margin});  // Host top-left
        if (players.size() > 1) {
            players[1].setPosition({Constants::SCREEN_WIDTH - margin, Constants::SCREEN_HEIGHT - margin});  // Client bottom-right
        }
    } else {
        players[0].setPosition({Constants::SCREEN_WIDTH - margin, Constants::SCREEN_HEIGHT - margin});  // Client bottom-right
        if (players.size() > 1) {
            players[1].setPosition({margin, margin});  // Host top-left
        }
    }
}

void Game::checkBulletCollisions(int localIndex, int remoteIndex) {
    // Check if local bullets hit remote player
    auto& localBullets = const_cast<std::vector<Bullet>&>(players[localIndex].getBullets());
    for (auto it = localBullets.begin(); it != localBullets.end();) {
        if (players[remoteIndex].isCollidingWith(*it)) {
            // Apply damage immediately for instant visual feedback
            players[remoteIndex].takeDamage(10);
            // No need to send damage over network - collision is handled locally on both sides
            it = localBullets.erase(it);
        } else {
            ++it;
        }
    }

    // Check if remote bullets hit local player
    auto& remoteBullets = const_cast<std::vector<Bullet>&>(players[remoteIndex].getBullets());
    for (auto it = remoteBullets.begin(); it != remoteBullets.end();) {
        if (players[localIndex].isCollidingWith(*it)) {
            // When remote bullet hits us, we take damage locally
            players[localIndex].takeDamage(10);
            it = remoteBullets.erase(it);
        } else {
            ++it;
        }
    }

    // Update the bullets vectors in the players
    players[localIndex].setBullets(localBullets);
    players[remoteIndex].setBullets(remoteBullets);
}

void Game::drawHealth() {
    int localIndex = 0;   // Local player is always index 0
    int remoteIndex = 1;  // Remote player is always index 1

    int localHealth = players[localIndex].getHealth();
    int remoteHealth = remotePlayerConnected && players.size() > 1 ? players[remoteIndex].getHealth() : 0;

    // Health bar dimensions and constants
    const int barWidth = 150;
    const int barHeight = 12;
    const int margin = 15;
    const int fontSize = 14;
    const int labelOffset = 18;

    // Color based on health percentage
    auto getHealthColor = [](int health) -> Color {
        if (health > 70) return GREEN;
        if (health > 40) return YELLOW;
        if (health > 15) return ORANGE;
        return RED;
    };

    Color localHealthColor = getHealthColor(localHealth);
    Color remoteHealthColor = getHealthColor(remoteHealth);

    // === LOCAL PLAYER HEALTH (Bottom Left) ===
    int localBarX = margin;
    int localBarY = Constants::SCREEN_HEIGHT - 35;

    // Background bar with subtle shadow
    DrawRectangle(localBarX + 1, localBarY + 1, barWidth, barHeight, {20, 20, 20, 180});
    DrawRectangle(localBarX, localBarY, barWidth, barHeight, {40, 40, 40, 220});

    // Health fill with simple gradient effect
    int healthWidth = (localHealth * (barWidth - 2)) / 100;
    DrawRectangle(localBarX + 1, localBarY + 1, healthWidth, barHeight - 2, localHealthColor);
    // Add a subtle highlight on top
    Color highlightColor = {(unsigned char)std::min(255, localHealthColor.r + 40), (unsigned char)std::min(255, localHealthColor.g + 40),
                            (unsigned char)std::min(255, localHealthColor.b + 40), 150};
    DrawRectangle(localBarX + 1, localBarY + 1, healthWidth, 2, highlightColor);

    // Border with highlight
    DrawRectangleLines(localBarX, localBarY, barWidth, barHeight, WHITE);
    DrawRectangleLines(localBarX + 1, localBarY + 1, barWidth - 2, barHeight - 2, {200, 200, 200, 100});

    // Player label and health text
    DrawText("YOU", localBarX, localBarY - labelOffset, fontSize, BLACK);
    std::string localHealthText = std::to_string(localHealth) + "%";
    int textWidth = MeasureText(localHealthText.c_str(), fontSize);
    DrawText(localHealthText.c_str(), localBarX + barWidth - textWidth, localBarY - labelOffset, fontSize, BLACK);

    // === ENEMY PLAYER HEALTH (Bottom Right) ===
    // Only show if remote player is connected
    if (remotePlayerConnected && players.size() > 1) {
        int enemyBarX = Constants::SCREEN_WIDTH - barWidth - margin;
        int enemyBarY = Constants::SCREEN_HEIGHT - 35;

        // Background bar with subtle shadow
        DrawRectangle(enemyBarX + 1, enemyBarY + 1, barWidth, barHeight, {20, 20, 20, 180});
        DrawRectangle(enemyBarX, enemyBarY, barWidth, barHeight, {40, 40, 40, 220});

        // Health fill with simple gradient effect
        int enemyHealthWidth = (remoteHealth * (barWidth - 2)) / 100;
        DrawRectangle(enemyBarX + 1, enemyBarY + 1, enemyHealthWidth, barHeight - 2, remoteHealthColor);
        // Add a subtle highlight on top
        Color enemyHighlightColor = {(unsigned char)std::min(255, remoteHealthColor.r + 40),
                                     (unsigned char)std::min(255, remoteHealthColor.g + 40),
                                     (unsigned char)std::min(255, remoteHealthColor.b + 40), 150};
        DrawRectangle(enemyBarX + 1, enemyBarY + 1, enemyHealthWidth, 2, enemyHighlightColor);

        // Border with highlight
        DrawRectangleLines(enemyBarX, enemyBarY, barWidth, barHeight, WHITE);
        DrawRectangleLines(enemyBarX + 1, enemyBarY + 1, barWidth - 2, barHeight - 2, {200, 200, 200, 100});

        // Enemy label and health text
        int enemyLabelWidth = MeasureText("ENEMY", fontSize);
        DrawText("ENEMY", enemyBarX + barWidth - enemyLabelWidth, enemyBarY - labelOffset, fontSize, BLACK);
        std::string remoteHealthText = std::to_string(remoteHealth) + "%";
        DrawText(remoteHealthText.c_str(), enemyBarX, enemyBarY - labelOffset, fontSize, BLACK);
    }

    // === LOW HEALTH VISUAL EFFECTS ===
    // Add warning indicators for critically low health
    if (localHealth <= 25) {
        // Pulsing red overlay for critically low health
        float pulse = (sin(GetTime() * 8.0f) + 1.0f) / 2.0f;  // 0.0 to 1.0
        Color pulseColor = {255, 0, 0, (unsigned char)(50 * pulse)};
        DrawRectangle(localBarX - 2, localBarY - 2, barWidth + 4, barHeight + 4, pulseColor);

        // Warning text
        if ((int)(GetTime() * 2) % 2 == 0) {  // Blink every 0.5 seconds
            DrawText("LOW HEALTH!", localBarX, localBarY + barHeight + 5, 12, RED);
        }
    }

    if (remotePlayerConnected && players.size() > 1 && remoteHealth <= 25) {
        // Subtle glow for enemy low health
        int enemyBarX = Constants::SCREEN_WIDTH - barWidth - margin;
        int enemyBarY = Constants::SCREEN_HEIGHT - 35;
        float pulse = (sin(GetTime() * 6.0f) + 1.0f) / 2.0f;
        Color pulseColor = {255, 100, 0, (unsigned char)(30 * pulse)};
        DrawRectangle(enemyBarX - 1, enemyBarY - 1, barWidth + 2, barHeight + 2, pulseColor);
    }
}
