#ifndef CLOX_ENVIRONMENT_H
#define CLOX_ENVIRONMENT_H

#include "../utils/bst.h"
#include "value.h"

typedef struct {
  BinarySearchTree tree;
} Environment;

typedef struct {
  StrSlice name;
  Value value;
} EnvField;

Environment new_env();

void env_define(Environment* env, StrSlice name, Value value);

Value* env_get(Environment* env, StrSlice name);

static inline void free_env(Environment* env) {
  free_bst(&env->tree);
}

#endif /* CLOX_ENVIRONMENT_H */