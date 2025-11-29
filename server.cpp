//
// Created by Piotr Pszczolkowski on 25/11/2025.
//

#include "server.h"
#include "shared/socket/all.hpp"
#include <iostream>
#include <print>
#include <atomic>

#include "request.h"

using namespace bee::crypto;


std::atomic_bool running{true};



void clientHandler(int const fd) {
    Server server{fd};
    if (!server.init()) {
        std::println(std::cerr, "Failed to initialize server socket!");
        return;
    }

    std::println("Client connected ({})", server.peerAddress());

    auto const request = server.read_text();
    if (!request) {
        print_error(request.error());
        return;
    }
    auto req = Request::fromJSON(request.value());
    std::println("Received request: {}", req.value());


    // while (true) {
    //     auto const request = server.read();
    //     if (!request) {
    //         print_error(request.error());
    //         break;
    //     }
    //     auto text = As<String>(request.value());
    //     std::println("-- Received package: {}", text);
    //
    //     auto output = As<Vector<u8>>("Witaj: " + text + "!");
    //     if (auto const answer = server.write(output); !answer) {
    //         print_error(answer.error());
    //         break;
    //     }
    // }
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
