#include "game.hpp"

#include <raylib.h>

#include <iostream>
#include <stdexcept>
#include <string>

#include "core/constants.hpp"
#include "network/network_manager.hpp"

Game::Game(bool hostFlag) : isRunning(false), isHost(hostFlag) {
    InitWindow(Constants::SCREEN_WIDTH, Constants::SCREEN_HEIGHT, windowTitle.c_str());
    SetTargetFPS(60);

    network = new NetworkManager(isHost);
    if (!network->init()) {
        CloseWindow();
        throw std::runtime_error("Failed to initialize network");
    }

    if (isHost) {
        players.emplace_back(5, BLUE, 10, PlayerShape::CIRCLE);  // Host player
        players.emplace_back(5, RED, 10, PlayerShape::CIRCLE);   // Client placeholder
    } else {
        players.emplace_back(5, RED, 10, PlayerShape::CIRCLE);   // Client player
        players.emplace_back(5, BLUE, 10, PlayerShape::CIRCLE);  // Host placeholder
    }
}

Game::~Game() {
    delete network;
    players.clear();
    CloseWindow();
}

void Game::start() {
    isRunning = true;

    while (isRunning) {
        BeginDrawing();
        ClearBackground(RAYWHITE);

        int localIndex = isHost ? 0 : 1;
        int remoteIndex = isHost ? 1 : 0;

        // === PLAYER MOVEMENT ===
        players[localIndex].move();
        Position localPos = players[localIndex].getPosition();
        network->sendPosition(localPos.x, localPos.y);

        float rx, ry;
        if (network->receivePosition(rx, ry)) {
            players[remoteIndex].setPosition({static_cast<int>(rx), static_cast<int>(ry)});
        }

        // === BULLET SYNCING ===
        // Send local bullets
        network->sendBullets(players[localIndex].getBullets());

        // Receive remote bullets
        std::vector<Bullet> remoteBullets;
        if (network->receiveBullets(remoteBullets)) {
            players[remoteIndex].setBullets(remoteBullets);
        }

        // === DAMAGE RECEIVING ===
        // Check for incoming damage from remote player
        int incomingDamage;
        if (network->receiveDamage(incomingDamage)) {
            players[localIndex].takeDamage(incomingDamage);
        }

        // === RESET SYNCHRONIZATION ===
        // Check for incoming reset from remote player
        if (network->receiveReset()) {
            reset();
        }

        // === UPDATE BULLETS ===
        players[localIndex].updateBullets();

        // === COLLISION DETECTION ===
        checkBulletCollisions(localIndex, remoteIndex);

        // === HEALTH SYNCING FOR DISPLAY ===
        // Send our health if it changed (after collision detection)
        if (players[localIndex].hasHealthChanged()) {
            network->sendHealth(players[localIndex].getHealth());
            players[localIndex].clearHealthChangeFlag();
        }

        // Send remote player health if we applied damage to them locally
        if (players[remoteIndex].hasHealthChanged()) {
            network->sendHealth(players[remoteIndex].getHealth());
            players[remoteIndex].clearHealthChangeFlag();
        }

        // Receive remote player's health for display
        int remoteHealth;
        if (network->receiveHealth(remoteHealth)) {
            players[remoteIndex].setHealth(remoteHealth);
            players[remoteIndex].clearHealthChangeFlag();  // Don't trigger another sync
        }

        // === DRAW PLAYERS AND BULLETS ===
        for (auto& player : players) {
            player.draw();
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
        if (!players[remoteIndex].isAlive()) {
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

void Game::reset() {
    // Reset player health
    for (auto& player : players) {
        player.setHealth(100);
        player.clearHealthChangeFlag();
    }

    // Clear all bullets
    std::vector<Bullet> emptyBullets;
    players[0].setBullets(emptyBullets);
    players[1].setBullets(emptyBullets);

    // Reset positions
    players[0].setPosition({Constants::SCREEN_WIDTH / 2 - 50, Constants::SCREEN_HEIGHT / 2});
    players[1].setPosition({Constants::SCREEN_WIDTH / 2 + 50, Constants::SCREEN_HEIGHT / 2});
}

void Game::checkBulletCollisions(int localIndex, int remoteIndex) {
    // Check if local bullets hit remote player
    auto& localBullets = const_cast<std::vector<Bullet>&>(players[localIndex].getBullets());
    for (auto it = localBullets.begin(); it != localBullets.end();) {
        if (players[remoteIndex].isCollidingWith(*it)) {
            // Apply damage immediately for instant visual feedback
            players[remoteIndex].takeDamage(10);
            // Send damage to them for their own tracking
            network->sendDamage(10);
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
    int localIndex = isHost ? 0 : 1;
    int remoteIndex = isHost ? 1 : 0;

    // Draw local player health with color based on health
    int localHealth = players[localIndex].getHealth();
    Color localHealthColor = localHealth > 50 ? GREEN : (localHealth > 25 ? YELLOW : RED);
    std::string localHealthText = "Your Health: " + std::to_string(localHealth) + "/100";
    DrawText(localHealthText.c_str(), 10, 10, 20, localHealthColor);

    // Draw enemy health with color based on health
    int remoteHealth = players[remoteIndex].getHealth();
    Color remoteHealthColor = remoteHealth > 50 ? GREEN : (remoteHealth > 25 ? YELLOW : RED);
    std::string remoteHealthText = "Enemy Health: " + std::to_string(remoteHealth) + "/100";
    DrawText(remoteHealthText.c_str(), 10, 40, 20, remoteHealthColor);

    // Draw health bars
    int barWidth = 200;
    int barHeight = 20;

    // Local player health bar
    DrawRectangle(10, 70, barWidth, barHeight, DARKGRAY);
    DrawRectangle(10, 70, (localHealth * barWidth) / 100, barHeight, localHealthColor);
    DrawRectangleLines(10, 70, barWidth, barHeight, BLACK);

    // Enemy health bar
    DrawRectangle(10, 100, barWidth, barHeight, DARKGRAY);
    DrawRectangle(10, 100, (remoteHealth * barWidth) / 100, barHeight, remoteHealthColor);
    DrawRectangleLines(10, 100, barWidth, barHeight, BLACK);
}
