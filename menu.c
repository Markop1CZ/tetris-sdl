#include "menu.h"

#include "tetris.h"
#include "button.h"
#include "game.h"
#include "text.h"

#include "SDL2/SDL.h"
#include "SDL2/SDL_mouse.h"

T_Text text_title;
T_Text text_info;
T_Button btn_play;
T_Button btn_quit;

void menu_init(SDL_Renderer *renderer) {
    text_create(&text_title, renderer, font_title, "Tetris", white);
    text_title.rect.y = 60;

    text_create(&text_info, renderer, font_text, "Made in 2024 by Markop1CZ", white);
    text_info.rect.x = WIDTH-text_info.rect.w-5;
    text_info.rect.y = HEIGHT-text_info.rect.h-5;

    button_create(&btn_play, renderer, font_button, "Play");
    button_set_size(&btn_play, 450, 80);
    button_set_pos(&btn_play, 110, 260);

    button_create(&btn_quit, renderer, font_button, "Quit");
    button_set_size(&btn_quit, 450, 80);
    button_set_pos(&btn_quit, 110, 380);
}

void menu_destroy() {
    text_destroy(&text_title);
    text_destroy(&text_info);
    button_destroy(&btn_play);
    button_destroy(&btn_quit);
}

void menu_render(SDL_Renderer *renderer) {
    text_render(&text_title, renderer);
    text_render(&text_info, renderer);
    button_render(&btn_play, renderer);
    button_render(&btn_quit, renderer);
}

void menu_update(enum state *next_state, Uint32 mouse_state, Uint32 mouse_state_last, int x, int y) {
    button_update(&btn_play, mouse_state, mouse_state_last, x, y);
    button_update(&btn_quit, mouse_state, mouse_state_last, x, y);

    if (btn_play.state & BUTTON_PRESSED) {
        game_reset();
        *next_state = STATE_GAME;
    }
    if (btn_quit.state & BUTTON_PRESSED)
        *next_state = STATE_QUIT;
}