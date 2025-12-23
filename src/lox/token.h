#ifndef CLOX_TOKEN_H
#define CLOX_TOKEN_H

#include "../utils/mem.h"
#include "../utils/slice.h"
#include "../utils/vec.h"

// clang-format off
typedef enum {
  // Single-character tokens.
  TOKEN_LEFT_PAREN, TOKEN_RIGHT_PAREN,
  TOKEN_LEFT_BRACE, TOKEN_RIGHT_BRACE,
  TOKEN_COMMA, TOKEN_DOT, TOKEN_MINUS, TOKEN_PLUS,
  TOKEN_SEMICOLON, TOKEN_SLASH, TOKEN_STAR,
  TOKEN_QUESTION_MARK, TOKEN_COLON,

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

typedef enum {
  LITERAL_NIL,
  LITERAL_NUM,
  LITERAL_BOOL,
  LITERAL_STR,
  LITERAL_ERR,
} LiteralValueType;

typedef struct {
  LiteralValueType type;
  union {
    void* nil;
    double num;
    bool boolean;
    char* str;
    char* error;
  } data;
} LiteralValue;

typedef struct {
  TokenType type;
  LiteralValue literal;
  StrSlice lexeme;
  size_t line;
} Token;

typedef VecType(Token) TokenVec;

#define free_token_vec(vec) \
  free_vec_with(vec, Token, free_token)

#ifndef ERROR_MSG_BUFFER_LENGTH
#define ERROR_MSG_BUFFER_LENGTH 1024
#endif

static inline LiteralValue new_nil_literal() {
  return (LiteralValue) { LITERAL_NIL, { .nil = NULL } };
}

static inline LiteralValue new_num_literal(double num) {
  return (LiteralValue) { LITERAL_NUM, { .num = num } };
}

static inline LiteralValue new_bool_literal(bool boolean) {
  return (LiteralValue) { LITERAL_BOOL, { .boolean = boolean } };
}

static inline LiteralValue new_str_literal(char* str, size_t length) {
  return (LiteralValue) { LITERAL_STR, { .str = xstrndup(str, length) } };
}

LiteralValue new_err_literal(const char* message, ...) __attribute__((format(printf, 1, 2)));

static inline bool is_literal_token(Token token) {
  return token.type == TOKEN_NIL
      || token.type == TOKEN_NUMBER
      || token.type == TOKEN_TRUE || token.type == TOKEN_FALSE
      || token.type == TOKEN_STRING;
}

static inline Token new_token(TokenType type, LiteralValue literal, const char* lexeme, size_t length, size_t line) {
  return (Token) { type, literal, new_str_slice(lexeme, length), line };
}

void free_literal(LiteralValue literal);

static inline void free_token(Token token) {
  free_literal(token.literal);
}

#endif /* CLOX_TOKEN_H */