#ifndef CLOX_INTERPRETER_H
#define CLOX_INTERPRETER_H

#include "error.h"
#include "expr.h"
#include "value.h"

typedef enum {
  INTERPRETER_OK,
  INTERPRETER_RUNTIME_ERROR,
} InterpreterResult;

InterpreterResult evaluate(Expr* expr, Value* output, RuntimeError* err);

#endif