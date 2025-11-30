#ifndef CLOX_SLICE_H
#define CLOX_SLICE_H

#include <stdlib.h>

typedef struct {
  const char* str;
  size_t length;
} StrSlice;

static inline StrSlice new_str_slice(const char* str, const size_t length) {
  return (StrSlice) { str, length };
}

#endif /* CLOX_SLICE_H */