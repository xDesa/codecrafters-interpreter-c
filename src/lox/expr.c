#include "expr.h"
#include <assert.h>
#include "token.h"

Expr* new_binary_expr(Expr* left, Token* operator, Expr* right) {
  BinaryExpr* bin_expr = xmalloc(sizeof(BinaryExpr));
  bin_expr->base.type = EXPR_BINARY;
  bin_expr->left = left;
  bin_expr->operator = operator;
  bin_expr->right = right;

  return (Expr*)bin_expr;
}

Expr* new_unary_expr(Token* operator, Expr* right) {
  UnaryExpr* unary_expr = xmalloc(sizeof(UnaryExpr));
  unary_expr->base.type = EXPR_UNARY;
  unary_expr->operator = operator;
  unary_expr->right = right;

  return (Expr*)unary_expr;
}

Expr* new_grouping_expr(Expr* subexpr) {
  GroupingExpr* grouping_expr = xmalloc(sizeof(GroupingExpr));
  grouping_expr->base.type = EXPR_GROUPING;
  grouping_expr->subexpr = subexpr;

  return (Expr*)grouping_expr;
}

Expr* new_literal_expr(Token* literal) {
  assert(is_token_literal_value(literal));

  LiteralExpr* literal_expr = xmalloc(sizeof(LiteralExpr));
  literal_expr->base.type = EXPR_LITERAL;
  literal_expr->literal = literal;

  return (Expr*)literal_expr;
}