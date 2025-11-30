//
// Created by Piotr Pszczolkowski on 21/11/2025.
//

#include "crypto.h"

using namespace std::string_literals;

namespace bee::crypto {
    Botan::System_RNG rng;

    SecVector<u8> Crypto::RandomBytes(size_t const nbytes) noexcept {
        return rng.random_vec<SecVector<u8>>(nbytes);
    }

    SecVector<u8> Crypto::join(Span<const u8> const signature, Span<const u8> const message) noexcept {
        SecVector<u8> buffer{};
        buffer.reserve(signature.size() + message.size());
        std::copy_n(signature.data(), signature.size(), std::back_inserter(buffer));
        std::copy_n(message.data(), message.size(), std::back_inserter(buffer));
        return buffer;
    }

    Option<Pair<Span<u8>, Span<u8>>> Crypto::split( Span<u8> const data ) noexcept {
        if (data.size() >= SIGNATURE_SIZE)
            return Pair{data.first(SIGNATURE_SIZE), data.subspan(SIGNATURE_SIZE)};
        return {};
    }

}