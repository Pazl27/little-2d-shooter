#ifndef NETWORK_MANAGER_HPP
#define NETWORK_MANAGER_HPP

#include <enet/enet.h>

class NetworkManager {
   public:
    NetworkManager(bool isHost);
    ~NetworkManager();
    bool init();
    void sendPosition(float x, float y);
    bool receivePosition(float& x, float& y);

   private:
    bool isHost;
    ENetHost* host;
    ENetPeer* peer;
};

#endif
