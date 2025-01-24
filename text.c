#include "text.h"
#include "tetris.h"

#include "SDL2/SDL.h"
#include "SDL2/SDL_ttf.h"

TTF_Font *font_title;
TTF_Font *font_button;
TTF_Font *font_text;
TTF_Font *font_score;

void text_load_fonts() {
    TTF_Init();

    font_title = TTF_OpenFont("assets/OpenSans-ExtraBold.ttf", 72);
    font_button = TTF_OpenFont("assets/OpenSans-SemiBold.ttf", 42);
    font_text = TTF_OpenFont("assets/OpenSans-Regular.ttf", 26);
    font_score = TTF_OpenFont("assets/OpenSans-SemiBold.ttf", 52);
}

void text_destroy_fonts() {
    TTF_CloseFont(font_title);
    TTF_CloseFont(font_button);
    TTF_CloseFont(font_text);
    TTF_CloseFont(font_score);
    TTF_Quit();
}

void text_create(T_Text *text, SDL_Renderer *renderer, TTF_Font *font, const char *text_string, SDL_Color color) {
    text->surface = TTF_RenderText_Blended(font, text_string, color);
    text->texture = SDL_CreateTextureFromSurface(renderer, text->surface);

    text->rect.x = (int) (WIDTH-text->surface->w)/2;
    text->rect.y = 0;
    text->rect.w = text->surface->w;
    text->rect.h = text->surface->h;
}

void text_render(T_Text *text, SDL_Renderer *renderer) {
    SDL_RenderCopy(renderer, text->texture, NULL, &text->rect);
}

void text_destroy(T_Text *text) {
    SDL_DestroyTexture(text->texture);
    SDL_FreeSurface(text->surface);
}
