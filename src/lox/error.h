#ifndef CLOX_ERROR_H
#define CLOX_ERROR_H

#include "token.h"

typedef struct {
  Token* token;
  char* message;
} SyntaxError;

SyntaxError new_syntax_err(Token* token, const char* message, ...) __attribute__((format(printf, 2, 3)));

void free_syntax_err(SyntaxError err);

typedef struct {
  Token* token;
  char* message;
} RuntimeError;

RuntimeError new_runtime_err(Token* token, const char* message, ...) __attribute__((format(printf, 2, 3)));

void free_runtime_err(RuntimeError err);

#endif /* CLOX_ERROR_H */