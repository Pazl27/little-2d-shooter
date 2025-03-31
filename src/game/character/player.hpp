#ifndef PLAYER_HPP
#define PLAYER_HPP

#include <raylib.h>

#include "character.hpp"

enum class PlayerShape {
    CIRCLE,
    SQUARE,
};

struct Position {
    int x;
    int y;
};

class Player : public Character {
   public:
    Player(int spd, Color clr, int rad, PlayerShape shp);
    ~Player() override;

    void move() override;
    void attack() override;
    void draw() override;

   protected:
    Position getInput() const;

    Position position;
    int speed;
    int radius;
    Color color;
    PlayerShape shape;
};

#endif
