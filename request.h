//
// Created by Piotr Pszczolkowski on 28/11/2025.
//

#pragma once
#include <format>
#include <iostream>
#include <glaze/glaze.hpp>
#include <glaze/api/std/deque.hpp>

#include "response.h"
#include "sqlite4cx/shared/types.h"

class Connector;

enum RequestType {
    Unknown,
    Database,
    Table,
    Query,
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
        case Query: return "Query";
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


struct Request {
    size_t id{};      // numer identyfikacyjny żądania.
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

    [[nodiscard]] Result<Response,std::errc> write(Connector const& conn) const noexcept;
    static Result<Request,std::errc> read(Connector const& conn) noexcept;
};

template<>
struct glz::meta<Request> {
    using T = Request;
    static constexpr auto value = object(
        &T::id,
        &T::type,
        &T::subType,
        &T::value,
        &T::content
    );
};

template<>
struct std::formatter<Request> : std::formatter<std::string> {
    auto format(Request const& req, std::format_context& ctx) const {
        return formatter<std::string>::format(
            std::format("Request[ id: {}, type: {} | {}, value: {}, content: {} ]",
                req.id, str(req.type), str(req.subType), req.value, req.content),
            ctx);
    }
};