#include "environment.h"
#include <string.h>
#include "value.h"

int env_field_cmp(EnvField* left, EnvField* right) {
  return strncmp(left->name.str, right->name.str, left->name.length);
}

void free_field(EnvField* field) {
  free_value(field->value);
  free(field);
}

Environment new_env(Environment* enclosing) {
  return (Environment) {
    new_binary_search_tree((BstCmp)env_field_cmp, (BstDataFree)free_field),
    enclosing
  };
}

void env_replace_value(Value* curr_value, Value value);
static Value* env_get_without_enclosing(Environment* env, StrSlice name);

void env_define(Environment* env, StrSlice name, Value value) {
  Value* curr_value = env_get_without_enclosing(env, name);

  if (curr_value != NULL) {
    env_replace_value(curr_value, value);
    return;
  }

  // variable not present in curr env

  EnvField* new_field
      = xmalloc(sizeof(EnvField));
  new_field->name = name;
  new_field->value = value;

  bst_insert(&env->tree, new_field);
}

void env_replace_value(Value* curr_value, Value value) {
  if (curr_value != NULL) {
    free_value(*curr_value);
    *curr_value = value;
  }
}

Value* env_assign(Environment* env, StrSlice name, Value value) {
  Value* curr_value = env_get(env, name);

  env_replace_value(curr_value, value);

  return curr_value;
}

Value* env_get_without_enclosing(Environment* env, StrSlice name) {
  EnvField field = { name, new_nil_value() };

  EnvField* field_found = (EnvField*)bst_search(env->tree, &field);

  if (field_found == NULL) {
    return NULL;
  }

  return &field_found->value;
}

Value* env_get(Environment* env, StrSlice name) {
  Value* value = env_get_without_enclosing(env, name);

  if (value == NULL) {
    return env->enclosing != NULL ? env_get(env->enclosing, name) : NULL;
  }

  return value;
}
