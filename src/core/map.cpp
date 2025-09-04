#include "map.hpp"

#include <raylib.h>

#include "core/constants.hpp"
#include "entities/obstacle.hpp"

Map::Map() {
    initializeObstacles();
}

Map::~Map() {
    clearObstacles();
}

void Map::initializeObstacles() {
    clearObstacles();
    createDefaultObstacles();
}

void Map::createDefaultObstacles() {
    // Create border walls
    // Top wall
    addObstacle(std::make_unique<RectangleObstacle>(Position{Constants::SCREEN_WIDTH / 2, 15}, Constants::SCREEN_WIDTH, 30, GRAY));

    // Bottom wall
    addObstacle(std::make_unique<RectangleObstacle>(Position{Constants::SCREEN_WIDTH / 2, Constants::SCREEN_HEIGHT - 15},
                                                    Constants::SCREEN_WIDTH, 30, GRAY));

    // Left wall
    addObstacle(std::make_unique<RectangleObstacle>(Position{15, Constants::SCREEN_HEIGHT / 2}, 30, Constants::SCREEN_HEIGHT, GRAY));

    // Right wall
    addObstacle(std::make_unique<RectangleObstacle>(Position{Constants::SCREEN_WIDTH - 15, Constants::SCREEN_HEIGHT / 2}, 30,
                                                    Constants::SCREEN_HEIGHT, GRAY));

    // Interior obstacles for interesting gameplay
    // Central pillar
    addObstacle(std::make_unique<CircleObstacle>(Position{Constants::SCREEN_WIDTH / 2, Constants::SCREEN_HEIGHT / 2}, 40, DARKGRAY));

    // Corner blocks
    addObstacle(std::make_unique<RectangleObstacle>(Position{150, 150}, 60, 80, BROWN));

    addObstacle(std::make_unique<RectangleObstacle>(Position{Constants::SCREEN_WIDTH - 150, 150}, 60, 80, BROWN));

    addObstacle(std::make_unique<RectangleObstacle>(Position{150, Constants::SCREEN_HEIGHT - 150}, 60, 80, BROWN));

    addObstacle(
        std::make_unique<RectangleObstacle>(Position{Constants::SCREEN_WIDTH - 150, Constants::SCREEN_HEIGHT - 150}, 60, 80, BROWN));

    // Side obstacles
    addObstacle(std::make_unique<CircleObstacle>(Position{Constants::SCREEN_WIDTH / 4, Constants::SCREEN_HEIGHT / 2}, 25, DARKGREEN));

    addObstacle(std::make_unique<CircleObstacle>(Position{3 * Constants::SCREEN_WIDTH / 4, Constants::SCREEN_HEIGHT / 2}, 25, DARKGREEN));

    // Additional rectangular obstacles for cover
    addObstacle(
        std::make_unique<RectangleObstacle>(Position{Constants::SCREEN_WIDTH / 2, Constants::SCREEN_HEIGHT / 4}, 100, 30, DARKBLUE));

    addObstacle(
        std::make_unique<RectangleObstacle>(Position{Constants::SCREEN_WIDTH / 2, 3 * Constants::SCREEN_HEIGHT / 4}, 100, 30, DARKBLUE));
}

void Map::draw() const {
    for (const auto& obstacle : obstacles) {
        obstacle->draw();
    }
}

bool Map::isPlayerColliding(Position playerPos, int playerRadius) const {
    for (const auto& obstacle : obstacles) {
        if (obstacle->isCollidingWith(playerPos, playerRadius)) {
            return true;
        }
    }
    return false;
}

bool Map::isBulletColliding(Position bulletPos, int bulletRadius) const {
    for (const auto& obstacle : obstacles) {
        if (obstacle->isCollidingWithBullet(bulletPos, bulletRadius)) {
            return true;
        }
    }
    return false;
}

void Map::addObstacle(std::unique_ptr<Obstacle> obstacle) {
    obstacles.push_back(std::move(obstacle));
}

void Map::clearObstacles() {
    obstacles.clear();
}

const std::vector<std::unique_ptr<Obstacle>>& Map::getObstacles() const {
    return obstacles;
}