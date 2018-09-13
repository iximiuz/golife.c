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
    srand(time(NULL));
    
    struct Game *game = argc == 1 
        ? game_new_random(80, 60)
        : game_load(argv[1]);
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
    game_destroy(game);

    return 0;
}

