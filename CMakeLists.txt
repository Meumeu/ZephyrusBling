cmake_minimum_required(VERSION 3.0)

project(zephyrusbling)

add_executable(zephyrusbling main.cpp)

install(TARGETS zephyrusbling RUNTIME DESTINATION bin)
