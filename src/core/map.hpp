#ifndef MAP_HPP
#define MAP_HPP

#include <memory>
#include <vector>

#include "entities/obstacle.hpp"
#include "entities/position.hpp"

class Map {
   public:
    Map();
    ~Map();

    void initializeObstacles();
    void draw() const;

    // Collision detection methods
    bool isPlayerColliding(Position playerPos, int playerRadius) const;
    bool isBulletColliding(Position bulletPos, int bulletRadius) const;

    // Obstacle management
    void addObstacle(std::unique_ptr<Obstacle> obstacle);
    void clearObstacles();

    const std::vector<std::unique_ptr<Obstacle>>& getObstacles() const;

   private:
    std::vector<std::unique_ptr<Obstacle>> obstacles;

    void createDefaultObstacles();
};

#endif