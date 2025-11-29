cmake_minimum_required(VERSION 4.0)
project(ClientServer)

set(CMAKE_CXX_STANDARD 26)

add_subdirectory(sqlite4cx)

find_package(Botan REQUIRED)

include(FetchContent)
FetchContent_Declare(
        glaze
        GIT_REPOSITORY https://github.com/stephenberry/glaze.git
        GIT_TAG main
        GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(glaze)


add_executable(Server
        server.cpp server.h
        shared/socket/socket.cpp shared/socket/socket.h
        shared/socket/logger.cpp shared/socket/logger.h
        shared/socket/connector.cpp shared/socket/connector.h
        shared/socket/all.hpp
        shared/crypto/crypto.cpp shared/crypto/crypto.h
        request.cpp request.h
        Response.cpp Response.h
)
target_include_directories(Server PUBLIC
        Botan::Botan
)
target_link_libraries(Server PUBLIC
        Botan::Botan
        sqlite4cx
        glaze::glaze
)

add_executable(Client
        client.cpp client.h
        shared/socket/socket.cpp shared/socket/socket.h
        shared/socket/logger.cpp shared/socket/logger.h
        shared/socket/connector.cpp shared/socket/connector.h
        shared/socket/all.hpp
        shared/crypto/crypto.cpp shared/crypto/crypto.h
        person.cpp
        person.h
        request.cpp request.h
        Response.cpp Response.h
        shared/shared.cpp
        shared/shared.h
)
target_include_directories(Client PUBLIC
        Botan::Botan
)
target_link_libraries(Client PUBLIC
        Botan::Botan
        sqlite4cx
        glaze::glaze
)
