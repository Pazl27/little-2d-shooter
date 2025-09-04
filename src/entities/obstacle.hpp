#ifndef OBSTACLE_HPP
#define OBSTACLE_HPP

#include <raylib.h>

#include "entities/position.hpp"

enum class ObstacleType { RECTANGLE, CIRCLE };

class Obstacle {
   public:
    Obstacle(Position pos, Color clr, ObstacleType type);
    virtual ~Obstacle() = default;

    virtual void draw() const = 0;
    virtual bool isCollidingWith(Position playerPos, int playerRadius) const = 0;
    virtual bool isCollidingWithBullet(Position bulletPos, int bulletRadius) const = 0;

    Position getPosition() const;
    Color getColor() const;
    ObstacleType getType() const;

   protected:
    Position position;
    Color color;
    ObstacleType type;
};

class RectangleObstacle : public Obstacle {
   public:
    RectangleObstacle(Position pos, int w, int h, Color clr);

    void draw() const override;
    bool isCollidingWith(Position playerPos, int playerRadius) const override;
    bool isCollidingWithBullet(Position bulletPos, int bulletRadius) const override;

    int getWidth() const;
    int getHeight() const;

   private:
    int width;
    int height;
};

class CircleObstacle : public Obstacle {
   public:
    CircleObstacle(Position pos, int rad, Color clr);

    void draw() const override;
    bool isCollidingWith(Position playerPos, int playerRadius) const override;
    bool isCollidingWithBullet(Position bulletPos, int bulletRadius) const override;

    int getRadius() const;

   private:
    int radius;
};

#endif