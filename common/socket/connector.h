//
// Created by Piotr Pszczółkowski on 26/11/2025.
//
#pragma once

/*------- include files:
-------------------------------------------------------------------*/
#include "socket.h"
#include "../crypto/crypto.h"

namespace bee {
    /*------- Connector:
    -------------------------------------------------------------------*/
    class Connector : public Socket {
    protected:
        bee::crypto::Crypto crypto{};
    public:
        Connector() = default;
        explicit Connector(int const fd) : Socket{fd} {}
        ~Connector() override = default;

        virtual bool init() noexcept = 0;
        [[nodiscard]] Result<size_t, std::errc> write(std::string&& text) const noexcept;
        [[nodiscard]] Result<std::string,std::errc> read() const noexcept;
    };

    /*------- Server:
    -------------------------------------------------------------------*/
    class Server final : public Connector {
    public:
        Server() = default;
        explicit Server(int const fd) : Connector(fd) {}
        ~Server() override = default;

        [[nodiscard]] Option<std::errc> run(int const port) const noexcept {
            if (auto const retv = bind(port))
                return retv;
            return listen();
        }

        bool init() noexcept override;
    };

    /*------- Client:
    -------------------------------------------------------------------*/
    class Client final : public Connector {
    public:
        Client() = default;
        explicit Client(int const fd) : Connector(fd) {}
        ~Client() override = default;

        bool init() noexcept override;
    };
}
