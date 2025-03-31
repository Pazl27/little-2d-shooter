#include "client/client.hpp"
#include "server/server.hpp"
#include <iostream>
#include <string>

int main() {
  std::string choice;
  std::cout << "Enter 'server' to run as server or 'client' to run as client: ";
  std::cin >> choice;

  if (choice == "server") {
    runServer();
  } else if (choice == "client") {
    runClient();
  } else {
    std::cerr << "Invalid choice. Please enter 'server' or 'client'."
              << std::endl;
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
