//
// Created by Piotr Pszczółkowski on 25/11/2025.
//

#include "client.h"
#include <print>
#include <string>
#include <vector>

#include "request.h"
#include "response.h"
#include "shared/socket/connector.h"
#include "shared/socket/logger.h"
#include "shared/socket/socket.h"

using namespace std::string_literals;
using namespace bee::crypto;

std::vector<unsigned char> bytes(std::string_view const text) {
    return {text.begin(), text.end()};
}

bool openDatabase(Client const& client, String name) noexcept {
    auto request = Request {
        .id = 123,
        .type = RequestType::Database,
        .subType = RequestSubType::Open,
        .content = As<std::vector<u8>>("test.sqlite"s)
    };

    std::println("Sending request: {}", request);
    if (auto const retv = client.write_text(request.toJSON()); !retv) {
        print_error(retv.error());
        return {};
    }

    auto const answer = client.read_text();
    if (!answer) {
        print_error(answer.error());
        return {};
    }
    auto response = Response::fromJSON(answer.value());
    if (response) {
        std::println("Received response: {}", response.value());
        return response->code == 0;
    }
    return {};
}

bool createDatabase(Client const& client, String name) noexcept {
    auto request = Request {
        .id = 123,
        .type = RequestType::Database,
        .subType = RequestSubType::Create,
        .content = As<std::vector<u8>>("test.sqlite"s)
    };

    std::println("Sending request: {}", request);
    if (auto const retv = client.write_text(request.toJSON()); !retv) {
        print_error(retv.error());
        return {};
    }

    auto const answer = client.read_text();
    if (!answer) {
        print_error(answer.error());
        return {};
    }
    if (auto response = Response::fromJSON(answer.value())) {
        std::println("Received response: {}", response.value());
        return response->code == 0;
    }
    return {};
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
        std::println(std::cerr, "Failed to initialize socket!");
        return EXIT_FAILURE;
    }

    std::println("Connected to server ({})", client.peerAddress());

    //===============================================================
    if (!openDatabase(client, "test.sqlite")) {
        std::println("Failed to open database!");
        if (!createDatabase(client, "test.sqlite")) {
            std::println("Failed to create database!");
            return EXIT_FAILURE;
        }
    }


    // for (auto const& line : data) {
    //     auto item = bytes(line);
    //     if (auto const retv = client.write(item); !retv) {
    //         print_error(retv.error());
    //         return EXIT_FAILURE;
    //     }
    //     std::println("Sent package: {}", line);
    //
    //     auto const answer = client.read();
    //     if (!answer) {
    //         print_error(answer.error());
    //         return EXIT_FAILURE;
    //     }
    //     std::println("Received answer: {}", As<String>(answer.value()));
    // }

    return EXIT_SUCCESS;
}
