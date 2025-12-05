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

Environment new_env() {
  return (Environment) { new_binary_search_tree((BstCmp)env_field_cmp, (BstDataFree)free_field) };
}

void env_define(Environment* env, StrSlice name, Value value) {
  Value* curr_value = env_assign(env, name, value);

  if (curr_value != NULL) {
    return;
  }

  // variable not present in env

  EnvField* new_field = xmalloc(sizeof(EnvField));
  new_field->name = name;
  new_field->value = value;

  bst_insert(&env->tree, new_field);
}

Value* env_assign(Environment* env, StrSlice name, Value value) {
  Value* curr_value = env_get(env, name);

  if (curr_value != NULL) {
    free_value(*curr_value);
    *curr_value = value;
  }

  return curr_value;
}

Value* env_get(Environment* env, StrSlice name) {
  EnvField field = { name, new_nil_value() };

  EnvField* field_found = (EnvField*)bst_search(env->tree, &field);

  if (field_found == NULL) {
    return NULL;
  }

  return &field_found->value;
}
