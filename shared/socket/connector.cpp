//
// Created by Piotr Pszczolkowski on 26/11/2025.
//

#include "connector.h"
#include "logger.h"
#include <ranges>
#include <algorithm>

using namespace bee::crypto;

/********************************************************************
 *                                                                  *
 *                          S E R V E R                             *
 *                                                                  *
 ********************************************************************/

bool Server::init() noexcept{
    // 1. Serwer czeka na klucz publiczny RSA klienta.
    auto const client_public_key_ber = readText();
    if (!client_public_key_ber) {
        print_error(client_public_key_ber.error());
        return {};
    }
    crypto.setBuddyRSAPublicKey(client_public_key_ber.value());

    // 2. Serwer wysyła swój klucz publiczny.
    if (auto retv = writeText(crypto.RSAPublicKeyBER()); !retv) {
        print_error(retv.error());
        return {};
    }

    return true;
}

std::expected<size_t, std::errc> Server::write(std::span<unsigned char> bytes) const noexcept {
    if (auto encrypted = crypto.encrypt(bytes))
        return writePackage(encrypted.value());
    return std::unexpected(std::errc::bad_message);
}

std::expected<std::vector<unsigned char>,std::errc> Server::read() const noexcept {
    auto const data = readPackage();
    if (not data)
        return std::unexpected(data.error());

    if (auto message = data.value(); auto const plain = crypto.decrypt(message))
        return As<Vector<u8>>(plain.value());

    return std::unexpected(std::errc::bad_message);
}


/********************************************************************
 *                                                                  *
 *                            C L I E N T                           *
 *                                                                  *
 ********************************************************************/

bool Client::init() noexcept {
    // 1. Klient jako pierwszy wysyła swój klucz publiczny w postaci BER.
    if (auto const retv = writeText(crypto.RSAPublicKeyBER()); !retv) {
        print_error(retv.error());
        return {};
    }

    // 2. Klient czeka na klucz publiczny serwera.
    auto const publicKeyBER = readText();
    if (!publicKeyBER) {
        print_error(publicKeyBER.error());
        return {};
    }
    crypto.setBuddyRSAPublicKey(publicKeyBER.value());

    return true;
}

std::expected<size_t, std::errc> Client::write(std::span<unsigned char> const bytes) const noexcept {
    if (auto encrypted = crypto.encrypt(bytes))
        return writePackage(encrypted.value());
    return std::unexpected(std::errc::bad_message);
}

[[nodiscard]] std::expected<std::vector<unsigned char>,std::errc> Client::read() const noexcept {
    auto const data = readPackage();
    if (not data)
        return std::unexpected(data.error());

    auto message = data.value();
    if (auto const plain = crypto.decrypt(message))
        return As<Vector<u8>>(plain.value());

    return std::unexpected(std::errc::bad_message);
}

