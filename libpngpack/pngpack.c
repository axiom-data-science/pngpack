#include "pngpack.h"

#include <string.h>
#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <png.h>
#include <float.h>


#define PNGPACK_BITS 16

struct _pngpack_stats pngpack_stats_calculate(const double *data, size_t data_length);
void pngpack_channel_pack(struct pngpack_channel *channel);
struct png_text_struct pngpack_png_text_struct_new(char *key, char *text);
void pngpack_png_text_struct_free(struct png_text_struct *metadata);


struct pngpack* pngpack_new(size_t width, size_t height, struct pngpack_bounds bounds, char *text_namespace) {
    struct pngpack *pp = malloc(sizeof(struct pngpack));
    pp->width = width;
    pp->height = height;
    pp->bounds = bounds;
    pp->text_namespace = strdup(text_namespace);
    pp->channels[0] = NULL;
    pp->channels[1] = NULL;
    pp->channels[2] = NULL;
    pp->channels[3] = NULL;
    pp->channels_length = 0;
    return pp;
}

void pngpack_free(struct pngpack *pp) {
    for (size_t i = 0; i < pp->channels_length; i += 1) {
        pngpack_channel_free(pp->channels[i]);
    }
    free(pp->text_namespace);
    free(pp);
}

void pngpack_add_channel(struct pngpack *pp, struct pngpack_channel *channel) {
    assert(pp->channels_length < 4);
    pp->channels[pp->channels_length++] = channel;
}

