#ifndef _GOLIFE_FIELD_H
#define _GOLIFE_FIELD_H

struct Field;

struct Field *field_new(int width, int height);

void field_destroy(struct Field *f);

struct Field *field_copy(struct Field *other);

int field_adjust_size(struct Field *f);

int field_width(struct Field *f);

int field_height(struct Field *f);

int field_cell_get(struct Field *f, int x, int y);

void field_cell_set(struct Field *f, int x, int y, int val);

int field_count_alive_cells_around(struct Field *f, int x, int y);

#endif  // _GOLIFE_FIELD_H

