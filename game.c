#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <math.h>
#include "game.h"
#include "tetris.h"
#include "button.h"

#include "sdl2/SDL.h"
#include "sdl2/SDL_scancode.h"

#define BOARD_X 75
#define SCORE_CLEAR 100
#define SCORE_MULTIPLIER 1.25f

SDL_Color COLORS[7] = {{0x00, 0xFF, 0xFF, 0xFF},  // cyan   0
                       {0xFF, 0xFF, 0x00, 0xFF},  // yellow 1
                       {0x80, 0x00, 0x80, 0xFF},  // purple 2
                       {0x00, 0xFF, 0x00, 0xFF},  // green  3
                       {0xFF, 0x00, 0x00, 0xFF},  // red    4
                       {0xFF, 0xC0, 0xCB, 0xFF},  // pink   5
                       {0xFF, 0x7F, 0x00, 0xFF}}; // orange 6

SDL_Rect g_r;
SDL_Rect b_r;

typedef struct {
    int coords[4][2];
    int origin;
    int color;
} T_FallingPiece;

T_FallingPiece piece_o = {{{0, 0}, {1, 0}, {0, 1}, {1, 1}}, -1, 1};
T_FallingPiece piece_i = {{{0, 0}, {1, 0}, {2, 0}, {3, 0}}, 1, 0};
T_FallingPiece piece_s = {{{1, 0}, {1, 1}, {0, 1}, {0, 2}}, 0, 3};
T_FallingPiece piece_z = {{{0, 0}, {0, 1}, {1, 1}, {1, 2}}, 2, 4};
T_FallingPiece piece_l = {{{0, 0}, {1, 0}, {2, 0}, {2, 1}}, 0, 6};
T_FallingPiece piece_j = {{{2, 0}, {2, 1}, {1, 1}, {0, 1}}, 2, 2};
T_FallingPiece piece_a = {{{0, 0}, {0, 1}, {1, 1}, {0, 2}}, 1, 5};

T_FallingPiece falling_piece;
int game_over = 0;
int score = 0;
int score_changed;
int t_fall = 0;
int t_move = 0;
int game_board[ROWS][COLUMNS];

void board_clear() {
    for (int y = 0; y < ROWS; y++) {
        for (int x = 0; x < COLUMNS; x++) {
            game_board[y][x] = -1;//rand() % 7;
        }
    }
}

