#include "token.h"

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

#include "../utils/mem.h"

LiteralValue new_err_literal(const char* message, ...) {
  char buffer[ERROR_MSG_BUFFER_LENGTH];
  va_list args;

  va_start(args, message);

  size_t length = vsnprintf(buffer, sizeof(buffer), message, args);

  va_end(args);

  return (LiteralValue) { LITERAL_ERR, { .error = xstrndup(buffer, length) } };
}

void free_literal(LiteralValue literal) {
  switch (literal.type) {
    case LITERAL_STR:
      free(literal.data.str);
      break;
    case LITERAL_ERR:
      free(literal.data.error);
      break;
    default:
      // no other memory to free
      break;
  }
}