#ifndef LOX_VEC
#define LOX_VEC

/*
  inspiration for vec implementation in C with "dynamic types"
  from https://github.com/tsoding/nob.h/blob/7deb15dcdbcb113794b79c60aabea6bada50aa93/nob.h#L300
*/

#include <assert.h>
#include <stddef.h>

#ifndef VEC_INIT_CAPACITY
#define VEC_INIT_CAPACITY 30
#endif /* VEC_INIT_CAPACITY */

#define VecType(Type) \
  struct {            \
    Type* items;      \
    size_t len;       \
    size_t capacity;  \
  }

#define vec_reserve(vec, expected_capacity)                                                                    \
  do {                                                                                                         \
    if ((expected_capacity) > (vec)->capacity) {                                                               \
      if ((vec)->capacity == 0) {                                                                              \
        (vec)->capacity = VEC_INIT_CAPACITY;                                                                   \
      }                                                                                                        \
      (vec)->capacity = (expected_capacity) > (vec)->capacity * 2 ? (expected_capacity) : (vec)->capacity * 2; \
      (vec)->items = xrealloc((vec)->items, (vec)->capacity * sizeof(*(vec)->items));                          \
    }                                                                                                          \
  } while (0)

#define vec_push(vec, item)              \
  do {                                   \
    vec_reserve(vec, (vec)->len + 1);    \
    (vec)->items[(vec)->len++] = (item); \
  } while (0);

#define vec_for_each(vec, Type, iter) \
  for (Type* iter = (vec)->items; iter < (vec)->items + (vec)->len; iter++)

#define vec_last(vec) \
  (vec)->items[(assert((vec)->len > 0), (vec)->len - 1)]

#define free_vec(vec)    \
  do {                   \
    free((vec)->items);  \
    (vec)->len = 0;      \
    (vec)->capacity = 0; \
  } while (0);

#define free_vec_with(vec, Type, free_fn)             \
  do {                                                \
    vec_for_each(vec, Type, iter) { free_fn(*iter); } \
    free_vec(vec);                                    \
  } while (0);

#endif /* LOX_VEC */