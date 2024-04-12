cmake_minimum_required(VERSION 3.15)
project(test)

set(CMAKE_CXX_STANDARD 17)


file(GLOB SRC
    ${PROJECT_SOURCE_DIR}/*.cpp
)

set(EXECUTABLE_OUTPUT_PATH    ${PROJECT_SOURCE_DIR})

add_executable(test ${SRC})