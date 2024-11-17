@echo off
rem Set path to SDL_DIR:
set SDL_DIR=c:\dev\lib\SDL2-64
set SDL_TTF_DIR=c:\dev\lib\SDL2_ttf

gcc -Wall -Wextra -O3 ../tetris.c ../menu.c ../game.c ../text.c ../button.c -I%SDL_DIR%\include -L%SDL_DIR%\lib -I%SDL_TTF_DIR%\include -L%SDL_TTF_DIR%\lib -lSDL2main -lSDL2 -lSDL2_ttf -o tetris.exe