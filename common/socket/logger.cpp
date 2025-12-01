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
// Date: 25.11.2025
// E-mail: piotr@beesoft.pl.

/*------- include files:
-------------------------------------------------------------------*/
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
