#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

struct Field {
    int *grid;
    int width;
    int height;
};

int *grid_new(int width, int height) {
    int nobytes = width * height * sizeof(int);
    int *grid = malloc(nobytes);
    if (!grid) {
        fprintf(stderr, "grid::malloc failed\n");
        return NULL;
    }

    memset(grid, 0, nobytes);
    return grid;
}

void grid_destroy(int *grid) {
    free(grid);
}

struct Field *field_new(int width, int height) {
    struct Field *f = malloc(sizeof(struct Field));
    if (!f) {
        fprintf(stderr, "field::malloc failed\n");
        return NULL;
    }

    f->grid = grid_new(width, height);
    if (!f->grid) {
        free(f);
        return NULL;
    }

    f->width = width;
    f->height = height;
    return f;
}

void field_destroy(struct Field *f) {
    if (f) {
        grid_destroy(f->grid);
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

static inline int max(int x, int y) {
    return x > y ? x : y;
}

static inline int min(int x, int y) {
    return x > y ? y : x;
}

int field_adjust_size(struct Field *f) {
    int left = f->width, top = f->height, right = -1, bottom = -1;
    for (int x = 0; x < f->width; x++) {
        for (int y = 0; y < f->height; y++) {
            if (f->grid[y * f->width + x]) {
                left = min(left, x);
                top = min(top, y);
                right = max(right, x);
                bottom = max(bottom, y);
            }
        }
    }

    int add_left = left < 2;
    int add_top = top < 2;
    int add_right = right >= f->width - 2;
    int add_bottom = bottom >= f->height - 2;

    // printf("l=%d t=%d r=%d b=%d\n", add_left, add_top, add_right, add_bottom);

    int width = f->width + add_left + add_right;
    int height = f->height + add_top + add_bottom;
    if (width == f->width && height == f->height) {
        return 0;
    }

    int *grid = grid_new(width, height);
    if (!grid) {
        return -1;
    }

    for (int x = 0; x < f->width; x++) {
        for (int y = 0; y < f->height; y++) {
            grid[(y + add_top) * width + (x + add_left)] = f->grid[y * f->width + x];
        }
    }

    grid_destroy(f->grid);

    f->grid = grid;
    f->width = width;
    f->height = height;

    return 0;
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

int field_count_alive_cells_around(struct Field *f, int x, int y) {
    int result = 0;
    for (int xx = max(0, x - 1); xx < min(f->width, x + 2); xx++) {
        for (int yy = max(0, y - 1); yy < min(f->height, y + 2); yy++) {
            result += (xx != x || yy != y) && field_cell_get(f, xx, yy);
        }
    }
    return result;
}

