#ifndef CLOX_PRINTERS_H
#define CLOX_PRINTERS_H

#include "../lox/expr.h"
#include "../lox/token.h"

#ifndef STRINGIFY_VALUE_NUM_BUFFER_SIZE
#define STRINGIFY_VALUE_NUM_BUFFER_SIZE 1024
#endif

#define INT_PRECISION 0
#define DOUBLE_PRECISION 6

void print_token(Token* token);

void print_expr(Expr* expr);

void rpn_print_expr(Expr* expr);

#endif /* CLOX_PRINTERS_H */