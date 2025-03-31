#include <raylib.h>

#include <iostream>
#include <string>

#include "client/client.hpp"
#include "game/game.hpp"
#include "server/server.hpp"

int main() {
    // std::string choice;
    // std::cout << "Enter 'server' to run as server or 'client' to run as client: ";
    // std::cin >> choice;
    //
    // if (choice == "server") {
    //     runServer();
    // } else if (choice == "client") {
    //     runClient();
    // } else {
    //     std::cerr << "Invalid choice. Please enter 'server' or 'client'." << std::endl;
    //     return EXIT_FAILURE;
    // }

    Game game;
    game.start();

    return EXIT_SUCCESS;
}
