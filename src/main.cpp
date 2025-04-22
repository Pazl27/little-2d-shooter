#include <raylib.h>

#include <iostream>
#include <string>

#include "core/game.hpp"

int main(int argc, char** argv) {
    if (argc != 2) {
        std::cout << "Usage: " << argv[0] << " [host|client]" << std::endl;
        return 1;
    }

    std::string role = argv[1];
    Game game(role == "host");
    game.start();
    return 0;
}
