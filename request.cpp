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
// Date: 28.11.2025
// E-mail: piotr@beesoft.pl.

/*------- include files:
-------------------------------------------------------------------*/
#include "request.h"
#include "common/socket/connector.h"

namespace bee {

    Result<Response,std::errc> Request::write(Connector const& conn) const noexcept {
        std::println("Request::write");
        std::println(" - request: {}", *this);

        if (auto json = toJSON()) {
            // Wysłanie żądania do gniazda.
            if (auto const stat = conn.write(std::move(json.value())); not stat)
                return Failure(stat.error());

            // Odczyt danych odpowiedzi z gniazda.
            auto const data = conn.read();
            if (not data)
                return Failure(data.error());

            // Z odczytanych danych tworzymy obiekt Odpowiedzi.
            if (auto const response = Response::fromJSON(data.value())) {
                std::println(" - response: {}", response.value());
                return response.value();
            }
        }
        return Failure(std::errc::bad_message);
    }

    Result<Request,std::errc> Request::read(Connector const& conn) noexcept {
        std::println("Request::read");

        auto const data = conn.read();
        if (not data)
            return Failure(data.error());

        auto request = fromJSON(data.value());
        if (not request)
            return Failure(std::errc::bad_message);

        std::println(" - request: {}", request.value());
        return request.value();
    }
}