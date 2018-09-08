#ifndef _GOLIFE_GAME_H
#define _GOLIFE_GAME_H

struct Game;

typedef int Cell;

#define CELL_DEAD  0
#define CELL_ALIVE 1

struct Game *game_new_random(int width, int height);

struct Game *game_load(char *state);

void game_destroy(struct Game *g);

int game_width(struct Game *g);

int game_height(struct Game *g);

int game_step(struct Game *g);

Cell game_cell_get(struct Game *g, int x, int y);

void game_cell_spawn(struct Game *g, int x, int y);

void game_cell_kill(struct Game *g, int x, int y);

#endif  // _GOLIFE_GAME_H

