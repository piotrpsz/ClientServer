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
// Date: 25.11.2025
// E-mail: piotr@beesoft.pl.
#pragma once

/*------- include files:
-------------------------------------------------------------------*/
#include "../../shared4cx/types.h"
#include <sys/socket.h>
#include <system_error>
#include <ranges>

namespace bee {
    static constexpr int INVALID_SOCKET = -1;

    /*------- Socket class:
     -------------------------------------------------------------------*/
    class Socket {
        int fd_ { INVALID_SOCKET };
    public:
        Socket();
        explicit Socket(int const fd) : fd_(fd) {}
        virtual ~Socket() { destroy(); }

        Socket(Socket const&) = delete;
        Socket& operator=(Socket const&) = delete;
        Socket(Socket&&) = default;
        Socket& operator=(Socket&&) = default;

        bool destroy() noexcept;
        [[nodiscard]] int fd() const noexcept { return fd_; }
        [[nodiscard]] Option<std::errc> connect(String const& address, int port) const noexcept;
        [[nodiscard]] Option<std::errc> bind(int port) const noexcept;
        [[nodiscard]] Option<std::errc> listen(int backlog = SOMAXCONN) const noexcept;
        [[nodiscard]] Result<int,Errc> accept() const noexcept;
        [[nodiscard]] String hostAddress() const noexcept;
        [[nodiscard]] String peerAddress() const noexcept;

        Result<size_t, Errc> writeBytes(void const* buffer, size_t size) const noexcept;
        [[nodiscard]] Result<size_t,Errc> writePackage(Span<u8> bytes) const noexcept;
        [[nodiscard]] Result<size_t,Errc> writeText(StringView const text) const noexcept {
            std::vector<unsigned char> vec{text.begin(), text.end()};
            return writePackage(vec);
        }

        Result<size_t, Errc> readBytes(void* buffer, size_t size) const noexcept;
        [[nodiscard]] Result<Vector<u8>,Errc> readPackage() const noexcept;
        [[nodiscard]] Result<String,Errc> readText() const noexcept {
            return readPackage().transform([](auto&& vec) {
                return std::string{vec.begin(), vec.end()};
            });
        }

    private:
        static bool set(int const fd, int const option, int const flag) noexcept {
            return setsockopt(fd, SOL_SOCKET, option, &flag, sizeof(int)) != -1;
        }
    };
}