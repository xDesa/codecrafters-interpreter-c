#include "token.h"

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

#include "../utils/error.h"
#include "../utils/mem.h"

TokenLiteral new_err_literal(const char* message, ...) {

  va_list args;
  va_start(args, message);

  char* msg = vcreate_error_msg(message, args);

  va_end(args);

  return (TokenLiteral) { .error = msg };
}

Token* new_token(TokenType type, TokenLiteral literal, const char* lexeme, size_t length, size_t line) {
  Token* new_token = xmalloc(sizeof(Token));
  new_token->type = type;
  new_token->literal = literal;
  new_token->lexeme = lexeme;
  new_token->length = length;
  new_token->line = line;

  return new_token;
}

void free_token(Token* token) {
  switch (token->type) {
    case TOKEN_STRING:
      free(token->literal.str);
      break;
    case TOKEN_ERROR:
      free(token->literal.error);
      break;
    default:
      // no other memory to free
      break;
  }

  free(token);
}