cmake_minimum_required(VERSION 3.22)

project(DinoSho)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -lSDL2 -lSDL2_ttf -lSDL2_image")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

file(GLOB src "./*.cpp")

add_executable(DinoSho main.cpp ${src})
