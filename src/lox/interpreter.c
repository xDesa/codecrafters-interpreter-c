#include "interpreter.h"
#include <stdlib.h>
#include "../utils/panic.h"
#include "error.h"
#include "expr.h"
#include "token.h"
#include "value.h"

#define OUTPUT(dest, src) \
  ({                      \
    if (dest != NULL) {   \
      *dest = src;        \
    }                     \
    INTERPRETER_OK;       \
  })

#define RUNTIME_ERROR(dest, src) \
  ({                             \
    if (dest != NULL) {          \
      *dest = src;               \
    }                            \
    INTERPRETER_RUNTIME_ERROR;   \
  })

#define TRY_EVAL(expr, err, before_return...)                                  \
  ({                                                                           \
    Value __internal_value;                                                    \
    InterpreterResult __internal_res = evaluate(expr, &__internal_value, err); \
    if (__internal_res != INTERPRETER_OK) {                                    \
      before_return;                                                           \
      return __internal_res;                                                   \
    }                                                                          \
    __internal_value;                                                          \
  })

static InterpreterResult evaluate_literal(LiteralExpr* expr, Value* output);
static InterpreterResult evaluate_unary(UnaryExpr* expr, Value* output, RuntimeError* err);
static InterpreterResult evaluate_binary(BinaryExpr* expr, Value* output, RuntimeError* err);
static InterpreterResult evaluate_ternary(TernaryExpr* expr, Value* output, RuntimeError* err);

static bool is_truthy(Value value);

InterpreterResult evaluate(Expr* expr, Value* output, RuntimeError* err) {
  switch (expr->type) {
    case EXPR_LITERAL:
      return evaluate_literal(as_literal_expr(expr), output);
    case EXPR_GROUPING:
      return evaluate(as_grouping_expr(expr)->subexpr, output, err);
    case EXPR_UNARY:
      return evaluate_unary(as_unary_expr(expr), output, err);
    case EXPR_BINARY:
      return evaluate_binary(as_binary_expr(expr), output, err);
    case EXPR_TERNARY:
      return evaluate_ternary(as_ternary_expr(expr), output, err);
    default:
      unreachable_code();
  }
}

static InterpreterResult evaluate_literal(LiteralExpr* expr, Value* output) {
  if (is_nil_literal(expr->literal)) {
    return OUTPUT(output, new_nil_value());
  } else if (is_num_literal(expr->literal)) {
    return OUTPUT(output, new_num_value(expr->literal->literal.num));
  } else if (is_bool_literal(expr->literal)) {
    return OUTPUT(output, new_bool_value(expr->literal->literal.boolean));
  } else if (is_str_literal(expr->literal)) {
    return OUTPUT(output, new_str_value(expr->literal->literal.str));
  } else {
    unreachable_code();
  }
}

static InterpreterResult evaluate_unary(UnaryExpr* expr, Value* output, RuntimeError* err) {
  InterpreterResult res;
  Value right = TRY_EVAL(expr->right, err);

  switch (expr->operator->type) {
    case TOKEN_MINUS:
      if (is_value_type(right, VALUE_NUMBER)) {
        res = OUTPUT(output, new_num_value(-as_num_value(right)));
      } else {
        res = RUNTIME_ERROR(err, new_runtime_err(expr->operator, "Operand must be a number."));
      }
      break;
    case TOKEN_BANG:
      res = OUTPUT(output, new_bool_value(!is_truthy(right)));
      break;
    default:
      unreachable_code();
  }

  free_value(right);

  return res;
}

