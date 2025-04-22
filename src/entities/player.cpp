#include "player.hpp"

#include <raylib.h>

#include <iostream>

#include "../core/constants.hpp"

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
    Position input = getInput();

    Position newPos = {position.x + input.x * speed, position.y + input.y * speed};

    newPos.x = std::max(radius, std::min(newPos.x, Constants::SCREEN_WIDTH - radius));
    newPos.y = std::max(radius, std::min(newPos.y, Constants::SCREEN_HEIGHT - radius));

    position = newPos;
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

Position Player::getPosition() const {
    return position;
}

void Player::setPosition(Position newPos) {
    position = newPos;
}
