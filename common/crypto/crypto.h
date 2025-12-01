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
// Date: 21.11.2025
// E-mail: piotr@beesoft.pl.
#pragma once

/*------- include files:
-------------------------------------------------------------------*/
#include <expected>
#include <iostream>
#include <botan/system_rng.h>
#include <botan/secmem.h>   // secure_vector
#include <botan/hex.h>      // hex_encode
#include <span>
#include <vector>
#include <optional>
#include <print>
#include <string>
#include <string_view>
#include <botan/aead.h>
#include <botan/rsa.h>
#include <botan/pubkey.h>
#include <botan/base64.h>
#include <botan/x509_key.h>
#include <ranges>
#include <boost/exception/exception.hpp>

namespace bee::crypto {
    extern Botan::System_RNG rng;

    using u8 = uint8_t;
    using i8 = int8_t;
    using String = std::string;
    using StringView = std::string_view;
    using Errc = std::errc;
    template<typename T, typename K> using Pair = std::pair<T, K>;
    template<typename T> using Span = std::span<T>;
    template<typename T> using Vector = std::vector<T>;
    template<typename T> using SecVector = Botan::secure_vector<T>;
    template<typename T> using Option = std::optional<T>;
    template<typename T, typename E> using Result = std::expected<T, E>;
    template<typename T> using Failure = std::unexpected<T>;
    template<typename T> using UniquePtr = std::unique_ptr<T>;


    inline SecVector<u8> As(Span<const u8> const data) noexcept {
        SecVector<u8> buffer{};
        buffer.reserve(data.size());
        std::copy_n(data.begin(), data.size(), std::back_inserter(buffer));
        return buffer;
    }

    template<typename Out>
    Out As(auto const& data) {
        Out out;
        out.resize(data.size());
        memcpy(out.data(), data.data(), data.size());
        return out;
    }

    inline String Hex(Span<const u8> const data) {
        return Botan::hex_encode(data);
    }

    /*------- Crypto:
    ---------------------------------------------------------------*/
    class Crypto {
        static constexpr auto RSA_ALGO = "EME-OAEP(SHA-256,MGF1)";
        static constexpr auto SIGN_ALGO = "PKCS1v15(SHA-256)";
        static constexpr size_t SIGNATURE_SIZE = 256;
        static constexpr size_t AES_KEY_SIZE = 32;
        static constexpr size_t AES_NONCE_SIZE = 12;

        UniquePtr<Botan::Private_Key> rsa_private_key_{};
        UniquePtr<Botan::Public_Key> rsa_buddy_public_key_{};
        Option<SecVector<u8>> aes_key_{};

    public:
        Crypto() {
            rsa_private_key_ = std::make_unique<Botan::RSA_PrivateKey>(rng, 2048);
        }

        ~Crypto() = default;
        Crypto(Crypto const&) = delete;
        Crypto& operator=(Crypto const&) = delete;
        Crypto(Crypto&&) = default;
        Crypto& operator=(Crypto&&) = default;

        /// Zaszyfrowanie komunikatu.
        [[nodiscard]] Option<SecVector<u8>> encrypt(Span<const u8> const plain_message) const noexcept {
            if (auto const encrypted_message = encryptAES(plain_message)) {
                auto message = encrypted_message.value();
                return sign(message);
            }
            return {};
        }

        /// Odszyfrowanie komunikatu.
        [[nodiscard]] Option<SecVector<u8>> decrypt(Span<u8> const signed_message) const noexcept {
            // std::println("Decrypting message...");
            try {
                if (auto const message = verify(signed_message))
                    return decryptAES(*message);
            }
            catch (Botan::Exception const&e) {
                std::println(std::cerr, "Error: {}", e.what());
            }
            return {};
        }

        /************************************************************
         *                                                          *
         *                         R S A                            *
         *                                                          *
         ************************************************************/

        /// Utworzenie klucza publicznego RSA partnera z BER.
        bool setBuddyRSAPublicKey(StringView const keyBER) {
            rsa_buddy_public_key_ = Botan::X509::load_key(Botan::base64_decode(keyBER));
            return rsa_buddy_public_key_ ? true : false;
        }

        /// Zwraca publiczny klucz RSA jako BER.
        /// \return Zwraca Result<String, Error>
        [[nodiscard]] String RSAPublicKeyBER() const {
            UniquePtr<Botan::Public_Key> const rsa_public_key = rsa_private_key_->public_key();
            return Botan::base64_encode(Botan::X509::BER_encode(*rsa_public_key));
        }


        /// Szyfrowanie.
        /// Szyfrujemy kluczem publicznym partnera,
        /// on odszyfruje to swoim kluczem prywatnym.
        [[nodiscard]] Vector<u8> encryptRSA(Span<u8> const data) const {
            return Botan::PK_Encryptor_EME(*rsa_buddy_public_key_, rng, RSA_ALGO).encrypt(data, rng);
        }

        /// Odszyfrowanie.
        /// Partner zaszyfrował naszym kluczem publicznym,
        /// my deszyfrujemy swoim kluczem prywatnym.
        [[nodiscard]] SecVector<u8> decryptRSA(Span<u8> const data) const {
            return Botan::PK_Decryptor_EME(*rsa_private_key_, rng, RSA_ALGO).decrypt(data);
        }


        /************************************************************
         *                                                          *
         *                         A E S                            *
         *                                                          *
         ************************************************************/

