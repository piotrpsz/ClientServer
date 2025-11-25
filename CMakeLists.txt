cmake_minimum_required(VERSION 4.0)
project(ClientServer)

set(CMAKE_CXX_STANDARD 26)


add_executable(Server
        server.cpp
        server.h
        shared/socket/socket.cpp
        shared/socket/socket.h
        shared/socket/logger.cpp
        shared/socket/logger.h
)

add_executable(Client
        client.cpp
        client.h
        shared/socket/socket.cpp
        shared/socket/socket.h
        shared/socket/logger.cpp
        shared/socket/logger.h
)
