#ifndef CLOX_STMT_H
#define CLOX_STMT_H

#include "expr.h"

typedef enum {
  STMT_EXPR,
  STMT_PRINT,
  STMT_VAR_DECL,
  STMT_BLOCK,
  STMT_IF,
  STMT_WHILE,
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

typedef struct {
  Stmt base;
  Token* name;
  Expr* initializer;
} VarDeclStmt;

typedef VecType(Stmt*) StmtVec;
#define free_stmt_vec(vec) \
  free_vec_with(vec, Stmt*, free_stmt)

typedef struct {
  Stmt base;
  StmtVec stmts;
} BlockStmt;

typedef struct {
  Stmt base;
  Expr* condition;
  Stmt* then_branch;
  Stmt* else_branch;
} IfStmt;

typedef struct {
  Stmt base;
  Expr* condition;
  Stmt* body;
} WhileStmt;

static inline ExprStmt* as_expr_stmt(Stmt* stmt) {
  return (ExprStmt*)stmt;
}

static inline PrintStmt* as_print_stmt(Stmt* stmt) {
  return (PrintStmt*)stmt;
}

static inline VarDeclStmt* as_var_decl_stmt(Stmt* stmt) {
  return (VarDeclStmt*)stmt;
}

static inline BlockStmt* as_block_stmt(Stmt* stmt) {
  return (BlockStmt*)stmt;
}

static inline IfStmt* as_if_stmt(Stmt* stmt) {
  return (IfStmt*)stmt;
}

static inline WhileStmt* as_while_stmt(Stmt* stmt) {
  return (WhileStmt*)stmt;
}

Stmt* new_expr_stmt(Expr* expr);

Stmt* new_print_stmt(Expr* expr);

Stmt* new_var_decl_stmt(Token* name, Expr* initializer);

Stmt* new_block_stmt(StmtVec stmts);

Stmt* new_if_stmt(Expr* condition, Stmt* then_branch, Stmt* else_branch);

Stmt* new_while_stmt(Expr* condition, Stmt* body);

void free_stmt(Stmt* stmt);

#endif /* CLOX_STMT_H */