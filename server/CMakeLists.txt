cmake_minimum_required(VERSION 2.8)

project(server)
add_executable(${PROJECT_NAME} "main.cpp"
                               "server.h"
                               "server.cpp")
