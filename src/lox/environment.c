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
  EnvField* field = xmalloc(sizeof(EnvField));
  field->name = name;
  field->value = value;

  bst_insert(&env->tree, field);
}

Value* env_get(Environment* env, StrSlice name) {
  EnvField field = { name, new_nil_value() };

  EnvField* field_found = (EnvField*)bst_search(env->tree, &field);

  if (field_found == NULL) {
    return NULL;
  }

  return &field_found->value;
}
