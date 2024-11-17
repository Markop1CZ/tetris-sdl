#include <stdio.h>
#include "button.h"
#include "tetris.h"
#include "text.h"

#include "sdl2/SDL.h"
#include "sdl2/SDL_mouse.h"
#include "sdl2/SDL_ttf.h"
#include "sdl2/SDL_rect.h"

void button_create(T_Button *button, SDL_Renderer *renderer, TTF_Font *font, const char *text_string) {
    text_create(&button->text, renderer, font, text_string, black);
    button->rect.x = 0;
    button->rect.y = 0;
    button->rect.w = button->text.rect.w+16;
    button->rect.h = button->text.rect.h+16;
}

void button_center_text(T_Button *button) {
    button->text.rect.x = button->rect.x + (int)(button->rect.w-button->text.rect.w)/2;
    button->text.rect.y = button->rect.y + (int)(button->rect.h-button->text.rect.h)/2;
    
    //printf("button: %d %d %d %d txt: %d %d %d %d\n", button->rect.x, button->rect.y, button->rect.w, button->rect.h, button->text.rect.x, button->text.rect.y, button->text.rect.w, button->text.rect.h);
}

void button_set_pos(T_Button *button, int x, int y) {
    button->rect.x = x;
    button->rect.y = y;

    button_center_text(button);
}

void button_set_size(T_Button *button, int w, int h) {
    button->rect.w = w;
    button->rect.h = h;

    button_center_text(button);
}

void button_render(T_Button *button, SDL_Renderer *renderer) {
    SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);
    if (button->state & BUTTON_HOVER)
        SDL_SetRenderDrawColor(renderer, 0x80, 0x80, 0x80, 0xFF);

    SDL_RenderFillRect(renderer, &button->rect);
    text_render(&button->text, renderer);
}

void button_update(T_Button *button, Uint32 state, int x, int y) {
    SDL_Point p;
    p.x = x;
    p.y = y;

    button->state = 0;

    if (SDL_PointInRect(&p, &button->rect)) {
        button->state |= BUTTON_HOVER;
        if (state & SDL_BUTTON(1)) {
            button->state |= BUTTON_PRESSED;
        }      
    }
}

void button_destroy(T_Button *button) {
    text_destroy(&button->text);
}