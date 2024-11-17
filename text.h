#ifndef TEXT_H
#define TEXT_H

#include "sdl2/SDL_ttf.h"

typedef struct {
    SDL_Surface *surface;
    SDL_Texture *texture;
    SDL_Rect rect;
} T_Text;

TTF_Font *font_title;
TTF_Font *font_button;
TTF_Font *font_text;
TTF_Font *font_score;

void text_load_fonts();
void text_destroy_fonts();
void text_create(T_Text *text, SDL_Renderer *renderer, TTF_Font *font, const char *text_string, SDL_Color color);
void text_render(T_Text *text, SDL_Renderer *renderer);
void text_destroy(T_Text *text);

#endif /* TEXT_H */