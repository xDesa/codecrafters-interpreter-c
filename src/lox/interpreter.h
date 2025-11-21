#ifndef CLOX_INTERPRETER_H
#define CLOX_INTERPRETER_H

#include "../utils/list.h"
#include "error.h"
#include "expr.h"
#include "value.h"

Value evaluate(Expr* expr);
bool interpret(List* stmts, RuntimeError* err);

#endif