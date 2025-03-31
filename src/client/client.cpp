#include "client.hpp"
#include <enet/enet.h>
#include <iostream>
#include <cstring>

void runClient() {
    if (enet_initialize() != 0) {
        std::cerr << "Failed to initialize ENet." << std::endl;
        return;
    }
    atexit(enet_deinitialize);

    ENetHost *client = enet_host_create(nullptr, 1, 2, 0, 0);
    if (!client) {
        std::cerr << "Failed to create ENet client." << std::endl;
        return;
    }

    ENetAddress address;
    ENetPeer *peer;

    enet_address_set_host(&address, "127.0.0.1");
    address.port = 1234;

    peer = enet_host_connect(client, &address, 2, 0);
    if (!peer) {
        std::cerr << "Failed to connect to server." << std::endl;
        enet_host_destroy(client);
        return;
    }

    std::cout << "Connected to server." << std::endl;

    ENetEvent event;
    while (enet_host_service(client, &event, 5000) > 0) {
        switch (event.type) {
            case ENET_EVENT_TYPE_CONNECT: {
                std::cout << "Connection to server succeeded." << std::endl;

                // Send a message to the server
                ENetPacket *packet =
                    enet_packet_create("Hello from client!", strlen("Hello from client!") + 1,
                                       ENET_PACKET_FLAG_RELIABLE);
                enet_peer_send(peer, 0, packet);
                break;
            }
            case ENET_EVENT_TYPE_RECEIVE: {
                std::cout << "Message from server: " << (char *)event.packet->data << std::endl;
                enet_packet_destroy(event.packet);
                break;
            }
            case ENET_EVENT_TYPE_DISCONNECT: {
                std::cout << "Disconnected from server." << std::endl;
                break;
            }
            default:
                break;
        }
    }

    enet_host_destroy(client);
}
