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

Expr* new_ternary_expr(Expr* condition, Expr* expr_if_true, Expr* expr_if_false) {
  TernaryExpr* ternary = xmalloc(sizeof(TernaryExpr));
  ternary->base.type = EXPR_TERNARY;
  ternary->condition = condition;
  ternary->expr_if_true = expr_if_true;
  ternary->expr_if_false = expr_if_false;

  return (Expr*)ternary;
}

Expr* new_var_expr(Token* name) {
  VarExpr* var = xmalloc(sizeof(VarExpr));
  var->base.type = EXPR_VAR;
  var->name = name;

  return (Expr*)var;
}

Expr* new_assignment_expr(Token* name, Expr* value) {
  AssignmentExpr* assignment = xmalloc(sizeof(AssignmentExpr));
  assignment->base.type = EXPR_ASSIGNMENT;
  assignment->name = name;
  assignment->value = value;

  return (Expr*)assignment;
}

Expr* new_logical_binary_expr(Expr* left, Token* operator, Expr* right) {
  LogicalBinaryExpr* logical_bin_expr = xmalloc(sizeof(LogicalBinaryExpr));
  logical_bin_expr->base.type = EXPR_LOGICAL_BINARY;
  logical_bin_expr->left = left;
  logical_bin_expr->operator = operator;
  logical_bin_expr->right = right;

  return (Expr*)logical_bin_expr;
}

void free_expr(Expr* expr) {
  switch (expr->type) {
    case EXPR_BINARY:
      BinaryExpr* binary_expr = as_binary_expr(expr);
      free_expr(binary_expr->left);
      free_expr(binary_expr->right);
      break;
    case EXPR_UNARY:
      UnaryExpr* unary_expr = as_unary_expr(expr);
      free_expr(unary_expr->right);
      break;
    case EXPR_GROUPING:
      GroupingExpr* grouping_expr = as_grouping_expr(expr);
      free_expr(grouping_expr->subexpr);
      break;
    case EXPR_LITERAL:
      // nothing internal to free
      break;
    case EXPR_TERNARY:
      TernaryExpr* ternary_expr = as_ternary_expr(expr);
      free_expr(ternary_expr->condition);
      free_expr(ternary_expr->expr_if_true);
      free_expr(ternary_expr->expr_if_false);
      break;
    case EXPR_VAR:
      // nothing internal to free
      break;
    case EXPR_ASSIGNMENT:
      AssignmentExpr* assignment_expr = as_assignment_expr(expr);
      free_expr(assignment_expr->value);
      break;
    case EXPR_LOGICAL_BINARY:
      LogicalBinaryExpr* logical_binary_expr = as_logical_binary_expr(expr);
      free_expr(logical_binary_expr->left);
      free_expr(logical_binary_expr->right);
      break;
    default:
      unreachable_code();
      break;
  }

  free(expr);
}