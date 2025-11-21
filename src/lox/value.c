#include "value.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../utils/panic.h"
#include "error.h"

void free_value(Value value) {
  switch (value.type) {
    case VALUE_STRING:
      free(value.data.str);
      break;
    case VALUE_ERR:
      free_runtime_err(as_err_value(value));
      break;
    case VALUE_NUMBER:
    case VALUE_BOOL:
    case VALUE_NIL:
      // nothing to free
      break;
    default:
      unreachable_code();
  }
}

bool are_values_eq(Value left, Value right) {
  if (left.type != right.type) {
    return false;
  }

  switch (left.type) {
    case VALUE_NIL:
      return true;
    case VALUE_NUMBER:
      return as_num_value(left) == as_num_value(right);
    case VALUE_BOOL:
      return as_bool_value(left) == as_bool_value(right);
    case VALUE_STRING:
      return strcmp(as_str_value(left), as_str_value(right)) == 0;
    case VALUE_ERR:
      return are_runtime_errs_eq(as_err_value(left), as_err_value(right));
    default:
      unreachable_code();
  }
}

Value clone_value(Value value) {
  switch (value.type) {
    case VALUE_NIL:
      return new_nil_value();
    case VALUE_NUMBER:
      return new_num_value(as_num_value(value));
    case VALUE_BOOL:
      return new_bool_value(as_bool_value(value));
    case VALUE_STRING:
      return new_str_value(as_str_value(value));
    case VALUE_ERR:
      return new_err_value(as_err_value(value));
    default:
      unreachable_code();
  }
}

const char* stringify_num_value(double num) {
  static char str[STRINGIFY_NUM_VALUE_BUFFER_SIZE];

  memset(str, 0, sizeof(str));

  if (num == (int)num) {
    snprintf(str, sizeof(str), "%d", (int)num);
    return str;
  }

  int printed_chars = snprintf(str, sizeof(str), "%lf", num);

  char* dot_ptr = strchr(str, '.');
  assert(dot_ptr != NULL);

  char* end_ptr = str + printed_chars - 1;

  while (end_ptr > dot_ptr + 1 && *end_ptr == '0') {
    *end_ptr = '\0';
    end_ptr--;
  }

  return str;
}

void print_value(Value value) {
  switch (value.type) {
    case VALUE_NIL:
      printf("nil");
      break;
    case VALUE_NUMBER:
      printf("%g", as_num_value(value));
      break;
    case VALUE_BOOL:
      printf("%s", as_bool_value(value) ? "true" : "false");
      break;
    case VALUE_STRING:
      printf("%s", as_str_value(value));
      break;
    default:
      unreachable_code();
      break;
  }
  printf("\n");
}