#ifndef BUTTON_H
#define BUTTON_H

#include "text.h"
#include "SDL2/SDL_ttf.h"

#define BUTTON_HOVER 1
#define BUTTON_PRESSED 2

typedef struct {
    int state;
    T_Text text;
    SDL_Rect rect;
} T_Button;

void button_create(T_Button *button, SDL_Renderer *renderer, TTF_Font *font, const char *text_string);
void button_render(T_Button *button, SDL_Renderer *renderer);
void button_set_pos(T_Button *button, int x, int y);
void button_set_size(T_Button *button, int w, int h);
void button_update(T_Button *button, Uint32 mouse_state, Uint32 mouse_staet_last, int x, int y);
void button_destroy(T_Button *button);

#endif /*BUTTON_H*/