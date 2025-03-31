#ifndef GAME_HPP
#define GAME_HPP

#include <string>
#include <vector>

#include "character/player.hpp"

class Game {
   public:
    Game();
    ~Game();

    void start();
    void stop();
    void reset();

   private:
    bool isRunning;
    const int screenWidth = 800;
    const int screenHeight = 600;
    const std::string windowTitle = "2d-shooter";
    std::vector<Player> players;
};

#endif
