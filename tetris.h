#ifndef TETRIS_H
#define TETRIS_H

#include "sdl2/SDL.h"

#define TARGET_FPS 100
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

extern SDL_Color alpha;
extern SDL_Color white;
extern SDL_Color black;

#endif /* TETRIS_H */