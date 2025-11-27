cmake_minimum_required(VERSION 4.0)
project(ClientServer)

set(CMAKE_CXX_STANDARD 26)

find_package(Botan REQUIRED)

add_executable(Server
        server.cpp server.h
        shared/socket/socket.cpp shared/socket/socket.h
        shared/socket/logger.cpp shared/socket/logger.h
        shared/socket/connector.cpp shared/socket/connector.h
        shared/socket/all.hpp
        shared/crypto/crypto.cpp shared/crypto/crypto.h
)
target_include_directories(Server PUBLIC
        Botan::Botan
)
target_link_libraries(Server PUBLIC
        Botan::Botan
)

add_executable(Client
        client.cpp client.h
        shared/socket/socket.cpp shared/socket/socket.h
        shared/socket/logger.cpp shared/socket/logger.h
        shared/socket/connector.cpp shared/socket/connector.h
        shared/socket/all.hpp
        shared/crypto/crypto.cpp shared/crypto/crypto.h
)
target_include_directories(Client PUBLIC
        Botan::Botan
)
target_link_libraries(Client PUBLIC
        Botan::Botan
)
