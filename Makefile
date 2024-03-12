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
	cd tests && mkdir tests_build
	cd tests/tests_build && rm -rf * && cmake ..
	cd tests/tests_build && cmake --build .
	cd tests/tests_build && ./unit_tests

style:
	clang-format -n -style=GOOGLE *.cc */*.cc */*.hpp */*/*.hpp

leaks: build
	valgrind --leak-check=full ./build/Crypto_CPP

clean_build:
	rm -rf build

clean_test:
	rm -rf tests/tests_build
	cd datasets/files && rm -rf *_encoded* *_decoded*
	cd datasets/configurations && rm -rf public_key private_key

clean: clean_build clean_test
