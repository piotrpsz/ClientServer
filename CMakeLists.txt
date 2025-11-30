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
        server/handler.cpp server/handler.h
        common/socket/socket.cpp common/socket/socket.h
        common/socket/logger.cpp common/socket/logger.h
        common/socket/connector.cpp common/socket/connector.h
        common/socket/all.hpp
        common/crypto/crypto.cpp common/crypto/crypto.h
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
        common/socket/socket.cpp common/socket/socket.h
        common/socket/logger.cpp common/socket/logger.h
        common/socket/connector.cpp common/socket/connector.h
        common/socket/all.hpp
        common/crypto/crypto.cpp common/crypto/crypto.h
        person.cpp
        person.h
        request.cpp request.h
        Response.cpp Response.h
        common/pub.cpp
        common/pub.h
        server/handler.cpp
        server/handler.h
        common/types.h
)
target_include_directories(Client PUBLIC
        Botan::Botan
)
target_link_libraries(Client PUBLIC
        Botan::Botan
        sqlite4cx
        glaze::glaze
)