void board_render(SDL_Renderer *renderer) {
    for (int y = 0; y < ROWS; y++) {
        for (int x = 0; x < COLUMNS; x++) {
            //WIDTH-(COLUMNS*BLOCK_SIZE)
            g_r.x = x*BLOCK_SIZE + BOARD_X;
            g_r.y = y*BLOCK_SIZE;
            b_r.x = g_r.x + 2;
            b_r.y = g_r.y + 2;

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

void board_clear_row(int y) {
    // clear row
    for (int x = 0; x < COLUMNS; x++) {
        game_board[y][x] = -1;
    }

    for (int j = y; j > 0; j--) {
        for (int x = 0; x < COLUMNS; x++) {
            game_board[j][x] = game_board[j-1][x];
        }
    }
}

void board_check_rows() {
    int x;
    int y;
    int cleared = 0;
    for (y = 0; y < ROWS; y++) {
        for (x = 0; x < COLUMNS; x++) {
            if (game_board[y][x] == -1)
                break;
        }

        if (x >= COLUMNS) {
            board_clear_row(y);
            score += ceil(SCORE_CLEAR*pow(SCORE_MULTIPLIER, cleared));
            cleared++;
            score_changed = 1;
        }  
    }
}

void falling_piece_transform(int dy, int dx) {
    for (int i = 0; i < 4; i++) {
        falling_piece.coords[i][0] += dy;
        falling_piece.coords[i][1] += dx;
    }
}

int falling_piece_check_collision() {
    for (int i = 0; i < 4; i++) {
        int y = falling_piece.coords[i][0];
        int x = falling_piece.coords[i][1];

        if (game_board[y][x] != -1)
            return 1;
    }

    return 0;
}

void falling_piece_rotate() {
    // rotation disabled (square)
    if (falling_piece.origin == -1)
        return;

    T_FallingPiece tmp;
    memcpy(&tmp, &falling_piece, sizeof(T_FallingPiece));

    int originY = tmp.coords[tmp.origin][0]; 
    int originX = tmp.coords[tmp.origin][1];

    for (int i = 0; i < 4; i++) {
        if (i != tmp.origin) {
            int y = tmp.coords[i][0];
            int x = tmp.coords[i][1];

            int newY = originX - x + originY;
            int newX = y - originY + originX;

            if (newY < 0 || newY >= ROWS || newX < 0 || newX >= COLUMNS || game_board[newY][newX] != -1)
                return;

            tmp.coords[i][0] = newY;
            tmp.coords[i][1] = newX;
        }
    }

    memcpy(&falling_piece, &tmp, sizeof(T_FallingPiece));
}

void falling_piece_spawn() {
    switch (rand() % 7) {
        case 0:
            memcpy(&falling_piece, &piece_o, sizeof(T_FallingPiece));
            break;
        case 1:
            memcpy(&falling_piece, &piece_i, sizeof(T_FallingPiece));
            break;
        case 2:
            memcpy(&falling_piece, &piece_s, sizeof(T_FallingPiece));
            break;
        case 3:
            memcpy(&falling_piece, &piece_z, sizeof(T_FallingPiece));
            break;
        case 4:
            memcpy(&falling_piece, &piece_l, sizeof(T_FallingPiece));
            break;
        case 5:
            memcpy(&falling_piece, &piece_j, sizeof(T_FallingPiece));
            break;
        case 6:
            memcpy(&falling_piece, &piece_a, sizeof(T_FallingPiece));
            break;
    }

    falling_piece_transform(0, 4);

    if (falling_piece_check_collision()) {
        game_over = 1;
    }
}

void falling_piece_render(SDL_Renderer *renderer) {
    for (int i = 0; i < 4; i++) {
        b_r.y = falling_piece.coords[i][0]*BLOCK_SIZE + 2;
        b_r.x = falling_piece.coords[i][1]*BLOCK_SIZE + BOARD_X + 2;
        
        SDL_Color clr = COLORS[falling_piece.color];
        SDL_SetRenderDrawColor(renderer, clr.r, clr.g, clr.b, clr.a);
        SDL_RenderFillRect(renderer, &b_r);
    }
}

void falling_piece_land() {
    for (int i = 0; i < 4; i++) {
        int p_x = falling_piece.coords[i][1];
        int p_y = falling_piece.coords[i][0];

        game_board[p_y][p_x] = falling_piece.color;
    }
    board_check_rows();
}

void falling_piece_update(int num_keys, const bool *key_state) {
    int falling = 1;
    int dx = 0;
    int dy = 0;

    // move down every 0.5 seconds
    if (++t_fall >= (int)TARGET_FPS/2) {
        dy = 1;
        t_fall = 0;
    }

    // get input every 0.25 seconds
    if (++t_move >= (int)TARGET_FPS/16) {
        if (key_state[SDL_SCANCODE_RIGHT]) {
            dx = 1;
        }
        if (key_state[SDL_SCANCODE_LEFT]) {
            dx = -1;
        }
        if (key_state[SDL_SCANCODE_DOWN]) {
            dy = 1;
        }

        t_move = 0;
    }

    // check x
    for (int i = 0; i < 4; i++) {
        int next_y = falling_piece.coords[i][0]+dy;
        int next_x = falling_piece.coords[i][1]+dx;

        if (next_x < 0 || next_x >= COLUMNS || game_board[next_y][next_x] != -1) {
            dx = 0;
            break;
        }
    }

    // check if can fall/move
    for (int i = 0; i < 4; i++) {
        int p_y = falling_piece.coords[i][0];
        int p_x = falling_piece.coords[i][1];

        int next_x = p_x+dx;
        int next_y = p_y+dy;

        if (next_y >= ROWS || game_board[next_y][next_x] != -1)
            falling = 0;
    }

    // fell
    if (!falling) {
        falling_piece_land();
        falling_piece_spawn();
        return;
    }

    // update movement
    falling_piece_transform(dy, dx);
}

void score_text_update(SDL_Renderer *renderer) {
    text_destroy(&text_score);

    char score_text[16];
    sprintf(score_text, "%d", score);
    text_create(&text_score, renderer, font_score, score_text, white);

    text_score.rect.x = 500;
    text_score.rect.y = 150;
}

void game_reset() {
    score = 0;
    score_changed = 1;
    game_over = 0;
    board_clear();
    falling_piece_spawn();
}

void game_init(SDL_Renderer *renderer) {
    srand(time(NULL));

    button_create(&btn_leave, renderer, font_button, "Quit");
    button_set_size(&btn_leave, 170, 70);
    button_set_pos(&btn_leave, 457, 570);
    board_clear();

    text_create(&text_game_over, renderer, font_title, "Game Over!", white);

    score_text_update(renderer);

    g_r.w = BLOCK_SIZE+1;
    g_r.h = BLOCK_SIZE+1;
    b_r.w = BLOCK_SIZE-3;
    b_r.h = BLOCK_SIZE-3;
}

void game_render(SDL_Renderer *renderer) {
    if (score_changed) 
        score_text_update(renderer);
    score_changed = 0;

    board_render(renderer);
    falling_piece_render(renderer);

    button_render(&btn_leave, renderer);
    button_render(&btn_leave, renderer);
    text_render(&text_score, renderer);
    
    if (game_over)
        text_render(&text_game_over, renderer);
}

void game_update(enum state *next_state, Uint32 mouse_state, int x, int y, int num_keys, const bool *key_state) {
    button_update(&btn_leave, mouse_state, x, y);

    if (!game_over)
        falling_piece_update(num_keys, key_state);

    if (btn_leave.state & BUTTON_PRESSED)
        *next_state = STATE_MENU;
}

void game_keydown(int key) {
    if (key == SDL_SCANCODE_R) {
        falling_piece_rotate();
    }
}

void game_destroy() {
    button_destroy(&btn_leave);
    text_destroy(&text_score);
    text_destroy(&text_game_over);
}