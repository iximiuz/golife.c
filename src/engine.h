#ifndef _GOLIFE_ENGINE_H
#define _GOLIFE_ENGINE_H

#include "game.h"

#define ENGINE_DONE 255

struct Engine;

struct Engine *engine_new(int width, int height, int delay, struct Game *game);

void engine_destroy(struct Engine *e);

int engine_step(struct Engine *e);

#endif  // _GOLIFE_ENGINE_H

