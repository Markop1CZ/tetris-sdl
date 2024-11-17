#ifndef GAME_H
#define GAME_H

#include "tetris.h"
#include "button.h"
#include "sdl2/SDL.h"

T_Button btn_leave;

void game_init(SDL_Renderer *renderer);
void game_update(enum state *next_state, Uint32 mouse, int x, int y);
void game_render(SDL_Renderer *renderer);
void game_destroy();

void game_shuffle();

#endif /* GAME_H */