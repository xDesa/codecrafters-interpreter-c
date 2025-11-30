#ifndef CLOX_INTERPRETER_H
#define CLOX_INTERPRETER_H

#include "../utils/list.h"
#include "environment.h"
#include "error.h"
#include "expr.h"
#include "value.h"

Value evaluate(Environment* env, Expr* expr);
bool interpret(Environment* env, List* stmts, RuntimeError* err);

#endif