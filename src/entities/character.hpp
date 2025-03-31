#ifndef CHARACTER_HPP
#define CHARACTER_HPP

class Character {
   public:
    virtual ~Character() = default;

    virtual void move() = 0;
    virtual void attack() = 0;
    virtual void draw() = 0;
};

#endif
