#ifndef CLOX_EXPR_H
#define CLOX_EXPR_H

#include "token.h"

typedef enum {
  EXPR_BINARY,
  EXPR_UNARY,
  EXPR_GROUPING,
  EXPR_LITERAL,
  EXPR_TERNARY,
  EXPR_VAR,
} ExprType;

typedef struct {
  ExprType type;
} Expr;

typedef struct {
  Expr base;
  Expr* left;
  Token* operator;
  Expr* right;
} BinaryExpr;

typedef struct {
  Expr base;
  Token* operator;
  Expr* right;
} UnaryExpr;

typedef struct {
  Expr base;
  Expr* subexpr;
} GroupingExpr;

typedef struct {
  Expr base;
  Token* literal;
} LiteralExpr;

typedef struct {
  Expr base;
  Expr* condition;
  Expr* expr_if_true;
  Expr* expr_if_false;
} TernaryExpr;

typedef struct {
  Expr base;
  Token* name;
} VarExpr;

static inline BinaryExpr* as_binary_expr(Expr* expr) {
  return (BinaryExpr*)expr;
}

static inline UnaryExpr* as_unary_expr(Expr* expr) {
  return (UnaryExpr*)expr;
}

static inline GroupingExpr* as_grouping_expr(Expr* expr) {
  return (GroupingExpr*)expr;
}

static inline LiteralExpr* as_literal_expr(Expr* expr) {
  return (LiteralExpr*)expr;
}

static inline TernaryExpr* as_ternary_expr(Expr* expr) {
  return (TernaryExpr*)expr;
}

static inline VarExpr* as_var_expr(Expr* expr) {
  return (VarExpr*)expr;
}

Expr* new_binary_expr(Expr* left, Token* operator, Expr * right);

Expr* new_unary_expr(Token* operator, Expr * right);

Expr* new_grouping_expr(Expr* subexpr);

Expr* new_literal_expr(Token* literal);

Expr* new_ternary_expr(Expr* condition, Expr* expr_if_true, Expr* expr_if_false);

Expr* new_var_expr(Token* name);

void free_expr(Expr* expr);

#endif /* CLOX_EXPR_H */