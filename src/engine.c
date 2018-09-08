#include "engine.h"
#include "game.h"
#include <SDL.h>
#include <time.h>

struct Engine {
    int width;
    int height;
    int delay;
    int done;
    int paused;
    int focused_x;
    int focused_y;
    struct Game *game;

    SDL_Window *sdl_window;
    SDL_Renderer *sdl_renderer;

    Uint32 last_step_at;
};

struct Engine *engine_new(int width, int height, int delay, struct Game *game) {
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        fprintf(stderr, "SDL_Init failed: %s\n", SDL_GetError());
        return NULL;
    }

    struct Engine *e = malloc(sizeof(struct Engine));
    if (!e) {
        fprintf(stderr, "engine::malloc failed\n");
        return NULL;
    }

    e->width = width;
    e->height = height;
    e->delay = delay;
    e->done = 0;
    e->paused = 0;
    e->focused_x = -1;
    e->focused_y = -1;
    e->game = game;

    e->sdl_window = NULL;
    e->sdl_renderer = NULL;

    e->last_step_at = 0;

    if (SDL_CreateWindowAndRenderer(width, height, 0, &e->sdl_window, &e->sdl_renderer) != 0) {
        fprintf(stderr, "SDL_CreateWindowAndRenderer failed: %s\n", SDL_GetError());
        return NULL;
    }

    return e;
}

void engine_destroy(struct Engine *e) {
    if (e) {
        if (e->sdl_renderer) {
            SDL_DestroyRenderer(e->sdl_renderer);
        }
        if (e->sdl_window) {
            SDL_DestroyWindow(e->sdl_window);
        }

        SDL_Quit();
        free(e);
    }
}

static int screen_clear(struct Engine *e) {
    int err;
    if ((err = SDL_SetRenderDrawColor(e->sdl_renderer, 0, 0, 0, SDL_ALPHA_OPAQUE)) != 0) {
        fprintf(stderr, "SDL_SetRenderDrawColor failed: %s\n", SDL_GetError());
        return err;
    }
    if ((err = SDL_RenderClear(e->sdl_renderer)) != 0) {
        fprintf(stderr, "SDL_RenderClear failed: %s\n", SDL_GetError());
        return err;
    }
    return 0;
}

static void screen_present(struct Engine *e) {
    SDL_RenderPresent(e->sdl_renderer);
}

static inline int calc_cell_width(struct Engine *e) {
    return e->width / game_width(e->game);
}

static inline int calc_cell_height(struct Engine *e) {
    return e->height / game_height(e->game);
}

static int draw_grid(struct Engine *e) {
    int err;
    int cell_width = calc_cell_width(e);
    int cell_height = calc_cell_height(e);

    if ((err = SDL_SetRenderDrawColor(e->sdl_renderer, 128, 128, 128, SDL_ALPHA_OPAQUE)) != 0) {
        fprintf(stderr, "SDL_SetRenderDrawColor failed: %s\n", SDL_GetError());
        return err;
    }

    int x = 0;
    while (x < e->width) {
        if ((err = SDL_RenderDrawLine(e->sdl_renderer, x, 0, x, e->height)) != 0) {
            fprintf(stderr, "SDL_RenderDrawLine failed: %s\n", SDL_GetError());
            return err;
        }
        x += cell_width;
    }

    int y = 0;
    while (y < e->height) {
        if ((err = SDL_RenderDrawLine(e->sdl_renderer, 0, y, e->width, y)) != 0) {
            fprintf(stderr, "SDL_RenderDrawLine failed: %s\n", SDL_GetError());
            return err;
        }
        y += cell_height;
    }

    return 0;
}

