#ifndef CLOX_VALUE_H
#define CLOX_VALUE_H

#include "../utils/common.h"
#include "../utils/mem.h"

#ifndef STRINGIFY_NUM_VALUE_BUFFER_SIZE
#define STRINGIFY_NUM_VALUE_BUFFER_SIZE 1024
#endif

typedef enum {
  VALUE_NIL,
  VALUE_NUMBER,
  VALUE_STRING,
  VALUE_BOOL,
} ValueType;

typedef struct {
  ValueType type;
  union {
    void* nil;
    double num;
    bool boolean;
    char* str;
  } data;
} Value;

static inline Value new_nil_value() {
  return ((Value) { VALUE_NIL, { .nil = NULL } });
}

static inline Value new_num_value(double num) {
  return ((Value) { VALUE_NUMBER, { .num = num } });
}

static inline Value new_bool_value(bool boolean) {
  return ((Value) { VALUE_BOOL, { .boolean = boolean } });
}

static inline Value new_str_value(const char* str) {
  return ((Value) { VALUE_STRING, { .str = xstrdup(str) } });
}

static inline void* as_nil_value(Value value) { return (value.data.nil); }

static inline double as_num_value(Value value) { return (value.data.num); }

static inline bool as_bool_value(Value value) { return (value.data.boolean); }

static inline char* as_str_value(Value value) { return (value.data.str); }

static inline bool is_value_type(Value value, ValueType type) { return value.type == type; }

void free_value(Value value);

bool are_values_eq(Value left, Value right);

Value clone_value(Value value);

const char* stringify_num_value(double num);

#endif /* CLOX_VALUE_H */