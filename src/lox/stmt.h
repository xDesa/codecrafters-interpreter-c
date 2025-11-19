#ifndef CLOX_STMT_H
#define CLOX_STMT_H

#include "expr.h"

typedef enum {
  STMT_EXPR,
  STMT_PRINT
} StmtType;

typedef struct {
  StmtType type;
} Stmt;

typedef struct {
  Stmt base;
  Expr* expr;
} ExprStmt;

typedef struct {
  Stmt base;
  Expr* expr;
} PrintStmt;

static inline ExprStmt* as_expr_stmt(Stmt* stmt) {
  return (ExprStmt*)stmt;
}

static inline PrintStmt* as_print_stmt(Stmt* stmt) {
  return (PrintStmt*)stmt;
}

Stmt* new_expr_stmt(Expr* expr);

Stmt* new_print_stmt(Expr* expr);

void free_stmt(Stmt* stmt);

#endif /* CLOX_STMT_H */