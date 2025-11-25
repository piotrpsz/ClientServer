//
// Created by Piotr Pszczolkowski on 25/11/2025.
//

#include "server.h"
#include "shared/socket/socket.h"
#include <print>

void clientHandler(int const fd) {
    Socket const socket{fd};
    std::println("Client connected ({})", socket.peerAddress());
}

[[noreturn]] int main() {
    if (Socket const s{}; s.bind(12345) and s.listen()) {
        std::println("Server waiting for connection ({})", s.hostAddress());
        for (;;) {
            if (auto const fd = s.accept()) {
                clientHandler(fd.value());
            }
        }
    }
}
