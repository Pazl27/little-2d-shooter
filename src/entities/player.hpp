#ifndef PLAYER_HPP
#define PLAYER_HPP

#include <raylib.h>

#include <vector>

#include "entities/bullet.hpp"
#include "entities/character.hpp"
#include "entities/position.hpp"

enum class PlayerShape {
    CIRCLE,
    SQUARE,
};

class Player : public Character {
   public:
    Player(int spd, Color clr, int rad, PlayerShape shp);
    ~Player() override;

    void move() override;
    void attack() override;
    void draw() override;

    Position getPosition() const;
    void setPosition(Position newPos);

    void shoot();
    void updateBullets();
    void drawBullets() const;
    const std::vector<Bullet>& getBullets() const;
    void setBullets(const std::vector<Bullet>& newBullets);

   protected:
    Position getInput();

    Position position;
    int speed;
    int radius;
    Color color;
    PlayerShape shape;

    Position lastDirection;
    float shootCooldown;
    float timeSinceLastShot;

    std::vector<Bullet> bullets;
};

#endif
