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
// Date: 29.11.2025
// E-mail: piotr@beesoft.pl.

/*------- include files:
-------------------------------------------------------------------*/
#include "handler.h"
#include "../shared4cx/shared.h"
#include "../sqlite4cx/sqlite4cx.hpp"
#include <ranges>
#include <algorithm>
#include <format>

namespace rg = std::ranges;
namespace rv = std::ranges::views;

namespace bee {
    static Response handleDatabaseRequest(Request&& request);
    static Response handleTableRequest(Request&& request);

    Response handleRequest(Request &&request) {
        switch (request.type) {
            case Database:
                return handleDatabaseRequest(std::move(request));
            case Table:
                return handleTableRequest(std::move(request));
            default:
                return Response{.code = -1, .message = "Request type is not supported"};
        }
    }

    Response handleDatabaseRequest(Request&& request) {
        switch (request.subType) {

            //------- OPEN ------------------------------------------
            case Open: {
                auto const name = request.value;
                if (auto home = homeDirectory()) {
                    auto const path = std::format("{}/.beesoft_test", home.value());
                    if (auto const err = createDirectory(path))
                        return Response{.id = request.id, .code = err->code, .message = err->message};

                    Database::self().sqlite(std::format("{}/{}", path, name));

                    if (auto const stat = Database::self().open())
                        return Response{.id = request.id, .code = stat->code, .message = stat->message};

                    return Response{.id = request.id, .code = 0, .message = "Database opened"};

                }
                return Response{.id = request.id, .code = -1, .message = "Failed to get home directory"};
            }

            //------- CREATE ----------------------------------------
            case Create: {
                auto const name = request.value;
                if (auto home = homeDirectory()) {
                    auto const path = std::format("{}/.beesoft_test", home.value());
                    if (auto const err = createDirectory(path))
                        return Response{.id = request.id, .code = err->code, .message = err->message};

                    Database::self().sqlite(std::format("{}/{}", path, name));
                    if (auto const stat = Database::self().create({}))
                        return Response{.id = request.id, .code = stat->code, .message = stat->message};

                    return Response{.id = request.id, .code = 0, .message = "Database created"};
                }
                return Response{.id = request.id, .code = -1, .message = "Failed to get home directory"};
            }
            default:
                return Response{.id = request.id, .code = -1, .message = "Request sub-type is not supported"};
        }
    }

    Response handleTableRequest(Request&& request) {
        switch (request.subType) {
            case Create: {
                if (auto const name = request.value; not name.empty()) {
                    auto const stat = Database::self().exec(name);
                }
                return Response{.id = request.id, .code = 0, .message = "Table created"};
            }
            default:
                return Response{.id = request.id, .code = -1, .message = "Request type is not supported"};
        }
    }

}