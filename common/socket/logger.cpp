//
// Created by Piotr Pszczolkowski on 25/11/2025.
//

#include "logger.h"
#include <print>
#include <iostream>

void print_error(int errcode, std::string_view const title) noexcept {
    print_error(static_cast<std::errc>(errcode), title);
}

void print_error(std::errc const errc, std::string_view const title) noexcept {
    // Przerwanie operacji blokującej.
    if (errc == std::errc::interrupted)
        return;

    // Druga strona połączenia zamknęła je.
    if (errc == std::errc::broken_pipe) {
        std::cout << "-- Closed peer connection.\n" << std::flush;
        return;
    }

    if (errc == std::errc::connection_aborted)
        return;

    // Rzeczywiście jakiś błąd.
    auto message = std::string{title};
    if (not message.empty())
        message += ": ";

    message += std::make_error_code(errc).message();
    std::println(std::cerr, "** {}\n", message);
}
