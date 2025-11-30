//
// Created by Piotr Pszczółkowski on 25/11/2025.
//

#include "client.h"
#include <print>
#include <string>
#include <vector>

#include "request.h"
#include "response.h"
#include "common/socket/connector.h"
#include "common/socket/logger.h"
#include "common/socket/socket.h"

using namespace std::string_literals;
using namespace bee::crypto;

std::vector<unsigned char> bytes(std::string_view const text) {
    return {text.begin(), text.end()};
}

bool openDatabase(Client const& client, String const& name) noexcept {
    auto const request = Request {
        .id = 123,
        .type = RequestType::Database,
        .subType = RequestSubType::Open,
        .value = name
    };

    if (auto answer = request.write(client)) {
        return answer.value().code == 0;
    } else {
        print_error(answer.error());
        return {};
    }
}

bool createDatabase(Client const& client, String const& name) noexcept {
    auto const request = Request {
        .id = 123,
        .type = RequestType::Database,
        .subType = RequestSubType::Create,
        .value = name,
    };

    if (auto answer = request.write(client)) {
        return answer.value().code == 0;
    } else {
        print_error(answer.error());
        return {};
    }
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

    std::println("Connecting...");
    if (auto const retv = client.connect("127.0.0.1" /*"192.168.50.210"*/, 123456)) {
        print_error(retv.value());
        return EXIT_FAILURE;
    }
    std::println("socket init.");
    if (!client.init()) {
        std::println(std::cerr, "Failed to initialize socket!");
        return EXIT_FAILURE;
    }

    std::println("Connected to server ({})", client.peerAddress());

    //===============================================================
    if (!openDatabase(client, "test.sqlite")) {
        if (!createDatabase(client, "test.sqlite")) {
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