        /// Utworzenie losowo wygenerowanego klucza dla szyfrowania EAS.
        /// \return Wygenerowany klucz.
        Option<SecVector<u8>> generateAESKey() {
            aes_key_ = rng.random_vec<SecVector<u8>>(AES_KEY_SIZE);
            return aes_key_;
        }

        /// Klucz AES jest zadany z zewnątrz.
        void setAESKey(SecVector<u8>&& key) {
            aes_key_ = std::move(key);
        }

        /// Szyfrowanie-AES wskazanych bajtów.
        [[nodiscard]] Option<SecVector<u8>> encryptAES(Span<const u8> const data) const {
            if (!aes_key_)
                // Jeśli nie ma klucza szyfrowania-AES, to nic nie robimy i zwracamy to, co przyszło bez zmian.
                return As<SecVector<u8>>(data);

            auto const encryptor_{ Botan::AEAD_Mode::create_or_throw("AES-256/GCM", Botan::Cipher_Dir::Encryption) };

            // Nonce jest generowane losowo za każdym razem od nowa.
            // Później jest dołączany do zaszyfrowanej wiadomości.
            // Zwracana wiadomość: nonce + zaszyfrowane dane.
            auto const nonce = rng.random_vec<SecVector<u8>>(AES_NONCE_SIZE);

            SecVector<u8> buffer{};
            buffer.reserve(AES_NONCE_SIZE + data.size() + encryptor_->output_length(data.size()));
            std::copy_n(nonce.data(), AES_NONCE_SIZE, std::back_inserter(buffer));
            std::copy_n(data.data(), data.size(), std::back_inserter(buffer));

            encryptor_->set_key(*aes_key_);
            encryptor_->start(nonce);
            encryptor_->finish(buffer, AES_NONCE_SIZE); // nonce omijamy, nie szyfrujemy, to losowe bajty.

            buffer.shrink_to_fit();
            return buffer;
        }

        /// Odszyfrowanie-AES wskazanych bajtów.
        [[nodiscard]] Option<SecVector<u8>> decryptAES(Span<const u8> const data) const {
            if (!aes_key_)
                // Jeśli nie ma klucza szyfrowania-AES, to nic nie robimy i zwracamy to, co przyszło bez zmian.
                return As(data);

            if (data.size() < AES_NONCE_SIZE)
                // To jest błąd, nic nie zwracamy.
                return {};

            auto const decryptor_{ Botan::AEAD_Mode::create_or_throw("AES-256/GCM", Botan::Cipher_Dir::Decryption)};
            auto const nonce = data.first(AES_NONCE_SIZE);
            auto const cipher = data.subspan(AES_NONCE_SIZE);

            Vector<u8> buffer{};
            buffer.reserve(cipher.size() + decryptor_->output_length(cipher.size()));
            std::copy_n(cipher.begin(), cipher.size(), std::back_inserter(buffer));

            decryptor_->set_key(*aes_key_);
            decryptor_->start(nonce);
            decryptor_->finish(buffer);

            buffer.shrink_to_fit();
            return As(buffer);
        }

        /************************************************************
         *                                                          *
         *                   S Y G N A T U R A                      *
         *                                                          *
         ************************************************************/

        [[nodiscard]] SecVector<u8> sign(Span<u8> const message) const {
            return join(createSignature(message), message);
        }

        [[nodiscard]] Option<SecVector<u8>> verify(Span<u8> const data) const {
            auto const retv = split(data);
            auto&& [signature, message] = retv.value();
            if (verifySignature(signature, message))
                return As(message);

            return {};
        }

        /// Sygnatura.
        /// Sygnaturę tworzymy swoim kluczem prywatnym.
        /// Partner sprawdzi to naszym kluczem publicznym.
        [[nodiscard]] Vector<u8> createSignature(Span<u8> const message) const {
            auto const signer = std::make_unique<Botan::PK_Signer>(*rsa_private_key_, rng, SIGN_ALGO);
            signer->update(message);
            return signer->signature(rng);
        }

        /// Weryfikacja sygnatury.
        /// Partner utworzył sygnaturę swoim kluczem prywatnym.
        /// My weryfikujemy jego kluczem publicznym.
        [[nodiscard]] bool verifySignature(Span<const u8> const signature, Span<const u8> const message) const {
            auto const verifier = std::make_unique<Botan::PK_Verifier>(*rsa_buddy_public_key_, SIGN_ALGO);
            verifier->update(message);
            return verifier->check_signature(signature);
        }

        static SecVector<u8> RandomBytes(size_t nbytes) noexcept;

    private:
        /// Połączenie bajtów komunikatu i bajtów sygnatury w jeden NOWY wektor bajtów.
        /// \remark W zwracanym ciągu bajtów najpierw jest sygnatura, a za nią komunikat.
        /// \param signature Bajty sygnatury,
        /// \param message Bajty komunikatu.
        /// \return Wektor bajtów, powstały z połączenia bajtów sygnatury i komunikatu.
        static SecVector<u8> join(Span<const u8> signature, Span<const u8> message) noexcept;

        /// Rozdzielenie bajtów danych na sygnaturę i komunikat.
        /// \param data Bajty danych.
        /// \return Para <sygnatura, komunikat>.
        static Option<Pair<Span<u8>, Span<u8>>> split(Span<u8> data) noexcept;
    };

}
