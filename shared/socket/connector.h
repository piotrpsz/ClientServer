//
// Created by Piotr Pszczolkowski on 26/11/2025.
//

#pragma once
#include "socket.h"
#include "../crypto/crypto.h"

class Server final : public Socket {
    bee::crypto::Crypto crypto{};
public:
    Server() = default;
    explicit Server(int const fd) : Socket{fd} {}
    ~Server() override = default;

    [[nodiscard]] std::expected<Unit,std::errc> run(int const port) const noexcept {
        if (auto const retv = bind(port); !retv)
            return retv;
        return listen();
    }

    bool init() noexcept;
    [[nodiscard]] std::expected<size_t, std::errc> write(std::span<unsigned char> bytes) const noexcept;
    [[nodiscard]] std::expected<size_t, std::errc> write_text(std::string&& text) const noexcept;
    [[nodiscard]] std::expected<std::vector<unsigned char>,std::errc> read() const noexcept;
    [[nodiscard]] std::expected<std::string,std::errc> read_text() const noexcept;
};

class Client final : public Socket {
    bee::crypto::Crypto crypto{};
public:
    Client() = default;
    explicit Client(int const fd) : Socket{fd} {}
    ~Client() override = default;

    bool init() noexcept;
    [[nodiscard]] std::expected<size_t, std::errc> write(std::span<unsigned char> bytes) const noexcept;
    [[nodiscard]] std::expected<size_t, std::errc> write_text(std::string&& text) const noexcept;
    [[nodiscard]] std::expected<std::vector<unsigned char>,std::errc> read() const noexcept;
    [[nodiscard]] std::expected<std::string,std::errc> read_text() const noexcept;
};
