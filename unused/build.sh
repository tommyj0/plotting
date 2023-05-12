#!/bin/sh

set -xe

clear

gcc -Wall -Wextra -o bin/debug/main src/main.c -lm

./bin/debug/main

open plot.png
