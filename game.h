#ifndef GAME_H
#define GAME_H

#include <stdbool.h>

#include "tetris.h"
#include "button.h"

#include "sdl2/SDL.h"
#include "sdl2/SDL_events.h"

#define BOARD_X 75
#define SCORE_CLEAR 100
#define SCORE_MULTIPLIER 1.25f
#define FALL_DELAY 500
#define INPUT_DELAY 35

void game_init(SDL_Renderer *renderer);
void game_update(enum state *next_state, Uint32 mouse_state, Uint32 mouse_state_last, int x, int y, const bool *key_state);
void game_keydown(SDL_KeyboardEvent key);
void game_render(SDL_Renderer *renderer);
void game_destroy();

void game_reset();

#endif /* GAME_H */