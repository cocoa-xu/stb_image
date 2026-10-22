#pragma once

#include "erl_nif.h"
#include <limits.h>
#include <stdbool.h>
#include <stdint.h>

static ERL_NIF_TERM error(ErlNifEnv *env, const char *msg)
{
  ERL_NIF_TERM atom = enif_make_atom(env, "error");
  ERL_NIF_TERM msg_term = enif_make_string(env, msg, ERL_NIF_LATIN1);
  return enif_make_tuple2(env, atom, msg_term);
}

static bool image_byte_size(int width, int height, int channels, int bytes_per_channel, size_t *size) {
    if (width <= 0 || height <= 0 || channels < 1 || channels > 4 ||
        (bytes_per_channel != 1 && bytes_per_channel != 4)) {
        return false;
    }

    // stb uses signed int sizes and strides internally. Check before multiplying.
    size_t result = (size_t)width;
    if (result > INT_MAX / (size_t)height) return false;
    result *= (size_t)height;
    if (result > INT_MAX / (size_t)channels) return false;
    result *= (size_t)channels;
    if (result > INT_MAX / (size_t)bytes_per_channel) return false;
    *size = result * (size_t)bytes_per_channel;
    return true;
}
