#define SDL_MAIN_HANDLED

#include "tetris.h"

#include <stdbool.h>

#ifdef EMSCRIPTEN
#include <emscripten.h>
#endif

#include "menu.h"
#include "game.h"
#include "text.h"
#include "button.h"

#include "sdl2/SDL.h"
#include "sdl2/SDL_mouse.h"
#include "sdl2/SDL_ttf.h"

SDL_Color alpha = {0x00, 0x00, 0x00, 0x00};
SDL_Color white = {0xFF, 0xFF, 0xFF, 0xFF};
SDL_Color black = {0x00, 0x00, 0x00, 0xFF};

SDL_Window *window;
SDL_Renderer *renderer;

enum state GAME_STATE = STATE_MENU;
bool GAME_RUNNING = true;

Uint32 mouse_state;
Uint32 mouse_state_last = 0;
int mouse_x;
int mouse_y;

int num_keys;
const bool *key_state;

SDL_Event event;

void game_loop() {
    int time_start = SDL_GetTicks();

    mouse_state = SDL_GetMouseState(&mouse_x, &mouse_y);
    key_state = (bool*)SDL_GetKeyboardState(&num_keys);

    while(SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
            GAME_RUNNING = false;
            break;
        }
        if (GAME_STATE == STATE_GAME && event.type == SDL_KEYDOWN) {
            game_keydown(event.key);
        }
    }

    SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0x00);
    SDL_RenderClear(renderer);

    switch(GAME_STATE) {
        case STATE_QUIT:
            GAME_RUNNING = false;
            break;
        case STATE_MENU:
            menu_update(&GAME_STATE, mouse_state, mouse_state_last, mouse_x, mouse_y);
            menu_render(renderer);
            break;
        case STATE_GAME:
            game_update(&GAME_STATE, mouse_state, mouse_state_last, mouse_x, mouse_y, key_state);
            game_render(renderer);
            break;
    }

    SDL_RenderPresent(renderer);

    #ifndef EMSCRIPTEN
    // FPS throttling
    int delta = SDL_GetTicks() - time_start;
    if (delta < 1000/TARGET_FPS) {
        SDL_Delay(1000/TARGET_FPS - delta);
    }
    #endif

    mouse_state_last = mouse_state;

    #ifdef EMSCRIPTEN
    if (!(GAME_RUNNING)) {
        emscripten_cancel_main_loop();
    }
    #endif
}

int main() {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't initialize SDL: %s", SDL_GetError());
        return 3;
    }

    if (SDL_CreateWindowAndRenderer(WIDTH, HEIGHT, 0, &window, &renderer)) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't create window and renderer: %s", SDL_GetError());
        return 3;
    }

    SDL_SetWindowTitle(window, "Tetris");

    text_load_fonts();
    menu_init(renderer);
    game_init(renderer);
    
    #ifndef EMSCRIPTEN
    while (GAME_RUNNING) {
        game_loop();
    }
    #endif

    #ifdef EMSCRIPTEN
    emscripten_set_main_loop(game_loop, 0, true);
    #endif

    text_destroy_fonts();
    menu_destroy();
    game_destroy();

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}