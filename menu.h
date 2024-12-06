#ifndef MENU_H
#define MENU_H

#include "tetris.h"
#include "button.h"
#include "sdl2/SDL.h"

T_Text text_title;
T_Text text_info;
T_Button btn_play;
T_Button btn_quit;

void menu_init(SDL_Renderer *renderer);
void menu_update(enum state *next_state, Uint32 mouse_state, Uint32 mouse_state_last, int x, int y);
void menu_render(SDL_Renderer *renderer);
void menu_destroy();

#endif /* MENU_H */