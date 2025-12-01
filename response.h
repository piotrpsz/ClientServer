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
#pragma once

/*------- include files:
-------------------------------------------------------------------*/
#include <iostream>
#include <glaze/glaze.hpp>
#include "common/socket/connector.h"
#include "shared4cx/types.h"

namespace bee {
    /*------- Response struct:
    -------------------------------------------------------------------*/
    struct Response final {
        size_t id{};
        int code{};
        int value{};
        String message{};
        Vector<u8> data{};

        [[nodiscard]] Option<String> toJSON() const noexcept {
            String buffer{};
            if (auto const ec = glz::write_json(*this, buffer)) {
                std::println(std::cerr, "Error: {}", format_error(ec.ec));
                return {};
            }
            return buffer;
        }

        /// Konstruktor obiektu z tekstu JSON.
        static Option<Response> fromJSON(String const& json) noexcept {
            Response request{};
            if (auto const ec = glz::read_json(request, json)) {
                std::println(std::cerr, "Error: {}", format_error(ec.ec));
                return {};
            }
            return request;
        }

        /// Przesłanie zserializowanej postaci obiektu (JSON) do wskazanego gniazda.
        /// \param conn Obiekt gniazda.
        /// \return Zwraca błąd lub nic.
        [[nodiscard]] Option<std::errc> write(Connector const& conn) const noexcept {
            if (auto json = toJSON()) {
                if (auto const stat = conn.write(std::move(json.value())); not stat)
                    return stat.error();
                return {};
            }
            return std::errc::bad_message;
        }
    };
}
template<>
struct glz::meta<bee::Response> {
    using T = bee::Response;
    static constexpr auto value = object(
        &T::id,
        &T::code,
        &T::value,
        &T::message,
        &T::data
    );
};

template<>
struct std::formatter<bee::Response> : std::formatter<std::string> {
    auto format(bee::Response const& ans, std::format_context& ctx) const {
        return formatter<std::string>::format(
            std::format("Response[ id: {}, code: {}, value: {}, message: {}, content: {} ]",
                ans.id, ans.code, ans.value, ans.message, ans.data),
            ctx);
    }
};
