all:
	gcc -Wall -Wextra -o bin/main src/main.c -lm -std=c17 -I include

	.\bin\main.exe