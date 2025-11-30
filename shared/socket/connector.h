//
// Created by Piotr Pszczolkowski on 26/11/2025.
//

#pragma once
#include "socket.h"
#include "../crypto/crypto.h"

class Connector : public Socket {
protected:
    bee::crypto::Crypto crypto{};
public:
    Connector() = default;
    explicit Connector(int const fd) : Socket{fd} {}
    ~Connector() override = default;

    virtual bool init() noexcept = 0;
    [[nodiscard]] std::expected<size_t, std::errc> write(std::string&& text) const noexcept;
    [[nodiscard]] std::expected<std::string,std::errc> read() const noexcept;
};

class Server final : public Connector {
public:
    Server() = default;
    explicit Server(int const fd) : Connector(fd) {}
    ~Server() override = default;

    [[nodiscard]] std::expected<Unit,std::errc> run(int const port) const noexcept {
        if (auto const retv = bind(port); !retv)
            return retv;
        return listen();
    }

    bool init() noexcept override;
};

class Client final : public Connector {
public:
    Client() = default;
    explicit Client(int const fd) : Connector(fd) {}
    ~Client() override = default;

    bool init() noexcept override;
};


// class Connector : public Socket {
// protected:
//     bee::crypto::Crypto crypto{};
// public:
//     Connector() = default;
//     explicit Connector(int const fd) : Socket{fd} {}
//     ~Connector() override = default;
//
//     virtual bool init() noexcept = 0;
//
//     [[nodiscard]] std::expected<size_t, std::errc> write(std::string&& text) const noexcept;
//     [[nodiscard]] std::expected<std::string,std::errc> read() const noexcept;
// };
//
//
//
// class Server final : public Connector {
// public:
//     Server() = default;
//     explicit Server(int const fd) : Connector(fd) {}
//     ~Server() override = default;
//
//     [[nodiscard]] std::expected<Unit,std::errc> run(int const port) const noexcept {
//         if (auto const retv = bind(port); !retv)
//             return retv;
//         return listen();
//     }
//
//     bool init() noexcept override;
// };
//
// class Client final : public Connector {
//     bee::crypto::Crypto crypto{};
// public:
//     Client() = default;
//     explicit Client(int const fd) : Connector(fd) {}
//     ~Client() override = default;
//
//     bool init() noexcept override;
// };
