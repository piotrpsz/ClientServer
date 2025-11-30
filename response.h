//
// Created by Piotr Pszczolkowski on 28/11/2025.
//

#pragma once
#include <iostream>
#include <boost/concept/detail/has_constraints.hpp>
#include <glaze/glaze.hpp>

#include "shared/socket/connector.h"
#include "sqlite4cx/shared/types.h"

struct Response {
    size_t id{};
    int code{};
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

    static Option<Response> fromJSON(String const& json) noexcept {
        Response request{};
        if (auto const ec = glz::read_json(request, json)) {
            std::println(std::cerr, "Error: {}", format_error(ec.ec));
            return {};
        }
        return request;
    }

    [[nodiscard]] Option<std::errc> write(Connector const& conn) const noexcept {
        if (auto json = toJSON()) {
            if (auto const stat = conn.write(std::move(json.value())); not stat)
                return stat.error();
            return {};
        }

        return std::errc::bad_message;
    }
};

template<>
struct glz::meta<Response> {
    using T = Response;
    static constexpr auto value = object(
        &T::id,
        &T::code,
        &T::message,
        &T::data
    );
};

template<>
struct std::formatter<Response> : std::formatter<std::string> {
    auto format(Response const& ans, std::format_context& ctx) const {
        return formatter<std::string>::format(
            std::format("Response[ id: {}, code: {}, message: {}, content: {} ]",
                ans.id, ans.code, ans.message, ans.data),
            ctx);
    }
};
