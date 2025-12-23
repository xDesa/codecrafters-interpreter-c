#ifndef CLOX_PRINTERS_H
#define CLOX_PRINTERS_H

#include <stdio.h>
#include "../lox/error.h"
#include "../lox/expr.h"
#include "../lox/token.h"

void report_syntax_error(SyntaxError err);
void report_runtime_error(RuntimeError err);

void print_token(Token token);

void print_expr(Expr* expr);
static inline void println_expr(Expr* expr) {
  print_expr(expr);
  printf("\n");
}

void rpn_print_expr(Expr* expr);
static inline void rpn_println_expr(Expr* expr) {
  rpn_print_expr(expr);
  printf("\n");
}

#endif /* CLOX_PRINTERS_H */