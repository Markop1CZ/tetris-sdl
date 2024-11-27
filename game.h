#ifndef GAME_H
#define GAME_H

#include <stdbool.h>
#include "tetris.h"
#include "button.h"
#include "sdl2/SDL.h"

T_Button btn_leave;
T_Text text_score;

void game_init(SDL_Renderer *renderer);
void game_update(enum state *next_state, Uint32 mouse, int x, int y, int num_keys, const bool *key_state);
void game_render(SDL_Renderer *renderer);
void game_destroy();

void game_reset();

#endif /* GAME_H */