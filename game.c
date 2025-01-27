#include "game.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <math.h>

#ifdef EMSCRIPTEN
#include <emscripten.h>
#endif

#include "text.h"
#include "tetris.h"
#include "button.h"

#include "SDL2/SDL.h"
#include "SDL2/SDL_events.h"
#include "SDL2/SDL_scancode.h"
#include "SDL2/SDL_timer.h"

T_Button btn_leave;
T_Button btn_resume;
T_Text text_score;
T_Text text_game_over;
T_Text text_paused;

SDL_Color COLORS[7] = {{0x00, 0xFF, 0xFF, 0xFF},  // cyan   0
                       {0xFF, 0xFF, 0x00, 0xFF},  // yellow 1
                       {0x80, 0x00, 0x80, 0xFF},  // purple 2
                       {0x00, 0xFF, 0x00, 0xFF},  // green  3
                       {0xFF, 0x00, 0x00, 0xFF},  // red    4
                       {0xFF, 0xC0, 0xCB, 0xFF},  // pink   5
                       {0xFF, 0x7F, 0x00, 0xFF}}; // orange 6

SDL_Rect rect_grid;
SDL_Rect rect_block;

typedef struct {
    int coords[4][2];
    int origin;
    int color;
} T_FallingPiece;

T_FallingPiece piece_i = {{{0, 0}, {1, 0}, {2, 0}, {3, 0}},  1, 0};
T_FallingPiece piece_o = {{{0, 0}, {1, 0}, {0, 1}, {1, 1}}, -1, 1};
T_FallingPiece piece_j = {{{2, 0}, {2, 1}, {1, 1}, {0, 1}},  2, 2};
T_FallingPiece piece_s = {{{1, 0}, {1, 1}, {0, 1}, {0, 2}},  0, 3};
T_FallingPiece piece_z = {{{0, 0}, {0, 1}, {1, 1}, {1, 2}},  2, 4};
T_FallingPiece piece_a = {{{0, 0}, {0, 1}, {1, 1}, {0, 2}},  1, 5};
T_FallingPiece piece_l = {{{0, 0}, {1, 0}, {2, 0}, {2, 1}},  0, 6};

T_FallingPiece falling_piece;
T_FallingPiece piece_next;

bool game_paused = false;
bool game_over = false;
bool score_changed;

int score = 0;
int time_move = 0;
int game_board[ROWS][COLUMNS];

long time_paused;
long time_last_fall;
long time_last_move;
bool input_held_last = false;

void board_clear() {
    for (int y = 0; y < ROWS; y++) {
        for (int x = 0; x < COLUMNS; x++) {
            game_board[y][x] = -1;
        }
    }
}

void board_render(SDL_Renderer *renderer) {
    for (int y = 0; y < ROWS; y++) {
        for (int x = 0; x < COLUMNS; x++) {
            rect_grid.x = x*BLOCK_SIZE + BOARD_X;
            rect_grid.y = y*BLOCK_SIZE;
            rect_block.x = rect_grid.x + 2;
            rect_block.y = rect_grid.y + 2;

            SDL_SetRenderDrawColor(renderer, 0x80, 0x80, 0x80, 0x80);
            SDL_RenderDrawRect(renderer, &rect_grid);

             if (game_board[y][x] != -1) {
                SDL_Color color = COLORS[game_board[y][x]];
                SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
                SDL_RenderFillRect(renderer, &rect_block);
             }
        }
    }
}

