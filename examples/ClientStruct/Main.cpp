#include <iostream>
#include <thread>
#include <chrono>
#include "GameClient.h"

int main() {
    std::cout << "Starting Game Client (Struct)..." << std::endl;

    GameClient::GameClient client("127.0.0.1", 4242, 1);
    client.Start();

    while (true) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    return 0;
}

