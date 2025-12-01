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
#include "shared4cx/types.h"
#include "response.h"
#include <format>
#include <iostream>
#include <glaze/glaze.hpp>
#include <glaze/api/std/deque.hpp>

namespace bee {
    class Connector;

    enum RequestType {
        Unknown,
        Database,
        Table,
        ExecQuery,
    };
    enum RequestSubType {
        None,
        Open,
        Create,
        Drop,
        Close,
        Insert,
        Select,
        Update,
        Delete,
    };

    inline std::string str(RequestType const& type) noexcept {
        switch (type) {
            case Database: return "Database";
            case Table: return "Table";
            case ExecQuery: return "ExecQuery";
            default: return "Unknown";
        }
    }
    inline std::string str(RequestSubType const& type) noexcept {
        switch (type) {
            case Open: return "Open";
            case Create: return "Create";
            case Drop: return "Drop";
            case Close: return "Close";
            case Insert: return "Insert";
            case Select: return "Select";
            case Update: return "Update";
            case Delete: return "Delete";
            default: return "Unknown";
        }
    }

    /*------- Request struct:
    -------------------------------------------------------------------*/
    struct Request final {
        size_t id{};
        RequestType type{};
        RequestSubType subType{};
        String value{};
        Vector<u8> content{};

        [[nodiscard]] Option<String> toJSON() const noexcept {
            String buffer{};
            if (auto const ec = glz::write_json(*this, buffer)) {
                std::println(std::cerr, "Error: {}", format_error(ec.ec));
                return {};
            }
            return buffer;
        }

        static Option<Request> fromJSON(String const& json) noexcept {
            Request request{};
            if (auto const ec = glz::read_json(request, json)) {
                std::println(std::cerr, "Error: {}", format_error(ec.ec));
                return {};
            }
            return request;
        }

        /// Wysłanie żądania poprzez wskazane gniazdo (używane zazwyczaj po stronie klienta).
        /// \param conn Obiekt gniazda, poprzez który należy wysłać dane.
        /// \return Albo odpowiedź na żądanie lub błąd errc.
        [[nodiscard]] Result<Response,std::errc> write(Connector const& conn) const noexcept;

        /// Odczyt żądania ze wskazanego gniazda (używane zazwyczaj po stronie serwera).
        /// \param conn Obiekt gniazda, z którego należy czytać dane.
        /// \return Albo obiekt żądania lub błąd errc.
        static Result<Request,std::errc> read(Connector const& conn) noexcept;
    };
}

    template<>
    struct glz::meta<bee::Request> {
        using T = bee::Request;
        static constexpr auto value = object(
            &T::id,
            &T::type,
            &T::subType,
            &T::value,
            &T::content
        );
    };

template<>
struct std::formatter<bee::Request> : std::formatter<std::string> {
    auto format(bee::Request const& req, std::format_context& ctx) const {
        return formatter<std::string>::format(
            std::format("Request[ id: {}, type: {} | {}, value: {}, content: {} ]",
                req.id, str(req.type), str(req.subType), req.value, req.content),
            ctx);
    }
};
