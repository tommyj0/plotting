# SHELL=cmd
all:
	gcc src/*.c -lm -std=c17 -I include -Wall -Wextra -o bin/main 
	.\bin\main.exe

# cmd /C out\myplot.png
