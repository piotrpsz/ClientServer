//
// Created by Piotr Pszczolkowski on 26/11/2025.
//

#include "connector.h"

bool Server::init() noexcept{
    return true;
}

bool Client::init() noexcept {
    // Klient jako pierwszy wysyła swój klucz publiczny w postaci BER.
    auto const retv = crypto.RSAPublicKeyBER();

    return true;
}


