cmake_minimum_required(VERSION 3.20)
project(wayla LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(raylib 6.2 CONFIG REQUIRED)

add_executable(wayla
    src/main.cpp
    src/room.cpp
    src/player.cpp
)

target_include_directories(wayla PRIVATE src)
target_link_libraries(wayla PRIVATE raylib)