static int draw_cells(struct Engine *e) {
    int err;
    int cell_width = calc_cell_width(e);
    int margin_left = cell_width / 8;
    int cell_height = calc_cell_height(e);
    int margin_top = cell_height / 8;
    SDL_Rect cell_rect;

    if ((err = SDL_SetRenderDrawBlendMode(e->sdl_renderer, SDL_BLENDMODE_BLEND)) != 0) {
        fprintf(stderr, "SDL_SetRenderDrawBlendMode failed: %s\n", SDL_GetError());
        return err;
    }

    for (int x = 0, gw = game_width(e->game); x < gw; x++) {
        for (int y = 0, gh = game_height(e->game); y < gh; y++) {
            Cell cell = game_cell_get(e->game, x, y);
            if (cell) {
                if ((err = SDL_SetRenderDrawColor(e->sdl_renderer, 0, 255, 0, 127)) != 0) {
                    fprintf(stderr, "SDL_SetRenderDrawColor failed: %s\n", SDL_GetError());
                    return err;
                }

                cell_rect.x = x * cell_width + margin_left;
                cell_rect.y = y * cell_height + margin_top;
                cell_rect.w = cell_width - 2 * margin_left;
                cell_rect.h = cell_height - 2 * margin_top;

                if ((err = SDL_RenderFillRect(e->sdl_renderer, &cell_rect)) != 0) {
                    fprintf(stderr, "SDL_RenderFillRect failed: %s\n", SDL_GetError());
                    return err;
                }
            }

            if (x == e->focused_x && y == e->focused_y) {
                if ((err = SDL_SetRenderDrawColor(e->sdl_renderer, cell*255, !cell*255, 0, 127)) != 0) {
                    fprintf(stderr, "SDL_SetRenderDrawColor failed: %s\n", SDL_GetError());
                    return err;
                }

                cell_rect.x = x * cell_width;
                cell_rect.y = y * cell_height;
                cell_rect.w = cell_width;
                cell_rect.h = cell_height;

                if ((err = SDL_RenderFillRect(e->sdl_renderer, &cell_rect)) != 0) {
                    fprintf(stderr, "SDL_RenderFillRect failed: %s\n", SDL_GetError());
                    return err;
                }
            }
        }
    }

    return 0;
}

static int draw_scene(struct Engine *e) {
    int err;
    if ((err = screen_clear(e)) != 0) {
        fprintf(stderr, "engine::screen_clear failed: %d\n", err);
        return err;
    }

    if ((err = draw_grid(e)) != 0) {
        fprintf(stderr, "engine::draw_grid failed: %d\n", err);
        return err;
    }

    if ((err = draw_cells(e)) != 0) {
        fprintf(stderr, "engine::draw_cells failed: %d\n", err);
        return err;
    }

    screen_present(e);
    return 0;
}

static int handle_user_input(struct Engine *e) {
    int err;
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
            e->done = 1;
            break;
        }
        if (event.type == SDL_KEYUP) {
            if (event.key.keysym.scancode == 0x2C) {
                e->paused = !e->paused;
            }
        }
        if (event.type == SDL_MOUSEMOTION) {
            int x, y;
            SDL_GetMouseState(&x, &y);
            e->focused_x = x / calc_cell_width(e);
            e->focused_y = y / calc_cell_height(e);
        }
        if (event.type == SDL_MOUSEBUTTONDOWN) {
            e->paused = 1;
        }
        if (event.type == SDL_MOUSEBUTTONUP) {
            int x, y;
            SDL_GetMouseState(&x, &y);
            e->focused_x = x / calc_cell_width(e);
            e->focused_y = y / calc_cell_height(e);
            if (game_cell_get(e->game, e->focused_x, e->focused_y)) {
                game_cell_kill(e->game, e->focused_x, e->focused_y);
            } else {
                game_cell_spawn(e->game, e->focused_x, e->focused_y);
            }
        }
    }

    return 0;
}

int engine_step(struct Engine *e) {
    int err;
    if ((err = draw_scene(e) != 0)) {
        return err;
    }

    if (!e->paused) {
        Uint32 now = SDL_GetTicks();
        if ((now - e->last_step_at) >= e->delay) {
            if ((err = game_step(e->game)) != 0) {
                fprintf(stderr, "engine::game_step failed: %d\n", err);
                return err;
            }
            e->last_step_at = now;
        }
    }

    if ((err = handle_user_input(e)) != 0) {
        return err;
    }
    if (e->done) {
        return ENGINE_DONE;
    }

    return 0;
}

