build:
	rm -rf a.out && g++ -Wall -Werror -Wextra -O3 -std=c++17 src/main.cc && clear && ./a.out

leaks:
	rm -rf a.out && g++ -Wall -Werror -Wextra -O3 -std=c++17 src/main.cc && clear && valgrind --leak-check=full ./a.out

clean:
	rm -rf a.out .vscode
