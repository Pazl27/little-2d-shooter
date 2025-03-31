#include "player.hpp"

#include <raylib.h>

#include <iostream>

#include "../../constants.hpp"

Player::Player(int spd, Color clr, int rad, PlayerShape shp) {
    position = {Constants::SCREEN_WIDTH / 2, Constants::SCREEN_HEIGHT / 2};
    speed = spd;
    color = clr;
    radius = rad;
    shape = shp;
}

Player::~Player() {
    // Destructor implementation
}

void Player::move() {
    const Position input = getInput();
    position.x += input.x * speed;
    position.y += input.y * speed;
}

void Player::attack() {
    std::cout << "Player is attacking" << std::endl;
}

void Player::draw() {
    if (shape == PlayerShape::CIRCLE) {
        DrawCircle(position.x, position.y, radius, color);
    } else if (shape == PlayerShape::SQUARE) {
        DrawRectangle(position.x - radius, position.y - radius, radius * 2, radius * 2, color);
    }
}

Position Player::getInput() const {
    Position input = {0, 0};
    if (IsKeyDown(KEY_W)) {
        input.y = -1;
    }
    if (IsKeyDown(KEY_S)) {
        input.y = 1;
    }
    if (IsKeyDown(KEY_A)) {
        input.x = -1;
    }
    if (IsKeyDown(KEY_D)) {
        input.x = 1;
    }
    return input;
}
