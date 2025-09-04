#include "network/network_manager.hpp"

#include <cstring>
#include <iostream>

NetworkManager::NetworkManager(bool hostFlag) : isHost(hostFlag), host(nullptr), peer(nullptr) {}

NetworkManager::~NetworkManager() {
    if (host) enet_host_destroy(host);
    enet_deinitialize();
}

bool NetworkManager::init() {
    if (enet_initialize() != 0) {
        std::cerr << "ENet failed to initialize." << std::endl;
        return false;
    }

    if (isHost) {
        ENetAddress address;
        address.host = ENET_HOST_ANY;
        address.port = 1234;

        host = enet_host_create(&address, 2, 2, 0, 0);
        std::cout << "Hosting on port 1234..." << std::endl;
    } else {
        host = enet_host_create(nullptr, 1, 2, 0, 0);
        ENetAddress address;
        enet_address_set_host(&address, "localhost");  // replace with IP later
        address.port = 1234;

        peer = enet_host_connect(host, &address, 2, 0);
        std::cout << "Connecting to server..." << std::endl;
    }

    return host != nullptr;
}

void NetworkManager::sendPosition(float x, float y) {
    float pos[2] = {x, y};
    ENetPacket* packet = enet_packet_create(pos, sizeof(pos), ENET_PACKET_FLAG_RELIABLE);

    if (isHost && peer) {
        enet_peer_send(peer, 0, packet);
    } else if (!isHost) {
        enet_peer_send(peer, 0, packet);
    }

    enet_host_flush(host);
}

bool NetworkManager::receivePosition(float& x, float& y) {
    ENetEvent event;
    while (enet_host_service(host, &event, 0) > 0) {
        if (event.type == ENET_EVENT_TYPE_RECEIVE) {
            if (event.packet->dataLength == sizeof(float) * 2) {
                float* data = reinterpret_cast<float*>(event.packet->data);
                x = data[0];
                y = data[1];
                enet_packet_destroy(event.packet);
                return true;
            }
            enet_packet_destroy(event.packet);
        } else if (event.type == ENET_EVENT_TYPE_CONNECT) {
            peer = event.peer;
            std::cout << "Peer connected!" << std::endl;
        }
    }
    return false;
}

void NetworkManager::sendBullets(const std::vector<Bullet>& bullets) {
    std::vector<uint8_t> buffer;

    for (const Bullet& bullet : bullets) {
        std::vector<uint8_t> serializedBullet = bullet.serialize();
        buffer.insert(buffer.end(), serializedBullet.begin(), serializedBullet.end());
    }

    ENetPacket* packet = enet_packet_create(buffer.data(), buffer.size(), ENET_PACKET_FLAG_RELIABLE);

    if (isHost && peer) {
        enet_peer_send(peer, 0, packet);
    } else if (!isHost) {
        enet_peer_send(peer, 0, packet);
    }

    enet_host_flush(host);
}

bool NetworkManager::receiveBullets(std::vector<Bullet>& bullets) {
    ENetEvent event;
    while (enet_host_service(host, &event, 0) > 0) {
        if (event.type == ENET_EVENT_TYPE_RECEIVE) {
            const uint8_t* data = event.packet->data;
            size_t offset = 0;

            while (offset < event.packet->dataLength) {
                bullets.push_back(Bullet::deserialize(data, offset));
            }

            enet_packet_destroy(event.packet);
            return true;
        } else if (event.type == ENET_EVENT_TYPE_CONNECT) {
            peer = event.peer;
            std::cout << "Peer connected!" << std::endl;
        }
    }
    return false;
}

void NetworkManager::sendDamage(int damage) {
    ENetPacket* packet = enet_packet_create(&damage, sizeof(int), ENET_PACKET_FLAG_RELIABLE);

    if (isHost && peer) {
        enet_peer_send(peer, 1, packet);
    } else if (!isHost) {
        enet_peer_send(peer, 1, packet);
    }

    enet_host_flush(host);
}

bool NetworkManager::receiveDamage(int& damage) {
    ENetEvent event;
    while (enet_host_service(host, &event, 0) > 0) {
        if (event.type == ENET_EVENT_TYPE_RECEIVE && event.channelID == 1) {
            if (event.packet->dataLength == sizeof(int)) {
                int* data = reinterpret_cast<int*>(event.packet->data);
                damage = *data;
                enet_packet_destroy(event.packet);
                return true;
            }
            enet_packet_destroy(event.packet);
        } else if (event.type == ENET_EVENT_TYPE_CONNECT) {
            peer = event.peer;
            std::cout << "Peer connected!" << std::endl;
        }
    }
    return false;
}

bool NetworkManager::isConnected() const {
    return peer != nullptr;
}

void NetworkManager::sendHealth(int health) {
    ENetPacket* packet = enet_packet_create(&health, sizeof(int), ENET_PACKET_FLAG_RELIABLE);

    if (isHost && peer) {
        enet_peer_send(peer, 2, packet);  // Use channel 2 for health
    } else if (!isHost) {
        enet_peer_send(peer, 2, packet);
    }

    enet_host_flush(host);
}

bool NetworkManager::receiveHealth(int& health) {
    ENetEvent event;
    while (enet_host_service(host, &event, 0) > 0) {
        if (event.type == ENET_EVENT_TYPE_RECEIVE && event.channelID == 2) {
            if (event.packet->dataLength == sizeof(int)) {
                int* data = reinterpret_cast<int*>(event.packet->data);
                health = *data;
                enet_packet_destroy(event.packet);
                return true;
            }
            enet_packet_destroy(event.packet);
        } else if (event.type == ENET_EVENT_TYPE_CONNECT) {
            peer = event.peer;
            std::cout << "Peer connected!" << std::endl;
        }
    }
    return false;
}

void NetworkManager::sendReset() {
    int resetSignal = 1;
    ENetPacket* packet = enet_packet_create(&resetSignal, sizeof(int), ENET_PACKET_FLAG_RELIABLE);

    if (isHost && peer) {
        enet_peer_send(peer, 3, packet);  // Use channel 3 for reset
    } else if (!isHost) {
        enet_peer_send(peer, 3, packet);
    }

    enet_host_flush(host);
}

bool NetworkManager::receiveReset() {
    ENetEvent event;
    while (enet_host_service(host, &event, 0) > 0) {
        if (event.type == ENET_EVENT_TYPE_RECEIVE && event.channelID == 3) {
            if (event.packet->dataLength == sizeof(int)) {
                enet_packet_destroy(event.packet);
                return true;
            }
            enet_packet_destroy(event.packet);
        } else if (event.type == ENET_EVENT_TYPE_CONNECT) {
            peer = event.peer;
            std::cout << "Peer connected!" << std::endl;
        }
    }
    return false;
}
