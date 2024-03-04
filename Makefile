CXX = g++
FLAGS = -Wall -Werror -Wextra -std=c++17 -O3
TEST_FLAGS = -lgtest -pthread

OS := $(shell uname)
ifeq ($(OS), Linux)
    TEST_FLAGS += -lgtest_main
else ifeq ($(OS), Darwin) 
    TEST_FLAGS += -lgmock
endif

.PHONY: all build rebuild open tests clean style leaks

all: tests build open

build: clean
	mkdir build
	cd build && rm -rf * && cmake ..
	cd build && cmake --build .

rebuild:
	cd build && cmake --build .

open:
	cd build && ./Crypto_CPP

tests: clean
	cd ./tests/ && $(CXX) $(FLAGS) unit_tests.cc -o unit_tests $(TEST_FLAGS)
	cd ./tests/ && ./unit_tests

clean:
	rm -rf build tests/unit_tests

style:
	clang-format -n -style=GOOGLE *.cc */*.cc */*.hpp */*/*.hpp

leaks:
	rm -rf a.out && g++ -Wall -Werror -Wextra -O3 -std=c++17 src/main.cc && clear && valgrind --leak-check=full ./a.out
