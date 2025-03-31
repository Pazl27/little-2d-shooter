#include "game.hpp"

#include <raylib.h>

#include <string>

Game::Game() : isRunning(false) {
    InitWindow(screenWidth, screenHeight, windowTitle.c_str());
    SetTargetFPS(60);
    players.push_back(Player());
}

Game::~Game() {
    CloseWindow();
}

void Game::start() {
    isRunning = true;
    while (isRunning) {
        BeginDrawing();
        ClearBackground(RAYWHITE);

        // Game logic and rendering goes here
        for (auto& player : players) {
            player.move();
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
    // Reset game state
}
