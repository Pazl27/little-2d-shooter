#include "obstacle.hpp"

#include <algorithm>
#include <cmath>

// Base Obstacle class implementation
Obstacle::Obstacle(Position pos, Color clr, ObstacleType type) : position(pos), color(clr), type(type) {}

Position Obstacle::getPosition() const {
    return position;
}

Color Obstacle::getColor() const {
    return color;
}

ObstacleType Obstacle::getType() const {
    return type;
}

// RectangleObstacle implementation
RectangleObstacle::RectangleObstacle(Position pos, int w, int h, Color clr)
    : Obstacle(pos, clr, ObstacleType::RECTANGLE), width(w), height(h) {}

void RectangleObstacle::draw() const {
    DrawRectangle(position.x - width / 2, position.y - height / 2, width, height, color);
    DrawRectangleLines(position.x - width / 2, position.y - height / 2, width, height, BLACK);
}

bool RectangleObstacle::isCollidingWith(Position playerPos, int playerRadius) const {
    // Rectangle bounds
    int left = position.x - width / 2;
    int right = position.x + width / 2;
    int top = position.y - height / 2;
    int bottom = position.y + height / 2;

    // Find the closest point on the rectangle to the circle center
    int closestX = std::max(left, std::min(playerPos.x, right));
    int closestY = std::max(top, std::min(playerPos.y, bottom));

    // Calculate distance from circle center to closest point
    int dx = playerPos.x - closestX;
    int dy = playerPos.y - closestY;
    float distance = std::sqrt(dx * dx + dy * dy);

    return distance < playerRadius;
}

bool RectangleObstacle::isCollidingWithBullet(Position bulletPos, int bulletRadius) const {
    return isCollidingWith(bulletPos, bulletRadius);
}

int RectangleObstacle::getWidth() const {
    return width;
}

int RectangleObstacle::getHeight() const {
    return height;
}

// CircleObstacle implementation
CircleObstacle::CircleObstacle(Position pos, int rad, Color clr) : Obstacle(pos, clr, ObstacleType::CIRCLE), radius(rad) {}

void CircleObstacle::draw() const {
    DrawCircle(position.x, position.y, radius, color);
    DrawCircleLines(position.x, position.y, radius, BLACK);
}

bool CircleObstacle::isCollidingWith(Position playerPos, int playerRadius) const {
    int dx = playerPos.x - position.x;
    int dy = playerPos.y - position.y;
    float distance = std::sqrt(dx * dx + dy * dy);

    return distance < (radius + playerRadius);
}

bool CircleObstacle::isCollidingWithBullet(Position bulletPos, int bulletRadius) const {
    return isCollidingWith(bulletPos, bulletRadius);
}

int CircleObstacle::getRadius() const {
    return radius;
}