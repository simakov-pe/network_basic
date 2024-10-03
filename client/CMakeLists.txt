cmake_minimum_required(VERSION 2.8)

project(client)
add_executable(${PROJECT_NAME} "main.cpp"
                               "client.h"
                               "client.cpp")
