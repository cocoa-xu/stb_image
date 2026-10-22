#include <assert.h>
#include <stdlib.h>

static void *enif_alloc(size_t size) { return malloc(size); }
static void enif_free(void *ptr) { free(ptr); }
#include "stb_allocator_macros.h"

int main(void) {
    struct Context { int value; };
    struct Context *context = NULL;
    unsigned char *pixels = STBIR_MALLOC(16, context->value);
    assert(pixels != NULL);
    pixels[15] = 42;
    assert(pixels[15] == 42);
    STBIR_FREE(pixels, context->value);
    STBIR_FREE(NULL, context->value);
    return 0;
}
