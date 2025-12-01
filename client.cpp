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
#include "request.h"
#include "response.h"
#include "common/socket/connector.h"
#include "common/socket/logger.h"
#include "common/socket/socket.h"
#include <print>
#include <string>
#include <vector>

using namespace std::string_literals;
using namespace bee;
using namespace bee::crypto;

bool openDatabase(Client const& client, String const& name) noexcept {
    auto const request = Request {
        .id = 123,
        .type = Database,
        .subType = Open,
        .value = name
    };

    if (auto const answer = request.write(client)) {
        return answer->code == 0;
    } else {
        print_error(answer.error());
        return {};
    }
}

bool createDatabase(Client const& client, String const& name) noexcept {
    auto const request = Request {
        .id = 123,
        .type = Database,
        .subType = Create,
        .value = name,
    };

    if (auto answer = request.write(client)) {
        return answer->code == 0;
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


    return EXIT_SUCCESS;
}
