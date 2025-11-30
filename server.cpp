//
// Created by Piotr Pszczolkowski on 25/11/2025.
//

#include "server.h"
#include "common/socket/all.hpp"
#include <iostream>
#include <print>
#include <atomic>
#include "request.h"
#include "server/handler.h"

using namespace bee::crypto;


std::atomic_bool running{true};


void clientHandler(int const fd) {
    Server server{fd};
    if (!server.init()) {
        std::println(std::cerr, "Failed to initialize server socket!");
        return;
    }

    std::println("------- Client connected: {} -------", server.peerAddress());

    while (true) {
        auto request = Request::read(server);
        if (!request) {
            print_error(request.error());
            break;
        }
        auto response = handleRequest(std::move(request.value()));
        if (auto const err = response.write(server)) {
            print_error(err.value());
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
