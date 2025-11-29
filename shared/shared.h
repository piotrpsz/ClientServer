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


inline std::optional<std::string> homeDirectory() noexcept {
    if (auto const pw = getpwuid(getuid()))
        return pw->pw_dir;
    return {};
}

inline bool createDirectory(std::string_view const path) noexcept {
    namespace fs = std::filesystem;

    if (fs::exists(path))
        return true;

    std::error_code ec{};
    if (fs::create_directories(path, ec))
        return true;

    std::println("{}", ec.message());;
    return {};
}
