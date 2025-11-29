//
// Created by Piotr Pszczolkowski on 29/11/2025.
//

#include "handler.h"
#include "../shared/pub.h"
#include "../sqlite4cx/sqlite4cx.hpp"
#include <ranges>
#include <algorithm>
#include <format>

namespace rg = std::ranges;
namespace rv = std::ranges::views;
using namespace bee;


static Response handleDatabaseRequest(Request&& request);

Response handleRequest(Request &&request) {
    std::println("Received request: {}", request);
    switch (request.type) {
        case RequestType::Database:
            return handleDatabaseRequest(std::move(request));
        default:
            return Response{.code = -1, .message = "Request type is not supported"};
    }
}

Response handleDatabaseRequest(Request&& request) {
    switch (request.subType) {
        case Open: {
            auto const name = pub::str(request.content);
            if (auto home = pub::homeDirectory()) {
                auto const path = std::format("{}/.beesoft_test", home.value());

                if (auto&& [code, message] = pub::createDirectory(path); code)
                    return Response{.id = request.id, .code = code, .message = message};

                Database::self().sqlite(std::format("{}/{}", path, name));
                auto const stat = Database::self().open();
                if (stat)
                    return Response{.id = request.id, .code = 0, .message = "Database opened"};
                return Response{.id = request.id, .code = stat.error().code, .message = stat.error().message};
            }
            return Response{.id = request.id, .code = -1, .message = "Failed to get home directory"};
        }
        case Create: {
            auto const name = pub::str(request.content);
            if (auto home = pub::homeDirectory()) {
                auto const path = std::format("{}/.beesoft_test", home.value());

                if (auto&& [code, message] = pub::createDirectory(path); code)
                    return Response{.id = request.id, .code = code, .message = message};

                Database::self().sqlite(std::format("{}/{}", path, name));
                auto const stat = Database::self().create({});
                if (stat)
                    return Response{.id = request.id, .code = 0, .message = "Database created"};
                return Response{.id = request.id, .code = stat.error().code, .message = stat.error().message};
            }
            return Response{.id = request.id, .code = -1, .message = "Failed to get home directory"};
        }
        default:
            return Response{.id = request.id, .code = -1, .message = "Request sub-type is not supported"};
    }
}
