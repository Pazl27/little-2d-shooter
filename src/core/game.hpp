#ifndef GAME_HPP
#define GAME_HPP

#include <string>
#include <vector>

#include "entities/player.hpp"
#include "network/network_manager.hpp"

class Game {
   public:
    Game(bool isHost);
    ~Game();

    void start();
    void stop();
    void reset();

   private:
    void checkBulletCollisions(int localIndex, int remoteIndex);
    void drawHealth();

    bool isRunning;
    const std::string windowTitle = "2d-shooter";
    bool isHost;
    NetworkManager* network;
    std::vector<Player> players;
};

#endif
