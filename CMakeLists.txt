cmake_minimum_required(VERSION 3.10)

project(Crypto)

set(CMAKE_CXX_STANDARD 17)

include_directories(
        ${CMAKE_CURRENT_SOURCE_DIR}/src/view
        ${CMAKE_CURRENT_SOURCE_DIR}/src/controller
        ${CMAKE_CURRENT_SOURCE_DIR}/src/model/des
        ${CMAKE_CURRENT_SOURCE_DIR}/src/model/rsa
        ${CMAKE_CURRENT_SOURCE_DIR}/src/model/enigma
        ${CMAKE_CURRENT_SOURCE_DIR}/src/model/huffman
        ${CMAKE_CURRENT_SOURCE_DIR}/src/third_party/tools/src
)

add_executable(Crypto_CPP
        src/main.cc
)

target_compile_options(Crypto_CPP PRIVATE -Wall -Werror -Wextra -O3)
