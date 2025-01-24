@echo off
rem Set path to SDL_DIR:
set SDL_DIR=C:\dev\lib\SDL2
set SDL_TTF_DIR=C:\dev\lib\SDL2_ttf

echo Copying assets...
if exist "assets" (
    rmdir /s /q "assets"
)
if exist "../assets" (
    xcopy "../assets" "assets" /e /i /h /q
)

echo Compiling...
gcc -mwindows -Wall -Wextra -O3 ../tetris.c ../menu.c ../game.c ../text.c ../button.c -I%SDL_DIR%\include -L%SDL_DIR%\lib -I%SDL_TTF_DIR%\include -L%SDL_TTF_DIR%\lib -lSDL2main -lSDL2 -lSDL2_ttf -o tetris.exe