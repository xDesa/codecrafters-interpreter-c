#include "interpreter.h"
#include <stdlib.h>
#include "../utils/panic.h"
#include "error.h"
#include "expr.h"
#include "stmt.h"
#include "token.h"
#include "value.h"

#define TRY_EVAL(expr)                                \
  ({                                                  \
    Value __internal_value = evaluate(expr);          \
    if (is_value_type(__internal_value, VALUE_ERR)) { \
      return __internal_value;                        \
    }                                                 \
    __internal_value;                                 \
  })

static Value execute(Stmt* stmt);
static Value execute_expr(ExprStmt* stmt);
static Value execute_print(PrintStmt* stmt);

static Value evaluate_literal(LiteralExpr* expr);
static Value evaluate_unary(UnaryExpr* expr);
static Value evaluate_binary(BinaryExpr* expr);
static Value evaluate_ternary(TernaryExpr* expr);

static bool is_truthy(Value value);

bool interpret(List* stmts, RuntimeError* err) {
  for (ListNode* curr = stmts->head; curr != NULL; curr = curr->next) {
    Value val = execute(curr->data);

    if (is_value_type(val, VALUE_ERR)) {
      *err = as_err_value(val);
      return false;
    }

    free_value(val);
  }

  return true;
}

static Value execute(Stmt* stmt) {
  switch (stmt->type) {
    case STMT_EXPR:
      return execute_expr(as_expr_stmt(stmt));
    case STMT_PRINT:
      return execute_print(as_print_stmt(stmt));
    default:
      unreachable_code();
  }
}

static Value execute_expr(ExprStmt* stmt) {
  return evaluate(stmt->expr);
}
static Value execute_print(PrintStmt* stmt) {
  Value val = TRY_EVAL(stmt->expr);

  print_value(val);

  return val;
}

Value evaluate(Expr* expr) {
  switch (expr->type) {
    case EXPR_LITERAL:
      return evaluate_literal(as_literal_expr(expr));
    case EXPR_GROUPING:
      return evaluate(as_grouping_expr(expr)->subexpr);
    case EXPR_UNARY:
      return evaluate_unary(as_unary_expr(expr));
    case EXPR_BINARY:
      return evaluate_binary(as_binary_expr(expr));
    case EXPR_TERNARY:
      return evaluate_ternary(as_ternary_expr(expr));
    default:
      unreachable_code();
  }
}

static Value evaluate_literal(LiteralExpr* expr) {
  if (is_nil_literal(expr->literal)) {
    return new_nil_value();
  } else if (is_num_literal(expr->literal)) {
    return new_num_value(expr->literal->literal.num);
  } else if (is_bool_literal(expr->literal)) {
    return new_bool_value(expr->literal->literal.boolean);
  } else if (is_str_literal(expr->literal)) {
    return new_str_value(expr->literal->literal.str);
  } else {
    unreachable_code();
  }
}

static Value evaluate_unary(UnaryExpr* expr) {
  Value right = TRY_EVAL(expr->right);

  Value val = new_nil_value();

  switch (expr->operator->type) {
    case TOKEN_MINUS:
      if (is_value_type(right, VALUE_NUMBER)) {
        val = new_num_value(-as_num_value(right));
      } else {
        val = new_err_value(new_runtime_err(expr->operator, "Operand must be a number."));
      }
      break;
    case TOKEN_BANG:
      val = new_bool_value(!is_truthy(right));
      break;
    default:
      unreachable_code();
  }

  free_value(right);

  return val;
}

