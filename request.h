//
// Created by Piotr Pszczolkowski on 28/11/2025.
//

#pragma once
#include <format>
#include <iostream>
#include <glaze/glaze.hpp>
#include <glaze/api/std/deque.hpp>
#include "sqlite4cx/shared/types.h"

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
        case RequestType::Database: return "Database";
        case RequestType::Table: return "Table";
        case RequestType::Query: return "Query";
        default: return "Unknown";
    }
}
inline std::string str(RequestSubType const& type) noexcept {
    switch (type) {
        case RequestSubType::Open: return "Open";
        case RequestSubType::Create: return "Create";
        case RequestSubType::Drop: return "Drop";
        case RequestSubType::Close: return "Close";
        case RequestSubType::Insert: return "Insert";
        case RequestSubType::Select: return "Select";
        case RequestSubType::Update: return "Update";
        case RequestSubType::Delete: return "Delete";
        default: return "Unknown";
    }
}


struct Request {
    size_t id;      // numer identyfikacyjny żądania.
    RequestType type;
    RequestSubType subType;
    Vector<u8> content;

    [[nodiscard]] String toJSON() const noexcept {
        String buffer{};
        if (auto const ec = glz::write_json(*this, buffer)) {
            std::println(std::cerr, "Error: {}", format_error(ec.ec));
            return buffer;
        }
        return buffer;
    }

    static std::optional<Request> fromJSON(String const& json) noexcept {
        Request request{};
        if (auto const ec = glz::read_json(request, json)) {
            std::println(std::cerr, "Error: {}", format_error(ec.ec));
            return {};
        }
        return request;
    }
};

template<>
struct glz::meta<Request> {
    using T = Request;
    static constexpr auto value = object(
        &T::id,
        &T::type,
        &T::subType,
        &T::content
    );
};

template<>
struct std::formatter<Request> : std::formatter<std::string> {
    auto format(Request const& req, std::format_context& ctx) const {
        return formatter<std::string>::format(
            std::format("Request[ id: {}, type: {} | {}, content: {} ]",
                req.id, str(req.type), str(req.subType), req.content),
            ctx);
    }
};