CXX = g++
FLAGS = -Wall -Werror -Wextra -std=c++17 -O3
TEST_FLAGS = -lgtest -pthread

OS := $(shell uname)
ifeq ($(OS), Linux)
    TEST_FLAGS += -lgtest_main
else ifeq ($(OS), Darwin) 
    TEST_FLAGS += -lgmock
endif

.PHONY: all build rebuild open tests clean_build clean_test clean style leaks

all: tests leaks

build: clean_build
	mkdir build
	cd build && rm -rf * && cmake ..
	cd build && cmake --build .

rebuild:
	cd build && cmake --build .

open:
	cd build && ./Crypto_CPP

tests: clean_test
	cd ./tests/ && $(CXX) $(FLAGS) unit_tests.cc -o unit_tests $(TEST_FLAGS)
	cd ./tests/ && ./unit_tests

style:
	clang-format -n -style=GOOGLE *.cc */*.cc */*.hpp */*/*.hpp

leaks: build
	valgrind --leak-check=full ./build/Crypto_CPP

clean_build:
	rm -rf build

clean_test:
	rm -rf tests/unit_tests

clean: clean_build clean_test
