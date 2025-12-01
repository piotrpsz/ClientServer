// MIT License
//
// Copyright (c) 2024 Piotr Pszczółkowski
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
//
// Author: Piotr Pszczółkowski
// Date: 25.11.2025
// E-mail: piotr@beesoft.pl.

/*------- include files:
-------------------------------------------------------------------*/
#include "common/socket/all.hpp"
#include <iostream>
#include <print>
#include <atomic>
#include "request.h"
#include "server/handler.h"

using namespace bee;
using namespace bee::crypto;


std::atomic_bool running{true};


void clientHandler(int const fd) {
    Server server{fd};
    std::println("server init");
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

    if (auto const err = server.run(123456)) {
        print_error(err.value());
        exit(EXIT_FAILURE);
    }

    std::println("Server waiting for connection ({})", server.hostAddress());

    while (running) {
        std::println("Waiting for connection...");
        if (auto const fd = server.accept()) {
            clientHandler(fd.value());
        }
    }

    return EXIT_SUCCESS;
}
