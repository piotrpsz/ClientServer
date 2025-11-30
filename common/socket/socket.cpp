//
// Created by Piotr Pszczółkowski on 25/11/2025.
//

/*------- include files:
-------------------------------------------------------------------*/
#include "socket.h"
#include "logger.h"
#include <cerrno>
#include <arpa/inet.h>
#include <unistd.h>
#include <netdb.h>
#include <format>
#include <print>

using namespace bee;

Socket::Socket() {
    if (auto const fd = socket(AF_INET, SOCK_STREAM, 0); fd != INVALID_SOCKET) {
        fd_ = fd;
        // auto const ok = set(fd, SO_REUSEADDR, 1)
        //     && set(fd, SO_KEEPALIVE, 1)
        //     && set(fd, SO_NOSIGPIPE, 1)
        //     && set(fd, SO_REUSEPORT, 1);
        // ok ? fd_ = fd : ::close(fd);
    }
}

bool Socket::destroy() noexcept {
    if (fd_ != INVALID_SOCKET) {
        if (close(fd_) == 0) {
            fd_ = INVALID_SOCKET;
            return true;
        }
        print_error(errno);
    }
    return {};
}

Option<std::errc> Socket::connect(std::string const& address, int const port) const noexcept {
    if (auto const host = gethostbyname(address.c_str())) {
        sockaddr_in dest{
            .sin_family = AF_INET,
            .sin_port = htons(port),
            .sin_addr {.s_addr = *reinterpret_cast<unsigned *>(host->h_addr_list[0])}
        };
        if (::connect(fd_, reinterpret_cast<sockaddr *>(&dest), sizeof(dest)) == 0)
            return {};
    }
    return std::errc{errno};
}

Option<std::errc> Socket::bind(int port) const noexcept {
    sockaddr_in const destination {
        .sin_family = AF_INET,
        .sin_port = htons(port),
        .sin_addr = {.s_addr = htonl(INADDR_ANY)}
    };
    auto const addr = reinterpret_cast<struct sockaddr const*>(&destination);

    if (::bind(fd_, addr, sizeof(destination)) == 0)
        return {};

    return std::errc{errno};
}

Option<std::errc> Socket::listen(int const backlog) const noexcept {
    if (::listen(fd_, 5) == 0)
        return {};

    return std::errc{errno};
}

Result<int,std::errc> Socket::accept() const noexcept {
    auto const fd = ::accept(fd_, nullptr, nullptr);
    if (fd != INVALID_SOCKET)
        return fd;
    std::println("{}", fd);
    print_error(std::errc{errno});
    return Failure(std::errc{errno});
}

std::string Socket::hostAddress() const noexcept {
    sockaddr_in addr{};
    socklen_t n = sizeof(addr);
    if (getsockname(fd_, reinterpret_cast<sockaddr *>(&addr), &n) == 0)
        return std::format("{}:{}", inet_ntoa(addr.sin_addr), ntohs(addr.sin_port));

    print_error(errno);
    return {};
}

std::string Socket::peerAddress() const noexcept {
    sockaddr_in addr{};
    socklen_t n = sizeof(addr);
    if (getpeername(fd_, reinterpret_cast<sockaddr *>(&addr), &n) == 0)
        return std::format("{}:{}", inet_ntoa(addr.sin_addr), ntohs(addr.sin_port));

    print_error(errno);
    return {};
}

/********************************************************************
 *                                                                  *
 *                  W R I T E   T O   S O C K E T                   *
 *                                                                  *
 ********************************************************************/

Result<size_t, std::errc> Socket::writeBytes(void const* const buffer, size_t const size) const noexcept {
    auto nleft = size;
    auto ptr = static_cast<char const*>(buffer);

    while (nleft > 0) {
        auto nwritten = ::write(fd_, ptr, nleft);
        if (nwritten <= 0) {
            if (errno == EINTR) {
                nwritten = 0;
            } else {
                return Failure(std::errc{errno});
            }
        }
        nleft -= nwritten;
        ptr += nwritten;
    }
    return size - nleft;
}

Result<size_t, std::errc> Socket::writePackage(std::span<unsigned char> const bytes) const noexcept {
    size_t const size = bytes.size();
    if (auto const retv = writeBytes(&size, sizeof(size)); !retv)
        return retv;

    return writeBytes(bytes.data(), size);
}

/********************************************************************
 *                                                                  *
 *                 R E A D   F R O M   S O C K E T                  *
 *                                                                  *
 ********************************************************************/

Result<size_t, std::errc> Socket::readBytes(void* const buffer, size_t const size) const noexcept {
    auto nleft = size;
    auto ptr = static_cast<char*>(buffer);

    while (nleft > 0) {
        auto nread = read(fd_, ptr, nleft);
        if (nread < 0) {
            if (errno == EINTR) {
                nread = 0;
            } else {
                return Failure(std::errc{errno});
            }
        }
        else if (nread == 0)
            break;
        nleft -= nread;
        ptr += nread;
    }

    return size - nleft;
}

Result<std::vector<unsigned char>,std::errc> Socket::readPackage() const noexcept {
    size_t nbytes{};
    auto retv = readBytes(&nbytes, sizeof(nbytes));
    if (not retv)
        return Failure(retv.error());
    if (retv.value() == 0)
        return Failure(std::errc::broken_pipe);

    std::vector<unsigned char> bytes(nbytes);
    retv = readBytes(bytes.data(), nbytes);
    if (not retv)
        return Failure(retv.error());
    if (retv.value() == 0)
        return Failure(std::errc::broken_pipe);

    return bytes;
}
