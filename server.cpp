//
// Created by Piotr Pszczolkowski on 25/11/2025.
//

#include "server.h"
#include "shared/socket/all.hpp"
#include <iostream>
#include <print>
#include <atomic>

std::atomic_bool running{true};



void clientHandler(int const fd) {
    Server server{fd};
    if (!server.init()) {
        std::println(std::cerr, "Failed to initialize server socket!");
        return;
    }

    // TODO tutaj wymiana kluczy szyfrujących.
    std::println("Client connected ({})", server.peerAddress());



    while (true) {
        auto const request = server.readText();
        if (!request) {
            print_error(request.error());
            break;
         }
        std::println("-- Received package: {}", request.value());

        auto const answer = server.writeText("Witaj: " + request.value() + "!");
        if (!answer) {
            print_error(answer.error());
            break;
        }
    }
    std::println("Client disconnected ({})", server.peerAddress());
}

int main() {
    Server const server{};

    if (auto const retv = server.run(12345); !retv) {
        print_error(retv.error());
        exit(EXIT_FAILURE);
    }

    std::println("Server waiting for connection ({})", server.hostAddress());

    while (running) {
        if (auto const fd = server.accept()) {
            clientHandler(fd.value());
        }
    }

    return EXIT_SUCCESS;
}
