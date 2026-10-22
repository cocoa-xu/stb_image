#include <assert.h>
#include <limits.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

typedef uintptr_t ERL_NIF_TERM;
typedef struct { int unused; } ErlNifEnv;
typedef struct { size_t size; unsigned char *data; } ErlNifBinary;
static int live, calls, fail_at;
static int image_width = 2, image_height = 2, image_frames = 1;
static void *tracked_alloc(size_t size) {
    void *p = malloc(size);
    assert(p != NULL);
    ++live;
    return p;
}
static void *enif_alloc(size_t size) {
    if (++calls == fail_at) return NULL;
    return tracked_alloc(size);
}
static void enif_free(void *p) { if (p) { --live; free(p); } }
#define STBI_FREE enif_free
static ERL_NIF_TERM error(ErlNifEnv *env, const char *message) { return 1; }
static ERL_NIF_TERM enif_make_badarg(ErlNifEnv *env) { return 2; }
static int enif_inspect_binary(ErlNifEnv *env, ERL_NIF_TERM value, ErlNifBinary *binary) {
    static unsigned char data[1];
    binary->data = data;
    binary->size = 1;
    return 1;
}
static unsigned char *stbi_load_gif_from_memory(const unsigned char *input, int size,
        int **delays, int *x, int *y, int *z, int *comp, int requested) {
    *x = image_width; *y = image_height; *z = image_frames; *comp = 4;
    *delays = tracked_alloc(sizeof(int));
    return tracked_alloc(16);
}
static int enif_alloc_binary(size_t size, ErlNifBinary *binary) { assert(fail_at == 0); return 0; }
static ERL_NIF_TERM enif_make_binary(ErlNifEnv *env, ErlNifBinary *binary) { abort(); }
static ERL_NIF_TERM enif_make_int(ErlNifEnv *env, int value) { abort(); }
static ERL_NIF_TERM enif_make_list_from_array(ErlNifEnv *env, const ERL_NIF_TERM *terms, unsigned count) { abort(); }
static ERL_NIF_TERM enif_make_atom(ErlNifEnv *env, const char *name) { abort(); }
static ERL_NIF_TERM enif_make_tuple3(ErlNifEnv *env, ERL_NIF_TERM a, ERL_NIF_TERM b, ERL_NIF_TERM c) { abort(); }
static ERL_NIF_TERM enif_make_tuple4(ErlNifEnv *env, ERL_NIF_TERM a, ERL_NIF_TERM b, ERL_NIF_TERM c, ERL_NIF_TERM d) { abort(); }
#include "gif_nif_function.h"

int main(void) {
    ErlNifEnv env = {0};
    ERL_NIF_TERM input = 0;
    // Fail each metadata allocation, then the frame binary allocation.
    for (fail_at = 0; fail_at <= 3; ++fail_at) {
        calls = 0;
        assert(read_gif_binary(&env, 1, &input) == 1);
        assert(live == 0);
    }
    image_frames = 0;
    assert(read_gif_binary(&env, 1, &input) == 1);
    assert(live == 0);
    image_frames = 1;
    image_width = INT_MAX;
    assert(read_gif_binary(&env, 1, &input) == 1);
    assert(live == 0);
    return 0;
}
