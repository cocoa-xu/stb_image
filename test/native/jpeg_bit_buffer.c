#include <assert.h>
#include <string.h>
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

struct Output { unsigned char data[16]; int size; };
static void capture(void *context, void *data, int size) {
    struct Output *output = context;
    assert(output->size + size <= (int)sizeof(output->data));
    memcpy(output->data + output->size, data, size);
    output->size += size;
}

int main(void) {
    struct Output output = {{0}, 0};
    stbi__write_context context = {0};
    unsigned int buffer = 0;
    int count = 0;
    const unsigned short bits[] = {65535, 16};
    const unsigned char expected[] = {255, 0, 255, 0, 255, 0, 255, 0};
    stbi__start_write_callbacks(&context, capture, &output);
    stbiw__jpg_writeBits(&context, &buffer, &count, bits);
    stbiw__jpg_writeBits(&context, &buffer, &count, bits);
    assert(count == 0);
    assert(output.size == (int)sizeof(expected));
    assert(memcmp(output.data, expected, sizeof(expected)) == 0);
    return 0;
}
