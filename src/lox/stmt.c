#include "stmt.h"
#include <stdlib.h>
#include "../utils/mem.h"
#include "../utils/panic.h"

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
    case STMT_PRINT:
      // nothing to free
      break;
    default:
      unreachable_code();
      break;
  }

  free(stmt);
}