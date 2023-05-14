all:
	gcc -Wall -Wextra -o bin/main src/*.c -lm -std=c17 -I include

	.\bin\main.exe