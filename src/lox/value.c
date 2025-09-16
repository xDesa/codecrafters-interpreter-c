#include "value.h"
#include <stdlib.h>

void free_value(Value value) {
  if (value.type == VALUE_STRING) {
    free(value.data.str);
  }
}