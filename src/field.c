#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

struct Field {
    int *grid;
    int width;
    int height;
};

struct Field *field_new(int width, int height) {
    struct Field *f = malloc(sizeof(struct Field));
    if (!f) {
        fprintf(stderr, "field::malloc failed\n");
        return NULL;
    }

    int nobytes = width * height * sizeof(int);
    f->grid = malloc(nobytes);
    if (!f->grid) {
        free(f);
        fprintf(stderr, "field::malloc failed\n");
        return NULL;
    }

    memset(f->grid, 0, nobytes);
    f->width = width;
    f->height = height;

    return f;
}

void field_destroy(struct Field *f) {
    if (f) {
        free(f->grid);
        free(f);
    }
}

struct Field *field_copy(struct Field *other) {
    struct Field *f = field_new(other->width, other->height);
    if (!f) {
        return NULL;
    }
    memcpy(f->grid, other->grid, other->width * other->height * sizeof(int));
    return f;
}

int field_width(struct Field *f) {
    return f->width;
}

int field_height(struct Field *f) {
    return f->height;
}

static inline void assert_valid_pos(struct Field *f, int x, int y) {
    assert(0 <= x); assert(x < f->width);
    assert(0 <= y); assert(y < f->height);
}

int field_cell_get(struct Field *f, int x, int y) {
    assert_valid_pos(f, x, y);
    return f->grid[y * f->width + x];
}

void field_cell_set(struct Field *f, int x, int y, int val) {
    assert_valid_pos(f, x, y);
    f->grid[y * f->width + x] = val;
}

static inline int max(int x, int y) {
    return x > y ? x : y;
}

static inline int min(int x, int y) {
    return x > y ? y : x;
}

int field_count_alive_cells_around(struct Field *f, int x, int y) {
    int result = 0;
    for (int xx = max(0, x - 1); xx < min(f->width, x + 2); xx++) {
        for (int yy = max(0, y - 1); yy < min(f->height, y + 2); yy++) {
            result += (xx != x || yy != y) && field_cell_get(f, xx, yy);
        }
    }
    return result;
}

