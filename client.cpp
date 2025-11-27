//
// Created by Piotr Pszczółkowski on 25/11/2025.
//

#include "client.h"
#include <print>
#include <string>
#include <vector>
#include "shared/socket/connector.h"
#include "shared/socket/logger.h"
#include "shared/socket/socket.h"

using namespace std::string_literals;

std::vector<char> bytes(std::string_view const text) {
    return {text.begin(), text.end()};
}

int main() {
    std::println("Client is running!");

    std::vector<std::string> data {
        "Piotr Pszczółkowski",
        "Jola Pszczółkowska",
        "Artur Pszczółkowski",
        "Błażej Pszczółkowski",
        "Niko & Kacpi & Adaś"
    };

    Client client{};
    if (auto const retv = client.connect("127.0.0.1", 12345); !retv) {
        print_error(retv.error());
        return EXIT_FAILURE;
    }
    if (!client.init()) {
        std::println("Failed to initialize socket!");
        return EXIT_FAILURE;
    }

    std::println("Connected to server ({})", client.peerAddress());

    for (auto const& line : data) {
        if (auto const retv = client.writeText(line); !retv) {
            print_error(retv.error());
            return EXIT_FAILURE;
        }
        std::println("Sent package: {}", line);

        auto const answer = client.readText();
        if (!answer) {
            print_error(answer.error());
            return EXIT_FAILURE;
        }
        std::println("Received answer: {}", answer.value());
    }

    return EXIT_SUCCESS;
}
