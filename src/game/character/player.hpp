#ifndef PLAYER_HPP
#define PLAYER_HPP

#include "character.hpp"

struct Position {
    int x;
    int y;
};

class Player : public Character {
   public:
    Player();
    ~Player() override;

    void move() override;
    void attack() override;
    void draw() override;

   protected:
    Position getInput() const;

    Position position;
    int speed;
};

#endif
