#include "expr.h"
#include <assert.h>
#include <stdlib.h>
#include "../utils/panic.h"
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

void free_expr(Expr* expr) {
  switch (expr->type) {
    case EXPR_BINARY:
      BinaryExpr* binary_expr = as_binary_expr(expr);
      free_expr(binary_expr->left);
      free_expr(binary_expr->right);
      free(binary_expr);
      break;
    case EXPR_UNARY:
      UnaryExpr* unary_expr = as_unary_expr(expr);
      free_expr(unary_expr->right);
      free(unary_expr);
      break;
    case EXPR_GROUPING:
      GroupingExpr* grouping_expr = as_grouping_expr(expr);
      free_expr(grouping_expr->subexpr);
      free(grouping_expr);
      break;
    case EXPR_LITERAL:
      LiteralExpr* literal_expr = as_literal_expr(expr);
      free(literal_expr);
      break;
    default:
      unreachable_code();
      break;
  }
}