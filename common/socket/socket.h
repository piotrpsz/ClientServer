//
// Created by Piotr Pszczolkowski on 25/11/2025.
//

#pragma once
#include "../../shared4cx/types.h"
#include <expected>
#include <string>
#include <span>
#include <vector>
#include <sys/socket.h>
#include <system_error>
#include <ranges>


static constexpr int INVALID_SOCKET = -1;
struct Unit{};
static constexpr auto Success = Unit{};

namespace bee {
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
        [[nodiscard]] Option<std::errc> connect(std::string const& address, int port) const noexcept;
        [[nodiscard]] Option<std::errc> bind(int port) const noexcept;
        [[nodiscard]] Option<std::errc> listen(int backlog = SOMAXCONN) const noexcept;
        [[nodiscard]] Result<int,std::errc> accept() const noexcept;
        [[nodiscard]] String hostAddress() const noexcept;
        [[nodiscard]] String peerAddress() const noexcept;

        Result<size_t, std::errc> writeBytes(void const* buffer, size_t size) const noexcept;
        [[nodiscard]] Result<size_t, std::errc> writePackage(std::span<unsigned char> bytes) const noexcept;
        [[nodiscard]] Result<size_t,std::errc> writeText(std::string_view const text) const noexcept {
            std::vector<unsigned char> vec{text.begin(), text.end()};
            return writePackage(vec);
        }

        Result<size_t, std::errc> readBytes(void* buffer, size_t size) const noexcept;
        [[nodiscard]] Result<std::vector<unsigned char>,std::errc> readPackage() const noexcept;
        [[nodiscard]] Result<std::string,std::errc> readText() const noexcept {
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