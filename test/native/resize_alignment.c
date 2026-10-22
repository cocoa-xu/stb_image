#include <assert.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>
// Isolate alignment behavior from allocation-context cleanup.
#define STBIR_MALLOC(size, context) malloc(size)
#define STBIR_FREE(ptr, context) free(ptr)
#define STB_IMAGE_RESIZE_IMPLEMENTATION
#include "stb_image_resize2.h"

int main(void) {
    unsigned char input[4], output[1024];
    float input_f[4], output_f[1024];
    const int sizes[][2] = {{3, 2}, {6, 4}, {13, 7}};
    memset(input, 128, sizeof(input));
    for (int i = 0; i < 4; ++i) input_f[i] = 0.5f;
    for (int channels = 1; channels <= 4; ++channels) {
        for (int s = 0; s < 3; ++s) {
            int width = sizes[s][0], height = sizes[s][1];
            assert(stbir_resize_uint8_linear(input, 1, 1, 0, output, width, height, 0,
                                             (stbir_pixel_layout)channels) == output);
            assert(stbir_resize_float_linear(input_f, 1, 1, 0, output_f, width, height, 0,
                                             (stbir_pixel_layout)channels) == output_f);
            for (int i = 0; i < width * height * channels; ++i) {
                assert(output[i] == 128);
                assert(fabsf(output_f[i] - 0.5f) < 0.00001f);
            }
        }
    }
    return 0;
}
