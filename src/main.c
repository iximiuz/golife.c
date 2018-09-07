#include "engine.h"
#include "game.h"
#include <stdlib.h>
#include <time.h>

#ifdef __EMSCRIPTEN__
#include <emscripten.h>

void main_loop(void *engine) {
    engine_step(engine);
}
#endif

int main(int argc, char* argv[]) {
    // TODO: resize
    // TODO: edit via mouse click
    // TODO: textual hint below the grid
    // TODO: load presets
    
    srand(time(NULL));

    // struct Game *game = game_new_random(40, 30);
    struct Game *game = game_load("000000000000000000000\n"
                                  "001000000000000000000\n"
                                  "000100000000000000000\n"
                                  "011100000000000000000\n"
                                  "000000000000000000000\n"
                                  "000000000000000000000\n"
                                  "000000000000000000000\n"
                                  "000000000000000000000\n"
                                  "000000000000000000000\n"
                                  "000000000000000000000\n"
                                  "000000000000000000000\n"
                                  "000000000000000000000\n"
                                  "000000000000000000000\n"
                                  "000000000000000000000\n"
                                  "000000000000000000000");
    if (!game) {
        return 1;
    }

    struct Engine *engine = engine_new(800, 600, 100, game);
    if (!engine) {
        game_destroy(game);
        return 2;
    }

#ifdef __EMSCRIPTEN__
    emscripten_set_main_loop_arg(main_loop, engine, 0, 1);
#else
    while (engine_step(engine) == 0) {
    }
#endif

    engine_destroy(engine);
    return 0;
}

