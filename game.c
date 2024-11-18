#include <time.h>
#include <stdlib.h>
#include "game.h"
#include "tetris.h"
#include "button.h"
#include "sdl2/SDL.h"

#define BOARD_X 75

SDL_Color COLORS[7] = {{0x00, 0xFF, 0xFF, 0xFF},
                       {0xFF, 0xFF, 0x00, 0xFF},
                       {0x80, 0x00, 0x80, 0xFF},
                       {0x00, 0xFF, 0x00, 0xFF},
                       {0xFF, 0x00, 0x00, 0xFF},
                       {0x00, 0x00, 0xFF, 0xFF},
                       {0xFF, 0x7F, 0x00, 0xFF}};

int score = 0;
int game_board[ROWS][COLUMNS];

void board_clear() {
    for (int y = 0; y < ROWS; y++) {
        for (int x = 0; x < COLUMNS; x++) {
            game_board[y][x] = rand() % 7;
        }
    }
}

void board_render(SDL_Renderer *renderer) {
    SDL_Rect g_r;
    SDL_Rect b_r;

    g_r.w = BLOCK_SIZE+1;
    g_r.h = BLOCK_SIZE+1;
    b_r.w = BLOCK_SIZE-3;
    b_r.h = BLOCK_SIZE-3;
    
    for (int y = 0; y < ROWS; y++) {
        for (int x = 0; x < COLUMNS; x++) {
            //WIDTH-(COLUMNS*BLOCK_SIZE)
            g_r.x = BOARD_X+x*BLOCK_SIZE;
            g_r.y = y*BLOCK_SIZE;
            b_r.x = g_r.x+2;
            b_r.y = g_r.y+2;

            SDL_SetRenderDrawColor(renderer, 0x80, 0x80, 0x80, 0x80);
            SDL_RenderDrawRect(renderer, &g_r);

             if (game_board[y][x] != -1) {
                SDL_Color clr = COLORS[game_board[y][x]];
                SDL_SetRenderDrawColor(renderer, clr.r, clr.g, clr.b, clr.a);
                SDL_RenderFillRect(renderer, &b_r);
             }
        }
    }
}

void game_shuffle() {
    //board_clear();
}

void game_init(SDL_Renderer *renderer) {
    srand(time(NULL));

    button_create(&btn_leave, renderer, font_button, "Quit");
    button_set_size(&btn_leave, 170, 70);
    button_set_pos(&btn_leave, 457, 570);
    board_clear();

    text_create(&text_score, renderer, font_score, "0", white);
    text_score.rect.x = 500;
    text_score.rect.y = 150;
}

void game_render(SDL_Renderer *renderer) {
    button_render(&btn_leave, renderer);
    button_render(&btn_leave, renderer);
    text_render(&text_score, renderer);

    board_render(renderer);
}

void game_update(enum state *next_state, Uint32 mouse_state, int x, int y) {
    button_update(&btn_leave, mouse_state, x, y);

    if (btn_leave.state & BUTTON_PRESSED)
        *next_state = STATE_MENU;
}

void game_destroy() {
    button_destroy(&btn_leave);
}