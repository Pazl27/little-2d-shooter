#include "game.hpp"

#include <raylib.h>

#include <stdexcept>
#include <string>

#include "../network/network_manager.hpp"
#include "constants.hpp"

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

        players[localIndex].move();
        Position localPos = players[localIndex].getPosition();
        network->sendPosition(localPos.x, localPos.y);

        float rx, ry;
        if (network->receivePosition(rx, ry)) {
            players[remoteIndex].setPosition({static_cast<int>(rx), static_cast<int>(ry)});
        }

        // Draw both players
        for (auto& player : players) {
            player.draw();
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
    // Reset game state if needed
}
