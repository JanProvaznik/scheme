release:
	@echo "building release"
	g++ -O3 -o scheme main.cpp

debug:
	@echo "building debug"
	g++ -Wall -Werror -Wextra --std=c++20 -o scheme main.cpp

debug-clang:
	clang -Wall -Werror -Wextra --std=c++20 -o scheme main.cpp

repl: release
	./scheme