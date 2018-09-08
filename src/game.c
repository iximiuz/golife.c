#include "field.h"
#include "game.h"
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

struct Game {
    struct Field *field;
};

struct Game *game_new_random(int width, int height) {
    struct Game *g = malloc(sizeof(struct Game));
    if (!g) {
        fprintf(stderr, "game::malloc failed\n");
        return NULL;
    }

    g->field = field_new(width, height);
    if (!g->field) {
        free(g);
        return NULL;
    }

    int alive = width * height * 0.15;
    while (alive) {
        int x = rand() % width;
        int y = rand() % height;

        if (!game_cell_get(g, x, y)) {
            game_cell_spawn(g, x, y);
            alive--;
        }
    }

    if (field_adjust_size(g->field) != 0) {
        field_destroy(g->field);
        free(g);
        return NULL;
    }

    return g;
}

struct Game *game_load(char *state) {
    struct Game *g = malloc(sizeof(struct Game));
    if (!g) {
        fprintf(stderr, "game::malloc failed\n");
        return NULL;
    }

    int width = strchr(state, '\n') - state;
    int len = strlen(state);
    assert((len + 1) % (width + 1) == 0);
    int height = (len + 1) / (width + 1);

    g->field = field_new(width, height);
    if (!g->field) {
        free(g);
        return NULL;
    }

    int correction = 0;
    for (int i = 0; i < len; i++) {
        correction += state[i] == '\n';
        if (state[i] == '1') {
            game_cell_spawn(g, (i - correction) % width, (i - correction) / width);
        }
    }

    if (field_adjust_size(g->field) != 0) {
        field_destroy(g->field);
        free(g);
        return NULL;
    }

    return g;
}

void game_destroy(struct Game *g) {
    if (g) {
        field_destroy(g->field);
        free(g);
    }
}

int game_width(struct Game *g) {
    return field_width(g->field);
}

int game_height(struct Game *g) {
    return field_height(g->field);
}

int game_step(struct Game *g) {
    struct Field *tmp_field = field_copy(g->field);
    for (int x = 0, w = game_width(g); x < w; x++) {
        for (int y = 0, h = game_height(g); y < h; y++) {
            int cell = field_cell_get(tmp_field, x, y);
            int noalives = field_count_alive_cells_around(tmp_field, x, y);
            if (cell) {
                if (noalives != 2 && noalives != 3) {
                    game_cell_kill(g, x, y);
                }
            } else {
                if (noalives == 3) {
                    game_cell_spawn(g, x, y);
                }
            }
        }
    }
    field_destroy(tmp_field);

    return field_adjust_size(g->field);
}

Cell game_cell_get(struct Game *g, int x, int y) {
    return field_cell_get(g->field, x, y);
}

void game_cell_spawn(struct Game *g, int x, int y) {
    field_cell_set(g->field, x, y, CELL_ALIVE);
}

void game_cell_kill(struct Game *g, int x, int y) {
    field_cell_set(g->field, x, y, CELL_DEAD);
}

