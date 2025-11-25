//
// Created by Piotr Pszczolkowski on 25/11/2025.
//

#include "client.h"
#include <print>

#include "shared/socket/socket.h"

int main() {
    std::println("Client is running!");
    if (Socket const s{}; s.connect("127.0.0.1", 12345)) {
        std::println("Connected to server ({})", s.peerAddress());
    }

}
