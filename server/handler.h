//
// Created by Piotr Pszczolkowski on 29/11/2025.
//

#pragma once
#include "../request.h"
#include "../response.h"

namespace bee {
    extern Response handleRequest(Request&& request);
}
