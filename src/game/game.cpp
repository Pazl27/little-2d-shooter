#include "game.hpp"

#include <raylib.h>

#include <string>

#include "../constants.hpp"

Game::Game() : isRunning(false) {
    InitWindow(Constants::SCREEN_WIDTH, Constants::SCREEN_HEIGHT, windowTitle.c_str());
    SetTargetFPS(60);
    Player player(5, BLUE, 10, PlayerShape::CIRCLE);
    players.push_back(player);
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