void board_clear_row(int y) {
    for (int x = 0; x < COLUMNS; x++) {
        game_board[y][x] = -1;
    }

    for (int j = y; j > 0; j--) {
        for (int x = 0; x < COLUMNS; x++) {
            game_board[j][x] = game_board[j-1][x];
        }
    }

    //comment out to get back epic bug
    for (int x = 0; x < COLUMNS; x++) {
        game_board[0][x] = -1;
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

void set_random_piece(T_FallingPiece *piece) {
    switch (rand() % 7) {
        case 0:
            memcpy(piece, &piece_o, sizeof(T_FallingPiece));
            break;
        case 1:
            memcpy(piece, &piece_i, sizeof(T_FallingPiece));
            break;
        case 2:
            memcpy(piece, &piece_s, sizeof(T_FallingPiece));
            break;
        case 3:
            memcpy(piece, &piece_z, sizeof(T_FallingPiece));
            break;
        case 4:
            memcpy(piece, &piece_l, sizeof(T_FallingPiece));
            break;
        case 5:
            memcpy(piece, &piece_j, sizeof(T_FallingPiece));
            break;
        case 6:
            memcpy(piece, &piece_a, sizeof(T_FallingPiece));
            break;
    }
}

void falling_piece_spawn() {
    memcpy(&falling_piece, &piece_next, sizeof(T_FallingPiece));

    set_random_piece(&piece_next);

    falling_piece_transform(0, 4);

    if (falling_piece_check_collision()) {
        game_over = true;
        
        #ifdef EMSCRIPTEN
        char script[64];
        sprintf(script, "gameEnded(%d);", score);
        emscripten_run_script(script);
        #endif
    }
}

void falling_piece_render(SDL_Renderer *renderer, T_FallingPiece *falling_piece, bool next) {
    for (int i = 0; i < 4; i++) {
        int y = falling_piece->coords[i][0];
        int x = falling_piece->coords[i][1];

        if (next) {
            y += 9;
            x += 13;
        }

        rect_block.y = y*BLOCK_SIZE + 2;
        rect_block.x = x*BLOCK_SIZE + BOARD_X + 2;
        
        SDL_Color color = COLORS[falling_piece->color];
        SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
        SDL_RenderFillRect(renderer, &rect_block);
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

void falling_piece_update(const bool *key_state) {
    int dx = 0;
    int dy = 0;

    long time = SDL_GetTicks64();
    bool input_held = (key_state[SDL_SCANCODE_LEFT] || key_state[SDL_SCANCODE_RIGHT] || key_state[SDL_SCANCODE_DOWN] ||
                       key_state[SDL_SCANCODE_KP_4] || key_state[SDL_SCANCODE_KP_6] || key_state[SDL_SCANCODE_KP_5]);

    if (time-time_last_fall >= FALL_DELAY) {
        long delta = time-time_last_fall;
        int n = (int) delta/FALL_DELAY;

        dy = n;
        time_last_fall = time;
    }

    if (time-time_last_move >= INPUT_DELAY || (input_held && !(input_held_last))) {
        long delta = time-time_last_move;
        int n = (int) delta/INPUT_DELAY;

        if (key_state[SDL_SCANCODE_LEFT] || key_state[SDL_SCANCODE_KP_4]) {
            dx = -n;
        }
        if (key_state[SDL_SCANCODE_RIGHT] || key_state[SDL_SCANCODE_KP_6]) {
            dx = n;
        }
        if (key_state[SDL_SCANCODE_DOWN] || key_state[SDL_SCANCODE_KP_5]) {
            dy = n;
        }

        time_last_move = time;

        if (!(input_held_last))
            time_last_move += 10; // short delay for key repeat
    }

    input_held_last = input_held;

    // check x
    for (int i = 0; i < 4; i++) {
        int next_y = falling_piece.coords[i][0]+dy;
        int next_x = falling_piece.coords[i][1]+dx;

        if (next_x < 0 || next_x >= COLUMNS || game_board[next_y][next_x] != -1) {
            dx = 0;
            break;
        }
    }

    // check y
    for (int i = 0; i < 4; i++) {
        int p_y = falling_piece.coords[i][0];
        int p_x = falling_piece.coords[i][1];

        int next_x = p_x+dx;
        int next_y = p_y+dy;

        if (next_y >= ROWS || game_board[next_y][next_x] != -1) {
            falling_piece_land();
            falling_piece_spawn();
            return;
        }
            
    }
    
    falling_piece_transform(dy, dx);
}   

void score_text_update(SDL_Renderer *renderer) {
    text_destroy(&text_score);

    char score_text[16];
    sprintf(score_text, "%d", score);
    text_create(&text_score, renderer, font_score, score_text, white);

    text_score.rect.x = 500;
    text_score.rect.y = 125;
}

void game_render(SDL_Renderer *renderer) {
    if (game_paused) {
        text_render(&text_paused, renderer);
        button_render(&btn_resume, renderer);
    }
    else {
        if (score_changed) 
            score_text_update(renderer);
        score_changed = 0;

        board_render(renderer);
        falling_piece_render(renderer, &falling_piece, false);
        falling_piece_render(renderer, &piece_next, true);

        text_render(&text_score, renderer);
        
        if (game_over)
            text_render(&text_game_over, renderer);
    }
    
    if (game_paused || game_over) {
        button_render(&btn_leave, renderer);
    }
}

void game_update(enum state *next_state, Uint32 mouse_state, Uint32 mouse_state_last, int x, int y, const bool *key_state) {
    if (game_paused) {
        button_update(&btn_resume, mouse_state, mouse_state_last, x, y);
        if (btn_resume.state & BUTTON_PRESSED)
            game_paused = false;
    }
    else {
        if (!game_over)
            falling_piece_update(key_state);
    }

    if (game_paused || game_over) {
        button_update(&btn_leave, mouse_state, mouse_state_last, x, y);
        if (btn_leave.state & BUTTON_PRESSED) {
            *next_state = STATE_MENU;

            #ifdef EMSCRIPTEN
            emscripten_run_script("gameQuit();");
            #endif
        }
    }
}

void game_keydown(SDL_KeyboardEvent key) {
    if (!game_paused) {
        if ((key.keysym.scancode == SDL_SCANCODE_R || key.keysym.scancode == SDL_SCANCODE_UP || key.keysym.scancode == SDL_SCANCODE_KP_8) && !(key.repeat)) {
            falling_piece_rotate();
        }
    }

    if (key.keysym.scancode == SDL_SCANCODE_ESCAPE && !(key.repeat)) {
        game_paused = !(game_paused);
        if (game_over)
            game_paused = false;

        if (game_paused)
            time_paused = SDL_GetTicks64();
        else {
            time_last_fall += SDL_GetTicks64()-time_paused; // compensate for time paused
            time_last_move += SDL_GetTicks64()-time_paused;
        }
    }
}

void game_reset() {
    set_random_piece(&piece_next);
    score = 0;
    score_changed = 1;
    game_over = false;
    game_paused = false;
    board_clear();
    falling_piece_spawn();

    time_last_fall = SDL_GetTicks64();
    time_last_move = SDL_GetTicks64();

    #ifdef EMSCRIPTEN
    emscripten_run_script("gameStarted();");
    #endif
}

void game_init(SDL_Renderer *renderer) {
    srand(time(NULL));

    button_create(&btn_resume, renderer, font_button, "Continue");
    button_set_size(&btn_resume, 450, 80);
    button_set_pos(&btn_resume, 110, 260);

    button_create(&btn_leave, renderer, font_button, "Quit");
    button_set_size(&btn_leave, 450, 80);
    button_set_pos(&btn_leave, 110, 380);

    text_create(&text_game_over, renderer, font_title, "Game Over!", white);
    text_create(&text_paused, renderer, font_title, "Paused...", white);

    text_paused.rect.y = 60;

    score_text_update(renderer);

    rect_grid.w = BLOCK_SIZE+1;
    rect_grid.h = BLOCK_SIZE+1;
    rect_block.w = BLOCK_SIZE-3;
    rect_block.h = BLOCK_SIZE-3;
}

void game_destroy() {
    button_destroy(&btn_leave);
    button_destroy(&btn_resume);
    text_destroy(&text_score);
    text_destroy(&text_game_over);
    text_destroy(&text_paused);
}