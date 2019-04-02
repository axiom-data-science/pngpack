#include <stdlib.h>
#include <stdio.h>

#include "../pngpack.h"

int main() {
    size_t width = 3;
    size_t height = 3;
    size_t len = width * height;

    struct pngpack_bounds bounds = {.min_w = -180, .max_w = 180, .min_h = -90, .max_h = 90};

    struct pngpack *pp = pngpack_new(width, height, bounds, "pp-example");

    double ch1_data[] = {70, 65, 68,
                         72, 85, 80,
                         85, 87, 75};
    struct pngpack_channel *ch1 = pngpack_channel_new("temps", ch1_data, len);
    pngpack_channel_add_textfield(ch1, "units", "degrees");

    pngpack_add_channel(pp, ch1);

    bool result = pngpack_write(pp, "example.png");
    printf("result: %d\n", result ? 1 : 0);

    pngpack_free(pp);
}