bool pngpack_write(struct pngpack *pp, char *path) {
    FILE *fp = fopen(path, "wb");
    if (!fp) return false;

    png_struct *png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if (!png_ptr) { goto error; }
    png_info *info_ptr = png_create_info_struct(png_ptr);
    if (!info_ptr) { goto error; }

    if (setjmp(png_jmpbuf(png_ptr))) {
        goto error;
    }


    png_init_io(png_ptr, fp);

    int color_type;
    if (pp->channels_length == 1) {
        color_type = PNG_COLOR_TYPE_GRAY;
    } else if (pp->channels_length == 2) {
        color_type = PNG_COLOR_TYPE_GRAY_ALPHA;
    } else if (pp->channels_length == 3) {
        color_type = PNG_COLOR_TYPE_RGB;
    } else if (pp->channels_length == 4) {
        color_type = PNG_COLOR_TYPE_RGB_ALPHA;
    } else {
        goto error;
    }

    png_set_IHDR(png_ptr, info_ptr,
            pp->width, pp->height,
            PNGPACK_BITS, color_type,
            PNG_INTERLACE_NONE,
            PNG_COMPRESSION_TYPE_DEFAULT,
            PNG_FILTER_TYPE_DEFAULT);

    /*
     * text metadata
     */

    /**
     * 4 fields for bounds
     * 8 fields per channel for name, min, max, stddev, mean, scale, offset, signed
     */
    size_t metadata_fields_length = 4;
    metadata_fields_length += 7 * pp->channels_length;

    struct png_text_struct *metadata_fields = malloc(metadata_fields_length * sizeof(struct png_text_struct));

    char *metadata_key = NULL;
    char *metadata_text = NULL;
    size_t metadata_i = 0;

    /**
     * global bounds
     */

    asprintf(&metadata_key, "%s:w:min", pp->text_namespace);
    asprintf(&metadata_text, "%.*f", DBL_DECIMAL_DIG, pp->bounds.min_w);
    metadata_fields[metadata_i++] = pngpack_png_text_struct_new(metadata_key, metadata_text);
    free(metadata_text), metadata_text = NULL;
    free(metadata_key), metadata_key = NULL;

    asprintf(&metadata_key, "%s:w:max", pp->text_namespace);
    asprintf(&metadata_text, "%.*f", DBL_DECIMAL_DIG, pp->bounds.max_w);
    metadata_fields[metadata_i++] = pngpack_png_text_struct_new(metadata_key, metadata_text);
    free(metadata_text), metadata_text = NULL;
    free(metadata_key), metadata_key = NULL;

    asprintf(&metadata_key, "%s:h:min", pp->text_namespace);
    asprintf(&metadata_text, "%.*f", DBL_DECIMAL_DIG, pp->bounds.min_h);
    metadata_fields[metadata_i++] = pngpack_png_text_struct_new(metadata_key, metadata_text);
    free(metadata_text), metadata_text = NULL;
    free(metadata_key), metadata_key = NULL;

    asprintf(&metadata_key, "%s:h:max", pp->text_namespace);
    asprintf(&metadata_text, "%.*f", DBL_DECIMAL_DIG, pp->bounds.max_h);
    metadata_fields[metadata_i++] = pngpack_png_text_struct_new(metadata_key, metadata_text);
    free(metadata_text), metadata_text = NULL;
    free(metadata_key), metadata_key = NULL;

    /**
     * channel attributes
     */

    for (size_t c = 0; c < pp->channels_length; c += 1) {
        asprintf(&metadata_key, "%s:channel:%lu:name", pp->text_namespace, c);
        asprintf(&metadata_text, "%s", pp->channels[c]->name);
        metadata_fields[metadata_i++] = pngpack_png_text_struct_new(metadata_key, metadata_text);
        free(metadata_text), metadata_text = NULL;
        free(metadata_key), metadata_key = NULL;

        asprintf(&metadata_key, "%s:channel:%lu:pack:scale", pp->text_namespace, c);
        asprintf(&metadata_text, "%.*f", DBL_DECIMAL_DIG, pp->channels[c]->packed.scale_factor);
        metadata_fields[metadata_i++] = pngpack_png_text_struct_new(metadata_key, metadata_text);
        free(metadata_text), metadata_text = NULL;
        free(metadata_key), metadata_key = NULL;

        asprintf(&metadata_key, "%s:channel:%lu:pack:offset", pp->text_namespace, c);
        asprintf(&metadata_text, "%.*f", DBL_DECIMAL_DIG, pp->channels[c]->packed.add_offset);
        metadata_fields[metadata_i++] = pngpack_png_text_struct_new(metadata_key, metadata_text);
        free(metadata_text), metadata_text = NULL;
        free(metadata_key), metadata_key = NULL;

        asprintf(&metadata_key, "%s:channel:%lu:pack:signed", pp->text_namespace, c);
        asprintf(&metadata_text, "%d", pp->channels[c]->packed.is_signed ? 1 : 0);
        metadata_fields[metadata_i++] = pngpack_png_text_struct_new(metadata_key, metadata_text);
        free(metadata_text), metadata_text = NULL;
        free(metadata_key), metadata_key = NULL;

        asprintf(&metadata_key, "%s:channel:%lu:stats:min", pp->text_namespace, c);
        asprintf(&metadata_text, "%.*f", DBL_DECIMAL_DIG, pp->channels[c]->stats.min);
        metadata_fields[metadata_i++] = pngpack_png_text_struct_new(metadata_key, metadata_text);
        free(metadata_text), metadata_text = NULL;
        free(metadata_key), metadata_key = NULL;

        asprintf(&metadata_key, "%s:channel:%lu:stats:max", pp->text_namespace, c);
        asprintf(&metadata_text, "%.*f", DBL_DECIMAL_DIG, pp->channels[c]->stats.max);
        metadata_fields[metadata_i++] = pngpack_png_text_struct_new(metadata_key, metadata_text);
        free(metadata_text), metadata_text = NULL;
        free(metadata_key), metadata_key = NULL;

        asprintf(&metadata_key, "%s:channel:%lu:stats:stddev", pp->text_namespace, c);
        asprintf(&metadata_text, "%.*f", DBL_DECIMAL_DIG, pp->channels[c]->stats.stddev);
        metadata_fields[metadata_i++] = pngpack_png_text_struct_new(metadata_key, metadata_text);
        free(metadata_text), metadata_text = NULL;
        free(metadata_key), metadata_key = NULL;

        asprintf(&metadata_key, "%s:channel:%lu:stats:mean", pp->text_namespace, c);
        asprintf(&metadata_text, "%.*f", DBL_DECIMAL_DIG, pp->channels[c]->stats.mean);
        metadata_fields[metadata_i++] = pngpack_png_text_struct_new(metadata_key, metadata_text);
        free(metadata_text), metadata_text = NULL;
        free(metadata_key), metadata_key = NULL;
    }

    png_set_text(png_ptr, info_ptr, metadata_fields, (int)metadata_fields_length);


    png_write_info(png_ptr, info_ptr);

    /*
     * image data
     */

    size_t pixel_count = pp->width * pp->height;
    uint16_t *data = malloc(pixel_count * pp->channels_length * sizeof(uint16_t));

    // combine the channels
    size_t subpixel_i = 0;
    for (size_t i = 0; i < pixel_count; i += 1) {
        for (size_t c = 0; c < pp->channels_length; c += 1) {
            data[subpixel_i] = htons(pp->channels[c]->packed.data[i]);
            subpixel_i += 1;
        }
    }

    png_byte *data_bytes = (png_byte*)data;
    for (size_t r = 0; r < pp->height; r += 1) {
        png_write_row(png_ptr, data_bytes + (pp->width * pp->channels_length * (PNGPACK_BITS / 8) * r));
    }

    /*
     * cleanup
     */

    png_write_end(png_ptr, info_ptr);

    png_destroy_write_struct(&png_ptr, &info_ptr);
    fclose(fp);

    free(data);

    for (size_t i = 0; i < metadata_fields_length; i += 1) {
        pngpack_png_text_struct_free(&metadata_fields[i]);
    }
    free(metadata_fields);

    return true;

error:
    if (png_ptr) {
        png_destroy_write_struct(&png_ptr, &info_ptr);
    }
    fclose(fp);
    return false;
}