static InterpreterResult evaluate_binary(BinaryExpr* expr, Value* output, RuntimeError* err) {
  InterpreterResult res;
  Value left = TRY_EVAL(expr->left, err);
  Value right = TRY_EVAL(expr->right, err);

  switch (expr->operator->type) {
    case TOKEN_MINUS:
      if (is_value_type(left, VALUE_NUMBER) && is_value_type(right, VALUE_NUMBER)) {
        res = OUTPUT(output, new_num_value(as_num_value(left) - as_num_value(right)));
      } else {
        res = RUNTIME_ERROR(err, new_runtime_err(expr->operator, "Operands must be a number."));
      }
      break;
    case TOKEN_PLUS:
      if (is_value_type(left, VALUE_NUMBER) && is_value_type(right, VALUE_NUMBER)) {
        res = OUTPUT(output, new_num_value(as_num_value(left) + as_num_value(right)));
      } else if (is_value_type(left, VALUE_STRING) && is_value_type(right, VALUE_STRING)) {
        char* str_plus_str = xstrcat(as_str_value(left), as_str_value(right));
        res = OUTPUT(output, new_str_value(str_plus_str));
        free(str_plus_str);
#ifdef CONCAT_STR_AND_NUM
      } else if (is_value_type(left, VALUE_STRING) && is_value_type(right, VALUE_NUMBER)) {
        char* str_plus_num = xstrcat(as_str_value(left), stringify_num_value(as_num_value(right)));
        res = OUTPUT(output, new_str_value(str_plus_num));
        free(str_plus_num);
      } else if (is_value_type(left, VALUE_NUMBER) && is_value_type(right, VALUE_STRING)) {
        char* num_plus_str = xstrcat(stringify_num_value(as_num_value(left)), as_str_value(right));
        res = OUTPUT(output, new_str_value(num_plus_str));
        free(num_plus_str);
#endif /* CONCAT_STR_AND_NUM */
      } else {
        res = RUNTIME_ERROR(err, new_runtime_err(expr->operator, "Operands must be numbers or strings."));
      }
      break;
    case TOKEN_SLASH:
      if (is_value_type(left, VALUE_NUMBER) && is_value_type(right, VALUE_NUMBER)) {
        if (as_num_value(right) == 0) {
          res = RUNTIME_ERROR(err, new_runtime_err(expr->operator, "Attempt to divide by 0."));
        } else {
          res = OUTPUT(output, new_num_value(as_num_value(left) / as_num_value(right)));
        }
      } else {
        res = RUNTIME_ERROR(err, new_runtime_err(expr->operator, "Operands must be a number."));
      }
      break;
    case TOKEN_STAR:
      if (is_value_type(left, VALUE_NUMBER) && is_value_type(right, VALUE_NUMBER)) {
        res = OUTPUT(output, new_num_value(as_num_value(left) * as_num_value(right)));
      } else {
        res = RUNTIME_ERROR(err, new_runtime_err(expr->operator, "Operands must be a number."));
      }
      break;
    case TOKEN_GREATER:
      if (is_value_type(left, VALUE_NUMBER) && is_value_type(right, VALUE_NUMBER)) {
        res = OUTPUT(output, new_num_value(as_num_value(left) > as_num_value(right)));
      } else {
        res = RUNTIME_ERROR(err, new_runtime_err(expr->operator, "Operands must be a number."));
      }
      break;
    case TOKEN_GREATER_EQUAL:
      if (is_value_type(left, VALUE_NUMBER) && is_value_type(right, VALUE_NUMBER)) {
        res = OUTPUT(output, new_num_value(as_num_value(left) >= as_num_value(right)));
      } else {
        res = RUNTIME_ERROR(err, new_runtime_err(expr->operator, "Operands must be a number."));
      }
      break;
    case TOKEN_LESS:
      if (is_value_type(left, VALUE_NUMBER) && is_value_type(right, VALUE_NUMBER)) {
        res = OUTPUT(output, new_num_value(as_num_value(left) < as_num_value(right)));
      } else {
        res = RUNTIME_ERROR(err, new_runtime_err(expr->operator, "Operands must be a number."));
      }
      break;
    case TOKEN_LESS_EQUAL:
      if (is_value_type(left, VALUE_NUMBER) && is_value_type(right, VALUE_NUMBER)) {
        res = OUTPUT(output, new_num_value(as_num_value(left) <= as_num_value(right)));
      } else {
        res = RUNTIME_ERROR(err, new_runtime_err(expr->operator, "Operands must be a number."));
      }
      break;
    case TOKEN_EQUAL_EQUAL:
      res = OUTPUT(output, new_bool_value(are_values_eq(left, right)));
      break;
    case TOKEN_BANG_EQUAL:
      res = OUTPUT(output, new_bool_value(!are_values_eq(left, right)));
      break;
    case TOKEN_COMMA:
      res = OUTPUT(output, clone_value(right));
      break;
    default:
      unreachable_code();
  }

  free_value(left);
  free_value(right);

  return res;
}

static InterpreterResult evaluate_ternary(TernaryExpr* expr, Value* output, RuntimeError* err) {
  Value condition_value = TRY_EVAL(expr->condition, err);

  Expr* expr_to_eval = is_truthy(condition_value)
      ? expr->expr_if_true
      : expr->expr_if_false;

  free_value(condition_value);

  return OUTPUT(output, TRY_EVAL(expr_to_eval, err));
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