static Value evaluate_binary(BinaryExpr* expr) {
  Value left = TRY_EVAL(expr->left);
  Value right = TRY_EVAL(expr->right);

  Value val = new_nil_value();

  switch (expr->operator->type) {
    case TOKEN_MINUS:
      if (is_value_type(left, VALUE_NUMBER) && is_value_type(right, VALUE_NUMBER)) {
        val = new_num_value(as_num_value(left) - as_num_value(right));
      } else {
        val = new_err_value(new_runtime_err(expr->operator, "Operands must be a number."));
      }
      break;
    case TOKEN_PLUS:
      if (is_value_type(left, VALUE_NUMBER) && is_value_type(right, VALUE_NUMBER)) {
        val = new_num_value(as_num_value(left) + as_num_value(right));
      } else if (is_value_type(left, VALUE_STRING) && is_value_type(right, VALUE_STRING)) {
        char* str_plus_str = xstrcat(as_str_value(left), as_str_value(right));
        val = new_str_value(str_plus_str);
        free(str_plus_str);
#ifdef CONCAT_STR_AND_NUM
      } else if (is_value_type(left, VALUE_STRING) && is_value_type(right, VALUE_NUMBER)) {
        char* str_plus_num = xstrcat(as_str_value(left), stringify_num_value(as_num_value(right)));
        val = new_str_value(str_plus_num);
        free(str_plus_num);
      } else if (is_value_type(left, VALUE_NUMBER) && is_value_type(right, VALUE_STRING)) {
        char* num_plus_str = xstrcat(stringify_num_value(as_num_value(left)), as_str_value(right));
        val = new_str_value(num_plus_str);
        free(num_plus_str);
#endif /* CONCAT_STR_AND_NUM */
      } else {
        val = new_err_value(new_runtime_err(expr->operator, "Operands must be numbers or strings."));
      }
      break;
    case TOKEN_SLASH:
      if (is_value_type(left, VALUE_NUMBER) && is_value_type(right, VALUE_NUMBER)) {
        if (as_num_value(right) == 0) {
          val = new_err_value(new_runtime_err(expr->operator, "Attempt to divide by 0."));
        } else {
          val = new_num_value(as_num_value(left) / as_num_value(right));
        }
      } else {
        val = new_err_value(new_runtime_err(expr->operator, "Operands must be a number."));
      }
      break;
    case TOKEN_STAR:
      if (is_value_type(left, VALUE_NUMBER) && is_value_type(right, VALUE_NUMBER)) {
        val = new_num_value(as_num_value(left) * as_num_value(right));
      } else {
        val = new_err_value(new_runtime_err(expr->operator, "Operands must be a number."));
      }
      break;
    case TOKEN_GREATER:
      if (is_value_type(left, VALUE_NUMBER) && is_value_type(right, VALUE_NUMBER)) {
        val = new_num_value(as_num_value(left) > as_num_value(right));
      } else {
        val = new_err_value(new_runtime_err(expr->operator, "Operands must be a number."));
      }
      break;
    case TOKEN_GREATER_EQUAL:
      if (is_value_type(left, VALUE_NUMBER) && is_value_type(right, VALUE_NUMBER)) {
        val = new_num_value(as_num_value(left) >= as_num_value(right));
      } else {
        val = new_err_value(new_runtime_err(expr->operator, "Operands must be a number."));
      }
      break;
    case TOKEN_LESS:
      if (is_value_type(left, VALUE_NUMBER) && is_value_type(right, VALUE_NUMBER)) {
        val = new_num_value(as_num_value(left) < as_num_value(right));
      } else {
        val = new_err_value(new_runtime_err(expr->operator, "Operands must be a number."));
      }
      break;
    case TOKEN_LESS_EQUAL:
      if (is_value_type(left, VALUE_NUMBER) && is_value_type(right, VALUE_NUMBER)) {
        val = new_num_value(as_num_value(left) <= as_num_value(right));
      } else {
        val = new_err_value(new_runtime_err(expr->operator, "Operands must be a number."));
      }
      break;
    case TOKEN_EQUAL_EQUAL:
      val = new_bool_value(are_values_eq(left, right));
      break;
    case TOKEN_BANG_EQUAL:
      val = new_bool_value(!are_values_eq(left, right));
      break;
    case TOKEN_COMMA:
      val = clone_value(right);
      break;
    default:
      unreachable_code();
  }

  free_value(left);
  free_value(right);

  return val;
}

static Value evaluate_ternary(TernaryExpr* expr) {
  Value condition_value = TRY_EVAL(expr->condition);

  Expr* expr_to_eval = is_truthy(condition_value)
      ? expr->expr_if_true
      : expr->expr_if_false;

  free_value(condition_value);

  return evaluate(expr_to_eval);
}

// only false and nil are falsy
static bool is_truthy(Value value) {
  switch (value.type) {
    case VALUE_NIL:
      return false;
    case VALUE_BOOL:
      return as_bool_value(value);
    default:
      return true;
  }
}