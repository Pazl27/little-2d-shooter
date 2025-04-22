#include "entities/player.hpp"

#include <raylib.h>

#include <algorithm>
#include <cmath>
#include <iostream>

#include "core/constants.hpp"

Player::Player(int spd, Color clr, int rad, PlayerShape shp) {
    position = {Constants::SCREEN_WIDTH / 2, Constants::SCREEN_HEIGHT / 2};
    speed = spd;
    color = clr;
    radius = rad;
    shape = shp;

    lastDirection = {0, -1};
    shootCooldown = 0.3f;
    timeSinceLastShot = shootCooldown;
}

Player::~Player() {
    // Destructor implementation
}

void Player::move() {
    timeSinceLastShot += GetFrameTime();

    Position input = getInput();

    if (input.x != 0 || input.y != 0) {
        lastDirection = input;
    }

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
    this->drawBullets();
}

Position Player::getInput() {
    Position input = {0, 0};
    if (IsKeyDown(KEY_W)) input.y = -1;
    if (IsKeyDown(KEY_S)) input.y = 1;
    if (IsKeyDown(KEY_A)) input.x = -1;
    if (IsKeyDown(KEY_D)) input.x = 1;

    if (IsKeyDown(KEY_SPACE)) {
        shoot();
    }

    return input;
}

void Player::shoot() {
    if (timeSinceLastShot >= shootCooldown) {
        Vector2 dir = {static_cast<float>(lastDirection.x), static_cast<float>(lastDirection.y)};

        float len = std::sqrt(dir.x * dir.x + dir.y * dir.y);
        if (len != 0) {
            dir.x /= len;
            dir.y /= len;
        }

        bullets.emplace_back(position, dir, 10.0f, color);
        timeSinceLastShot = 0.0f;
    }
}

void Player::updateBullets() {
    for (auto& b : bullets) b.update();
    bullets.erase(std::remove_if(bullets.begin(), bullets.end(), [](const Bullet& b) { return b.isOffScreen(); }), bullets.end());
}

void Player::drawBullets() const {
    for (const auto& b : bullets) b.draw();
}

const std::vector<Bullet>& Player::getBullets() const {
    return bullets;
}

Position Player::getPosition() const {
    return position;
}

void Player::setPosition(Position newPos) {
    position = newPos;
}
