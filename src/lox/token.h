#ifndef CLOX_TOKEN_H
#define CLOX_TOKEN_H

#include "../utils/common.h"
#include "../utils/mem.h"

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

typedef union {
  void* nil;
  double num;
  char* str;
  bool boolean;
  char* error;
} TokenLiteral;

typedef struct {
  TokenType type;
  TokenLiteral literal;
  const char* lexeme;
  size_t length;
  size_t line;
} Token;

static inline TokenLiteral new_nil_literal() {
  return ((TokenLiteral) { .nil = NULL });
}

static inline TokenLiteral new_num_literal(double num) {
  return ((TokenLiteral) { .num = num });
}

static inline TokenLiteral new_str_literal(char* str, size_t len) {
  return ((TokenLiteral) { .str = xstrndup(str, len) });
}

static inline TokenLiteral new_bool_literal(bool boolean) {
  return ((TokenLiteral) { .boolean = boolean });
}

TokenLiteral new_err_literal(const char* message, ...);

Token* new_token(TokenType type, TokenLiteral literal, const char* lexeme, size_t length, size_t line);

void free_token(Token token);

#endif /* CLOX_TOKEN_H */