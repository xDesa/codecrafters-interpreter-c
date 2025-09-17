#ifndef CLOX_EXPR_H
#define CLOX_EXPR_H

#include "token.h"

typedef enum {
  EXPR_BINARY,
  EXPR_UNARY,
  EXPR_GROUPING,
  EXPR_LITERAL,
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

Expr* new_binary_expr(Expr* left, Token* operator, Expr * right);

Expr* new_unary_expr(Token* operator, Expr * right);

Expr* new_grouping_expr(Expr* subexpr);

Expr* new_literal_expr(Token* literal);

#endif /* CLOX_EXPR_H */