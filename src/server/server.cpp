#include "server.hpp"
#include <cstring>
#include <enet/enet.h>
#include <iostream>

void runServer() {
  if (enet_initialize() != 0) {
    std::cerr << "Failed to initialize ENet." << std::endl;
    return;
  }
  atexit(enet_deinitialize);

  ENetAddress address;
  ENetHost *server;

  address.host = ENET_HOST_ANY;
  address.port = 1234;

  server = enet_host_create(&address, 32, 2, 0, 0);
  if (!server) {
    std::cerr << "Failed to create ENet server." << std::endl;
    return;
  }

  std::cout << "Server started on port 1234." << std::endl;

  while (true) {
    ENetEvent event;
    while (enet_host_service(server, &event, 1000) > 0) {
      switch (event.type) {
      case ENET_EVENT_TYPE_CONNECT: {
        std::cout << "A client connected." << std::endl;
        break;
      }
      case ENET_EVENT_TYPE_RECEIVE: {
        std::cout << "Message received: " << (char *)event.packet->data
                  << std::endl;
        enet_packet_destroy(event.packet);

        // Echo the message back to the client
        ENetPacket *packet = enet_packet_create(
            "Hello from server!", strlen("Hello from server!") + 1,
            ENET_PACKET_FLAG_RELIABLE);
        enet_peer_send(event.peer, 0, packet);
        break;
      }
      case ENET_EVENT_TYPE_DISCONNECT: {
        std::cout << "A client disconnected." << std::endl;
        break;
      }
      default:
        break;
      }
    }
  }

  enet_host_destroy(server);
}
