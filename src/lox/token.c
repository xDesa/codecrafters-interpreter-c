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

Token* new_token(TokenType type, LiteralValue literal, const char* lexeme, size_t length, size_t line) {
  Token* new_token = xmalloc(sizeof(Token));
  new_token->type = type;
  new_token->literal = literal;
  new_token->lexeme = new_str_slice(lexeme, length);
  new_token->line = line;

  return new_token;
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

void free_token(Token* token) {
  if (token == NULL) {
    return;
  }

  free_literal(token->literal);
  free(token);
}