#include "stmt.h"
#include <stdlib.h>
#include "../utils/mem.h"
#include "../utils/panic.h"
#include "expr.h"

Stmt* new_expr_stmt(Expr* expr) {
  PrintStmt* stmt = xmalloc(sizeof(ExprStmt));
  stmt->base.type = STMT_EXPR;
  stmt->expr = expr;

  return (Stmt*)stmt;
}

Stmt* new_print_stmt(Expr* expr) {
  PrintStmt* stmt = xmalloc(sizeof(PrintStmt));
  stmt->base.type = STMT_PRINT;
  stmt->expr = expr;

  return (Stmt*)stmt;
}

void free_stmt(Stmt* stmt) {
  switch (stmt->type) {
    case STMT_EXPR:
      free_expr(as_expr_stmt(stmt)->expr);
      break;
    case STMT_PRINT:
      free_expr(as_print_stmt(stmt)->expr);
      break;
    default:
      unreachable_code();
      break;
  }

  free(stmt);
}