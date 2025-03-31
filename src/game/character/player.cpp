#include "player.hpp"

#include <raylib.h>

#include <iostream>

Player::Player() {
    position.x = 0;
    position.y = 0;
    speed = 5;
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
    DrawCircle(position.x, position.y, 10, RED);
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
