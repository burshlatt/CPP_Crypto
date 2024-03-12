cmake_minimum_required(VERSION 3.10)

project(Crypto_tests)

set(CMAKE_CXX_STANDARD 17)

find_package(GTest REQUIRED)
# include_directories(${GTEST_INCLUDE_DIRS})

set(TEST_SOURCES 
    unit_tests.cc
)

include_directories(
        ${CMAKE_CURRENT_SOURCE_DIR}/../src/model/des
        ${CMAKE_CURRENT_SOURCE_DIR}/../src/model/rsa
        ${CMAKE_CURRENT_SOURCE_DIR}/../src/model/enigma
        ${CMAKE_CURRENT_SOURCE_DIR}/../src/model/huffman
        ${CMAKE_CURRENT_SOURCE_DIR}/../src/third_party/tools/src
)

add_executable(unit_tests ${TEST_SOURCES})

target_link_libraries(unit_tests ${GTEST_LIBRARIES} pthread)
