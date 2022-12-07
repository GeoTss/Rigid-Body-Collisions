#!/bin/zsh
clang++ -std=c++20 -O3 -Wall -lSDL2 -lSDL2_image -L/opt/homebrew/Cellar/../lib -I/opt/homebrew/Cellar/../include -o main *.cpp
