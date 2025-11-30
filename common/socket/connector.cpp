//
// Created by Piotr Pszczółkowski on 26/11/2025.
//

/*------- include files:
-------------------------------------------------------------------*/
#include "connector.h"
#include "logger.h"
#include <ranges>

using namespace bee;
using namespace bee::crypto;
namespace rg = std::ranges;
namespace rv = rg::views;


/********************************************************************
 *                                                                  *
 *                       C O N N E C T O R                          *
 *                                                                  *
 ********************************************************************/

std::expected<size_t, std::errc> Connector::write(std::string&& text) const noexcept {
    if (not text.empty()) {
        auto data = text
            | rv::transform([](auto c) { return static_cast<u8>(c); })
            | rg::to<Vector<u8>>();
        if (auto encrypted = crypto.encrypt(data))
            return writePackage(encrypted.value());
    }
    return std::unexpected(std::errc::bad_message);
}

std::expected<std::string,std::errc> Connector::read() const noexcept {
    auto const data = readPackage();
    if (not data)
        return std::unexpected(data.error());

    if (auto message = data.value(); auto const plain = crypto.decrypt(message)) {
        auto retv = plain.value()
            | rv::transform([](auto const c) { return static_cast<char>(c); })
            | rg::to<String>();
        return retv;
    }
    return std::unexpected(std::errc::bad_message);
}

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

    // 3. Odczyt zaszyfrowanego klucza AES klienta.
    auto const res = readPackage();
    if (!res) {
        print_error(res.error());
        return {};
    }
    auto encrypted_key = res.value();
    auto decrypted_key = crypto.decryptRSA(encrypted_key);
    crypto.setAESKey(std::move(decrypted_key));

    return true;
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

    // 3. Generujemy klucz AES i wysyłamy go do serwera.
    if (auto const aesKey = crypto.generateAESKey()) {
        auto vec = aesKey.value();
        auto encrypted_key = crypto.encryptRSA(vec);
        if (auto const res = writePackage(encrypted_key); !res) {
            print_error(res.error());
            return {};
        }
    }
    return true;
}
