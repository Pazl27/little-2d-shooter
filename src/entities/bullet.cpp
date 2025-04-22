#include "bullet.hpp"

#include "core/constants.hpp"
#include "raymath.h"

Bullet::Bullet(Position startPos, Vector2 dir, float spd, Color clr)
    : position(startPos), direction(Vector2Normalize(dir)), speed(spd), color(clr) {}

void Bullet::update() {
    position.x += direction.x * speed;
    position.y += direction.y * speed;
}

void Bullet::draw() const {
    DrawCircle(position.x, position.y, radius, color);
}

bool Bullet::isOffScreen() const {
    return position.x < 0 || position.x > Constants::SCREEN_WIDTH || position.y < 0 || position.y > Constants::SCREEN_HEIGHT;
}

Position Bullet::getPosition() const {
    return position;
}
