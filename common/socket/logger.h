//
// Created by Piotr Pszczolkowski on 25/11/2025.
//

#pragma once
/*------- include files:
-------------------------------------------------------------------*/
#include <string_view>
#include <system_error>

void print_error(int errcode, std::string_view title = {}) noexcept;
void print_error(std::errc errc, std::string_view title = {}) noexcept;
