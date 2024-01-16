# SHELL=cmd
all:
	gcc src/*.c -lm -std=c17 -I src/ -Wall -Wextra -o bin/main 
	.\bin\main.exe

# cmd /C out\myplot.png
