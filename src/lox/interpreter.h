#ifndef CLOX_INTERPRETER_H
#define CLOX_INTERPRETER_H

#include "../utils/list.h"
#include "environment.h"
#include "error.h"
#include "expr.h"
#include "value.h"

typedef struct {
  Environment env;
} Interpreter;

static inline Interpreter new_interpreter(Environment* enclosing_env) {
  return (Interpreter) { .env = new_env(enclosing_env) };
}

Value evaluate(Interpreter* interpreter, Expr* expr);
bool interpret(Interpreter* interpreter, List* stmts, RuntimeError* err);

static inline void free_interpreter(Interpreter interpreter) {
  free_env(&interpreter.env);
}

#endif