#ifndef PLAYER_HPP
#define PLAYER_HPP

#include <raylib.h>

#include <vector>

#include "entities/bullet.hpp"
#include "entities/character.hpp"
#include "entities/position.hpp"

// Forward declaration
class Map;

enum class PlayerShape {
    CIRCLE,
    SQUARE,
};

class Player : public Character {
   public:
    Player(int spd, Color clr, int rad, PlayerShape shp);
    ~Player() override;

    void move() override;
    void move(const Map* map);  // Overloaded move with collision detection
    void attack() override;
    void draw() override;

    Position getPosition() const;
    void setPosition(Position newPos);

    void shoot();
    void updateBullets();
    void updateBullets(const Map* map);  // Overloaded updateBullets with collision detection
    void drawBullets() const;
    const std::vector<Bullet>& getBullets() const;
    void setBullets(const std::vector<Bullet>& newBullets);

    // Health system
    int getHealth() const;
    void setHealth(int newHealth);
    void takeDamage(int damage);
    bool isAlive() const;
    bool hasHealthChanged() const;
    void clearHealthChangeFlag();

    // Collision detection
    bool isCollidingWith(const Bullet& bullet) const;
    int getRadius() const;

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

    // Health properties
    int health;
    int maxHealth;
    mutable bool healthChanged;
};

#endif
