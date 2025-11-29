//
// Created by Piotr Pszczolkowski on 28/11/2025.
//

#pragma once
#include <iostream>
#include <glaze/glaze.hpp>
#include "sqlite4cx/shared/types.h"

struct Response {
    size_t id{};
    int code{};
    Vector<u8> data{};

    [[nodiscard]] String toJSON() const noexcept {
        String buffer{};
        if (auto const ec = glz::write_json(*this, buffer)) {
            std::println(std::cerr, "Error: {}", format_error(ec.ec));
            return buffer;
        }
        return buffer;
    }

    static std::optional<Response> fromJSON(String const& json) noexcept {
        Response request{};
        if (auto const ec = glz::read_json(request, json)) {
            std::println(std::cerr, "Error: {}", format_error(ec.ec));
            return {};
        }
        return request;
    }
};

template<>
struct glz::meta<Response> {
    using T = Response;
    static constexpr auto value = object(
        &T::id,
        &T::code,
        &T::data
    );
};

template<>
struct std::formatter<Response> : std::formatter<std::string> {
    auto format(Response const& ans, std::format_context& ctx) const {
        return formatter<std::string>::format(std::format("Request[ id: {}, code: {}, content: {} ]", ans.id, ans.code, ans.data), ctx);
    }
};
