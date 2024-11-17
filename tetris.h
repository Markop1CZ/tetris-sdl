#ifndef TETRIS_H
#define TETRIS_H

#include "sdl2/SDL.h"

#define TARGET_FPS 60
#define BLOCK_SIZE 32
#define ROWS 20
#define COLUMNS 10

#define WIDTH COLUMNS*BLOCK_SIZE+350
#define HEIGHT ROWS*BLOCK_SIZE

// 320x640
// 670x640 window size

enum state {
    STATE_QUIT,
    STATE_MENU,
    STATE_GAME,
};

SDL_Color alpha;
SDL_Color white;
SDL_Color black;

#endif /* TETRIS_H */