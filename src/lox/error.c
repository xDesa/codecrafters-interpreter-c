#include "error.h"
#include <stdio.h>
#include <stdlib.h>

SyntaxError new_syntax_err(Token* token, const char* message, ...) {
  SyntaxError err = { token, NULL };

  if (message == NULL) {
    return err;
  }

  char buffer[ERROR_MSG_BUFFER_LENGTH];
  va_list args;

  va_start(args, message);

  size_t length = vsnprintf(buffer, sizeof(buffer), message, args);

  va_end(args);

  err.message = xstrndup(buffer, length);

  return err;
}

void free_syntax_err(SyntaxError err) {
  if (err.message != NULL) {
    free(err.message);
  }
}

RuntimeError new_runtime_err(Token* token, const char* message, ...) {
  if (message == NULL) {
    return (RuntimeError) { token, NULL };
  }

  char buffer[ERROR_MSG_BUFFER_LENGTH];
  va_list args;

  va_start(args, message);

  size_t length = vsnprintf(buffer, sizeof(buffer), message, args);

  va_end(args);

  return (RuntimeError) { token, xstrndup(buffer, length) };
}

void free_runtime_err(RuntimeError err) {
  if (err.message != NULL) {
    free(err.message);
  }
}