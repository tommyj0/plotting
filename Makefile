all:
	gcc -Wall -Wextra -o bin/debug/main src/main.c -lm -std=c17 -I include

	.\bin\debug\main.exe