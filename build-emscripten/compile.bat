@echo off

emcc ../tetris.c ../menu.c ../game.c ../text.c ../button.c -s USE_SDL=2 -s USE_SDL_TTF=2 --embed-file ../assets@assets -o tetris.js