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

/*------- include files:
-------------------------------------------------------------------*/
#include "connector.h"
#include "logger.h"
#include <ranges>

namespace rg = std::ranges;
namespace rv = rg::views;

namespace bee {

    /********************************************************************
     *                                                                  *
     *                       C O N N E C T O R                          *
     *                                                                  *
     ********************************************************************/

    Result<size_t,Errc> Connector::write(String&& text) const noexcept {
        if (not text.empty()) {
            auto data = text
                | rv::transform([](auto c) { return static_cast<u8>(c); })
                | rg::to<Vector<u8>>();
            if (auto encrypted = crypto.encrypt(data))
                return writePackage(encrypted.value());
        }
        return Failure(std::errc::bad_message);
    }

    Result<String,Errc> Connector::read() const noexcept {
        auto const data = readPackage();
        if (not data)
            return Failure(data.error());

        if (auto message = data.value(); auto const plain = crypto.decrypt(message)) {
            auto retv = plain.value()
                | rv::transform([](auto const c) { return static_cast<char>(c); })
                | rg::to<String>();
            return retv;
        }
        return Failure(std::errc::bad_message);
    }

    /********************************************************************
     *                                                                  *
     *                          S E R V E R                             *
     *                                                                  *
     ********************************************************************/

    bool Server::init() noexcept{
        // 1. Serwer czeka na klucz publiczny RSA klienta.
        auto const client_public_key_ber = readText();
        if (not client_public_key_ber) {
            print_error(client_public_key_ber.error());
            return {};
        }
        crypto.setBuddyRSAPublicKey(client_public_key_ber.value());

        // 2. Serwer wysyła swój klucz publiczny.
        if (auto const retv = writeText(crypto.RSAPublicKeyBER()); not retv) {
            print_error(retv.error());
            return {};
        }

        // 3. Odczyt zaszyfrowanego klucza AES klienta.
        auto const res = readPackage();
        if (not res) {
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
        if (auto const retv = writeText(crypto.RSAPublicKeyBER()); not retv) {
            print_error(retv.error());
            return {};
        }

        // 2. Klient czeka na klucz publiczny serwera.
        auto const publicKeyBER = readText();
        if (not publicKeyBER) {
            print_error(publicKeyBER.error());
            return {};
        }
        crypto.setBuddyRSAPublicKey(publicKeyBER.value());

        // 3. Generujemy klucz AES i wysyłamy go do serwera.
        if (auto const aesKey = crypto.generateAESKey()) {
            auto vec = aesKey.value();
            auto encrypted_key = crypto.encryptRSA(vec);
            if (auto const res = writePackage(encrypted_key); not res) {
                print_error(res.error());
                return {};
            }
        }
        return true;
    }
}