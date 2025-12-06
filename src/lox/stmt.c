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

Stmt* new_var_decl_stmt(Token* name, Expr* initializer) {
  VarDeclStmt* stmt = xmalloc(sizeof(VarDeclStmt));
  stmt->base.type = STMT_VAR_DECL;
  stmt->name = name;
  stmt->initializer = initializer;

  return (Stmt*)stmt;
}

Stmt* new_block_stmt(List stmts) {
  BlockStmt* stmt = xmalloc(sizeof(BlockStmt));
  stmt->base.type = STMT_BLOCK;
  stmt->stmts = stmts;

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
    case STMT_VAR_DECL:
      free_expr(as_var_decl_stmt(stmt)->initializer);
      break;
    case STMT_BLOCK:
      free_list(&as_block_stmt(stmt)->stmts, (Iterator)free_stmt);
      break;
    default:
      unreachable_code();
      break;
  }

  free(stmt);
}