build:
	rm -rf a.out && g++ -Wall -Werror -Wextra -O3 -std=c++17 src/main.cc && clear && ./a.out