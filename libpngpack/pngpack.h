#ifndef PNGPACK_LIBRARY_H
#define PNGPACK_LIBRARY_H

#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>

struct _pngpack_stats {
    double min;
    double max;
    double mean;
    double stddev;
    double sum;
    size_t count;
};

struct _pngpack_packed {
    uint16_t *data;
    double scale_factor;
    double add_offset;
    uint16_t nan;
    bool is_signed;
};

struct _pngpack_textfield {
    char *key;
    char *value;
    struct _pngpack_textfield *next;
};

struct _pngpack_textfield_container {
    struct _pngpack_textfield *first;
    size_t length;
};

struct pngpack_channel {
    char *name;
    double *data;
    size_t data_length;
    struct _pngpack_stats stats;
    struct _pngpack_packed packed;
    struct _pngpack_textfield_container *textfields;
};

struct pngpack_bounds {
    double min_w, max_w;
    double min_h, max_h;
};

struct pngpack {
    size_t width;
    size_t height;

    struct pngpack_bounds bounds;

    char *text_namespace;

    struct pngpack_channel *channels[4];
    size_t channels_length;
};


struct pngpack* pngpack_new(size_t width, size_t height, struct pngpack_bounds bounds, char *text_namespace);

void pngpack_free(struct pngpack *pp);

void pngpack_add_channel(struct pngpack *pp, struct pngpack_channel *channel);

/**
 * Write the resulting png file to the given path
 */
bool pngpack_write(struct pngpack *pp, char *path);

/**
 * This creates a channel structure and calculates statistics for the data passed.
 *
 * When this structure is passed to `pngpack_add_channel` pngpack becomes the owner
 * and will free it as part of `pngpack_free`. Otherwise, you may free it yourself.
 *
 * The data passed remains owned by the caller, and must remain allocated throughout
 * this structure's lifetime.
 */
struct pngpack_channel* pngpack_channel_new(char *name, double *data, size_t data_length);

void pngpack_channel_free(struct pngpack_channel *channel);

void pngpack_channel_add_textfield(struct pngpack_channel *channel, char *key, char *value);

#endif