struct pngpack_channel* pngpack_channel_new(char *name, double *data, size_t data_length) {
    struct pngpack_channel *channel = malloc(sizeof(struct pngpack_channel));
    channel->name = strdup(name);
    channel->data = data;
    channel->data_length = data_length;
    channel->stats = pngpack_stats_calculate(data, data_length);
    pngpack_channel_pack(channel);
    return channel;
}

void pngpack_channel_pack(struct pngpack_channel *channel) {
    struct _pngpack_packed *packed = &channel->packed;
    packed->data = malloc(channel->data_length * sizeof(uint16_t));

    packed->scale_factor = (channel->stats.max - channel->stats.min) / ((1 << PNGPACK_BITS) - 2);

    if (channel->stats.min < 0.0) {
        packed->is_signed = true;
        // reserve the lowest signed value for missing/nan
        packed->nan = (uint16_t)(-(1 << (PNGPACK_BITS - 1)));
        packed->add_offset = (channel->stats.max + channel->stats.min) / 2.0;
    } else {
        packed->is_signed = false;
        // reserve 0 for missing/nan values
        packed->nan = 0;
        packed->add_offset = channel->stats.min - packed->scale_factor;
    }

    for (size_t i = 0; i < channel->data_length; i += 1) {
        packed->data[i] = (uint16_t)lround((channel->data[i] - packed->add_offset) / packed->scale_factor);
    }
}

void pngpack_channel_free(struct pngpack_channel *channel) {
    free(channel->packed.data);
    free(channel->name);
    free(channel);
}

struct _pngpack_stats pngpack_stats_calculate(const double *data, size_t data_length) {
    struct _pngpack_stats stats;

    stats.count = data_length;

    stats.min = data[0];
    stats.max = data[0];
    stats.sum = data[0];

    for (size_t i = 1; i < data_length; i += 1) {
        stats.sum += data[i];

        if (data[i] < stats.min) {
            stats.min = data[i];
        }
        if (data[i] > stats.max) {
            stats.max = data[i];
        }
    }

    stats.mean = stats.sum / (double)stats.count;

    double diff_sq_sum = 0.0;
    for (size_t i = 0; i < data_length; i += 1) {
        double diff = data[i] - stats.mean;
        diff_sq_sum += diff * diff;
    }

    stats.stddev = sqrt(diff_sq_sum / (double)stats.count);

    return stats;
}

struct png_text_struct pngpack_png_text_struct_new(char *key, char *text) {
    struct png_text_struct metadata;
    metadata.compression = PNG_TEXT_COMPRESSION_NONE;
    metadata.key = strdup(key);
    metadata.text = strdup(text);
    metadata.text_length = strlen(text);
    // unused:
    metadata.itxt_length = 0;
    metadata.lang = NULL;
    metadata.lang_key = NULL;
    return metadata;
}

void pngpack_png_text_struct_free(struct png_text_struct *metadata) {
    free(metadata->key);
    free(metadata->text);
}
