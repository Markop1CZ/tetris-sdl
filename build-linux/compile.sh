#!/bin/bash

echo "Copying assets..."
if [ -d "assets" ]; then
    rm -rf "assets"
fi
if [ -d "../assets" ]; then
    cp -r "../assets" "./assets"
fi

echo "Compiling..."
gcc -Wall -Wextra -O3 ../tetris.c ../menu.c ../game.c ../text.c ../button.c -Iusr/include/ -lSDL2main -lSDL2 -lSDL2_ttf -lm -o tetris