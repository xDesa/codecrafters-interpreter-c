#include "token.h"

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

#include "../utils/mem.h"
#include "value.h"

Value new_err_value(const char* message, ...) {
  char buffer[ERROR_MSG_BUFFER_LENGTH];
  va_list args;

  va_start(args, message);

  size_t length = vsnprintf(buffer, sizeof(buffer), message, args);

  va_end(args);

  return new_str_value(buffer, length);
}

Token* new_token(TokenType type, Value literal, const char* lexeme, size_t length, size_t line) {
  Token* new_token = xmalloc(sizeof(Token));
  new_token->type = type;
  new_token->literal = literal;
  new_token->lexeme = lexeme;
  new_token->length = length;
  new_token->line = line;

  return new_token;
}

void free_token(Token* token) {
  free_value(token->literal);
  free(token);
}