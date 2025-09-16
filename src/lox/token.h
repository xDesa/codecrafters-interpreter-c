#ifndef CLOX_TOKEN_H
#define CLOX_TOKEN_H

#include "value.h"

// clang-format off
typedef enum {
  // Single-character tokens.
  TOKEN_LEFT_PAREN, TOKEN_RIGHT_PAREN,
  TOKEN_LEFT_BRACE, TOKEN_RIGHT_BRACE,
  TOKEN_COMMA, TOKEN_DOT, TOKEN_MINUS, TOKEN_PLUS,
  TOKEN_SEMICOLON, TOKEN_SLASH, TOKEN_STAR,

  // One or two character tokens.
  TOKEN_BANG, TOKEN_BANG_EQUAL,
  TOKEN_EQUAL, TOKEN_EQUAL_EQUAL,
  TOKEN_GREATER, TOKEN_GREATER_EQUAL,
  TOKEN_LESS, TOKEN_LESS_EQUAL,

  // Literals.
  TOKEN_IDENTIFIER, TOKEN_STRING, TOKEN_NUMBER,

  // Keywords.
  TOKEN_AND, TOKEN_CLASS, TOKEN_ELSE, TOKEN_FALSE,
  TOKEN_FOR, TOKEN_FUN, TOKEN_IF, TOKEN_NIL, TOKEN_OR,
  TOKEN_PRINT, TOKEN_RETURN, TOKEN_SUPER, TOKEN_THIS,
  TOKEN_TRUE, TOKEN_VAR, TOKEN_WHILE,

  // Special
  TOKEN_ERROR, TOKEN_EOF
} TokenType;
// clang-format on

typedef struct {
  TokenType type;
  Value literal; // store error message in str literal value
  const char* lexeme;
  size_t length;
  size_t line;
} Token;

#ifndef ERROR_MSG_BUFFER_LENGTH
#define ERROR_MSG_BUFFER_LENGTH 1024
#endif

Value new_err_value(const char* message, ...) __attribute__((format(printf, 1, 2)));

Token* new_token(TokenType type, Value literal, const char* lexeme, size_t length, size_t line) __attribute__((malloc));

void free_token(Token* token);

#endif /* CLOX_TOKEN_H */