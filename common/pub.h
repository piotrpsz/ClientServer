//
// Created by Piotr Pszczolkowski on 29/11/2025.
//

#pragma once
#include <optional>
#include <string>
#include <pwd.h>
#include <unistd.h>
#include <filesystem>
#include <format>
#include <iostream>
#include <system_error>
#include <span>
#include <expected>
#include "../sqlite4cx/shared/types.h"

namespace pub {
    inline String str(std::span<unsigned char> const data) {
        return std::string{data.begin(), data.end()};
    }

    inline Option<String> homeDirectory() noexcept {
        if (auto const pw = getpwuid(getuid()))
            return pw->pw_dir;
        return {};
    }

    inline Pair<int,String> createDirectory(std::string_view const path) noexcept {
        namespace fs = std::filesystem;

        std::error_code ec{};
        if (fs::exists(path, ec))
            return {0, {}};
        if (ec != std::errc{})
            return {ec.value(), ec.message()};

        if (fs::create_directories(path, ec))
            return {0, {}};
        return {ec.value(), ec.message()};
    }
}
