#include "bullet.hpp"

#include <cstring>

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

std::vector<uint8_t> Bullet::serialize() const {
    std::vector<uint8_t> buffer;

    // Serialize position
    buffer.insert(buffer.end(), reinterpret_cast<const uint8_t*>(&position),
                  reinterpret_cast<const uint8_t*>(&position) + sizeof(position));

    // Serialize direction
    buffer.insert(buffer.end(), reinterpret_cast<const uint8_t*>(&direction),
                  reinterpret_cast<const uint8_t*>(&direction) + sizeof(direction));

    // Serialize speed
    buffer.insert(buffer.end(), reinterpret_cast<const uint8_t*>(&speed), reinterpret_cast<const uint8_t*>(&speed) + sizeof(speed));

    // Serialize color
    buffer.insert(buffer.end(), reinterpret_cast<const uint8_t*>(&color), reinterpret_cast<const uint8_t*>(&color) + sizeof(color));

    return buffer;
}

Bullet Bullet::deserialize(const uint8_t* data, size_t& offset) {
    Position pos;
    Vector2 dir;
    float spd;
    Color clr;

    // Deserialize position
    std::memcpy(&pos, data + offset, sizeof(pos));
    offset += sizeof(pos);

    // Deserialize direction
    std::memcpy(&dir, data + offset, sizeof(dir));
    offset += sizeof(dir);

    // Deserialize speed
    std::memcpy(&spd, data + offset, sizeof(spd));
    offset += sizeof(spd);

    // Deserialize color
    std::memcpy(&clr, data + offset, sizeof(clr));
    offset += sizeof(clr);

    return Bullet(pos, dir, spd, clr);
}
