//
// Created by Piotr Pszczolkowski on 25/11/2025.
//

#pragma once
#include <expected>
#include <string>
#include <span>
#include <vector>
#include <sys/socket.h>
#include <system_error>


static constexpr int INVALID_SOCKET = -1;
struct Unit{};
static constexpr auto Success = Unit{};

class Socket final {
    int fd_ { INVALID_SOCKET };
public:
    Socket();
    explicit Socket(int const fd) : fd_(fd) {}
    ~Socket() { destroy(); }

    Socket(Socket const&) = delete;
    Socket& operator=(Socket const&) = delete;
    Socket(Socket&&) = default;
    Socket& operator=(Socket&&) = default;

    bool destroy() noexcept;
    [[nodiscard]] int fd() const noexcept { return fd_; }
    [[nodiscard]] std::expected<Unit,std::errc> connect(std::string const& address, int port) const noexcept;
    [[nodiscard]] std::expected<Unit,std::errc> bind(int port) const noexcept;
    [[nodiscard]] std::expected<Unit,std::errc> listen(int backlog = SOMAXCONN) const noexcept;
    [[nodiscard]] std::expected<int,std::errc> accept() const noexcept;
    [[nodiscard]] std::string hostAddress() const noexcept;
    [[nodiscard]] std::string peerAddress() const noexcept;

    std::expected<size_t, std::errc> writeBytes(void const* buffer, size_t size) const noexcept;
    [[nodiscard]] std::expected<size_t, std::errc> writePackage(std::span<char> bytes) const noexcept;

    std::expected<size_t, std::errc> readBytes(void* buffer, size_t size) const noexcept;
    [[nodiscard]] std::expected<std::vector<char>,std::errc> readPackage() const noexcept;

private:
    static bool set(int const fd, int const option, int const flag) noexcept {
        return setsockopt(fd, SOL_SOCKET, option, &flag, sizeof(int)) != -1;
    }
};
