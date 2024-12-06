#ifndef GAME_H
#define GAME_H

#include <stdbool.h>
#include "tetris.h"
#include "button.h"
#include "sdl2/SDL.h"
#include "sdl2/SDL_events.h"

void game_init(SDL_Renderer *renderer);
void game_update(enum state *next_state, Uint32 mouse_state, Uint32 mouse_state_last, int x, int y, int num_keys, const bool *key_state);
void game_keydown(SDL_KeyboardEvent key);
void game_render(SDL_Renderer *renderer);
void game_destroy();

void game_reset();

#endif /* GAME_H */