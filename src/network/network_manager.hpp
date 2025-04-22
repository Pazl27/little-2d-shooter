#ifndef NETWORK_MANAGER_HPP
#define NETWORK_MANAGER_HPP

#include <enet/enet.h>

#include <vector>

#include "entities/bullet.hpp"

class NetworkManager {
   public:
    NetworkManager(bool isHost);
    ~NetworkManager();
    bool init();
    void sendPosition(float x, float y);
    bool receivePosition(float& x, float& y);

    // New methods for sending and receiving bullets
    void sendBullets(const std::vector<Bullet>& bullets);
    bool receiveBullets(std::vector<Bullet>& bullets);

   private:
    bool isHost;
    ENetHost* host;
    ENetPeer* peer;
};

#endif
