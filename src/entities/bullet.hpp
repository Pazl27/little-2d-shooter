#ifndef BULLET_HPP
#define BULLET_HPP

#include <raylib.h>

#include "entities/position.hpp"

class Bullet {
   public:
    Bullet(Position startPos, Vector2 dir, float speed, Color color);

    void update();
    void draw() const;
    bool isOffScreen() const;
    Position getPosition() const;

   private:
    Position position;
    Vector2 direction;
    float speed;
    Color color;
    float radius = 4;
};

#endif
