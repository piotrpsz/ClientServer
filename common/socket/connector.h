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
// Date: 26.11.2025
// E-mail: piotr@beesoft.pl.
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
        crypto::Crypto crypto{};
    public:
        Connector() = default;
        explicit Connector(int const fd) : Socket{fd} {}
        ~Connector() override = default;

        virtual bool init() noexcept = 0;
        [[nodiscard]] Result<size_t,Errc> write(std::string&& text) const noexcept;
        [[nodiscard]] Result<String,Errc> read() const noexcept;
    };

    /*------- Server:
    -------------------------------------------------------------------*/
    class Server final : public Connector {
    public:
        Server() = default;
        explicit Server(int const fd) : Connector(fd) {}
        ~Server() override = default;

        [[nodiscard]] Option<Errc> run(int const port) const noexcept {
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
