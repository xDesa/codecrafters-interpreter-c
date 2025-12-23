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

Stmt* new_block_stmt(StmtVec stmts) {
  BlockStmt* stmt = xmalloc(sizeof(BlockStmt));
  stmt->base.type = STMT_BLOCK;
  stmt->stmts = stmts;

  return (Stmt*)stmt;
}

Stmt* new_if_stmt(Expr* condition, Stmt* then_branch, Stmt* else_branch) {
  IfStmt* stmt = xmalloc(sizeof(IfStmt));
  stmt->base.type = STMT_IF;
  stmt->condition = condition;
  stmt->then_branch = then_branch;
  stmt->else_branch = else_branch;

  return (Stmt*)stmt;
}

Stmt* new_while_stmt(Expr* condition, Stmt* body) {
  WhileStmt* stmt = xmalloc(sizeof(WhileStmt));
  stmt->base.type = STMT_WHILE;
  stmt->condition = condition;
  stmt->body = body;

  return (Stmt*)stmt;
}

void free_stmt(Stmt* stmt) {
  if (stmt == NULL) {
    return;
  }

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
      free_stmt_vec(&as_block_stmt(stmt)->stmts);
      break;
    case STMT_IF:
      IfStmt* if_stmt = as_if_stmt(stmt);
      free_expr(if_stmt->condition);
      free_stmt(if_stmt->then_branch);
      if (if_stmt->else_branch != NULL) {
        free_stmt(if_stmt->else_branch);
      }
      break;
    case STMT_WHILE:
      WhileStmt* while_stmt = as_while_stmt(stmt);
      free_expr(while_stmt->condition);
      free_stmt(while_stmt->body);
      break;
    default:
      unreachable_code();
      break;
  }

  free(stmt);
}