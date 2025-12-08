#include "interpreter.h"
#include <stdio.h>
#include <stdlib.h>
#include "../utils/panic.h"
#include "environment.h"
#include "error.h"
#include "expr.h"
#include "stmt.h"
#include "token.h"
#include "value.h"

#define TRY_EVAL(env, expr)                           \
  ({                                                  \
    Value __internal_value = evaluate(env, expr);     \
    if (is_value_type(__internal_value, VALUE_ERR)) { \
      return __internal_value;                        \
    }                                                 \
    __internal_value;                                 \
  })

static Value execute(Stmt* stmt, Environment* env);
static Value execute_expr(ExprStmt* stmt, Environment* env);
static Value execute_print(PrintStmt* stmt, Environment* env);
static Value execute_var_decl(VarDeclStmt* stmt, Environment* env);
static Value execute_block(BlockStmt* stmt, Environment* enclosing_env);
static Value execute_if(IfStmt* stmt, Environment* env);
static Value execute_while(WhileStmt* stmt, Environment* env);

static Value evaluate_literal(LiteralExpr* expr);
static Value evaluate_unary(UnaryExpr* expr, Environment* env);
static Value evaluate_binary(BinaryExpr* expr, Environment* env);
static Value evaluate_logical_binary(LogicalBinaryExpr* expr, Environment* env);
static Value evaluate_ternary(TernaryExpr* expr, Environment* env);
static Value evaluate_var(VarExpr* expr, Environment* env);
static Value evaluate_assignment(AssignmentExpr* expr, Environment* env);

static bool is_truthy(Value value);

bool interpret(Environment* env, List* stmts, RuntimeError* err) {
  for (ListNode* curr = stmts->head; curr != NULL; curr = curr->next) {
    Value val = execute(curr->data, env);

    if (is_value_type(val, VALUE_ERR)) {
      *err = as_err_value(val);
      return false;
    }

    free_value(val);
  }

  return true;
}

static Value execute(Stmt* stmt, Environment* env) {
  switch (stmt->type) {
    case STMT_EXPR:
      return execute_expr(as_expr_stmt(stmt), env);
    case STMT_PRINT:
      return execute_print(as_print_stmt(stmt), env);
    case STMT_VAR_DECL:
      return execute_var_decl(as_var_decl_stmt(stmt), env);
    case STMT_BLOCK:
      return execute_block(as_block_stmt(stmt), env);
    case STMT_IF:
      return execute_if(as_if_stmt(stmt), env);
    case STMT_WHILE:
      return execute_while(as_while_stmt(stmt), env);
    default:
      unreachable_code();
  }
}

static Value execute_expr(ExprStmt* stmt, Environment* env) {
  return evaluate(env, stmt->expr);
}
static Value execute_print(PrintStmt* stmt, Environment* env) {
  Value val = TRY_EVAL(env, stmt->expr);

  print_value(val);

  free_value(val);

  return new_nil_value();
}

static Value execute_var_decl(VarDeclStmt* stmt, Environment* env) {
  Value value = new_nil_value();

  if (stmt->initializer != NULL) {
    value = TRY_EVAL(env, stmt->initializer);
  }

  env_define(env, stmt->name->lexeme, value);

  return new_nil_value();
}

static Value execute_block(BlockStmt* stmt, Environment* enclosing_env) {
  Environment block_env = new_env(enclosing_env);
  RuntimeError err;

  bool is_interpreter_ok
      = interpret(&block_env, &stmt->stmts, &err);

  free_env(&block_env);

  if (!is_interpreter_ok) {
    return new_err_value(err);
  }

  return new_nil_value();
}

static Value execute_if(IfStmt* stmt, Environment* env) {
  if (is_truthy(TRY_EVAL(env, stmt->condition))) {
    return execute(stmt->then_branch, env);
  } else if (stmt->else_branch != NULL) {
    return execute(stmt->else_branch, env);
  }

  return new_nil_value();
}

static Value execute_while(WhileStmt* stmt, Environment* env) {
  while (is_truthy(TRY_EVAL(env, stmt->condition))) {
    Value loop_res = execute(stmt->body, env);

    if (is_value_type(loop_res, VALUE_ERR)) {
      return loop_res;
    }

    free_value(loop_res);
  }

  return new_nil_value();
}

Value evaluate(Environment* env, Expr* expr) {
  switch (expr->type) {
    case EXPR_LITERAL:
      return evaluate_literal(as_literal_expr(expr));
    case EXPR_GROUPING:
      return evaluate(env, as_grouping_expr(expr)->subexpr);
    case EXPR_UNARY:
      return evaluate_unary(as_unary_expr(expr), env);
    case EXPR_BINARY:
      return evaluate_binary(as_binary_expr(expr), env);
    case EXPR_LOGICAL_BINARY:
      return evaluate_logical_binary(as_logical_binary_expr(expr), env);
    case EXPR_TERNARY:
      return evaluate_ternary(as_ternary_expr(expr), env);
    case EXPR_VAR:
      return evaluate_var(as_var_expr(expr), env);
    case EXPR_ASSIGNMENT:
      return evaluate_assignment(as_assignment_expr(expr), env);
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

static Value evaluate_unary(UnaryExpr* expr, Environment* env) {
  Value right = TRY_EVAL(env, expr->right);

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

static Value evaluate_binary(BinaryExpr* expr, Environment* env) {
  Value left = TRY_EVAL(env, expr->left);
  Value right = TRY_EVAL(env, expr->right);

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
        val = new_bool_value(as_num_value(left) > as_num_value(right));
      } else {
        val = new_err_value(new_runtime_err(expr->operator, "Operands must be a number."));
      }
      break;
    case TOKEN_GREATER_EQUAL:
      if (is_value_type(left, VALUE_NUMBER) && is_value_type(right, VALUE_NUMBER)) {
        val = new_bool_value(as_num_value(left) >= as_num_value(right));
      } else {
        val = new_err_value(new_runtime_err(expr->operator, "Operands must be a number."));
      }
      break;
    case TOKEN_LESS:
      if (is_value_type(left, VALUE_NUMBER) && is_value_type(right, VALUE_NUMBER)) {
        val = new_bool_value(as_num_value(left) < as_num_value(right));
      } else {
        val = new_err_value(new_runtime_err(expr->operator, "Operands must be a number."));
      }
      break;
    case TOKEN_LESS_EQUAL:
      if (is_value_type(left, VALUE_NUMBER) && is_value_type(right, VALUE_NUMBER)) {
        val = new_bool_value(as_num_value(left) <= as_num_value(right));
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

static Value evaluate_logical_binary(LogicalBinaryExpr* expr, Environment* env) {
  Value left = TRY_EVAL(env, expr->left);

  if (expr->operator->type == TOKEN_OR) {
    if (is_truthy(left)) {
      return left;
    }
  } else {
    if (!is_truthy(left)) {
      return left;
    }
  }

  return evaluate(env, expr->right);
}

static Value evaluate_ternary(TernaryExpr* expr, Environment* env) {
  Value condition_value = TRY_EVAL(env, expr->condition);

  Expr* expr_to_eval = is_truthy(condition_value)
      ? expr->expr_if_true
      : expr->expr_if_false;

  free_value(condition_value);

  return evaluate(env, expr_to_eval);
}

static Value evaluate_var(VarExpr* expr, Environment* env) {
  StrSlice name = expr->name->lexeme;

  Value* val = env_get(env, name);

  if (val == NULL) {
    return new_err_value(new_runtime_err(expr->name, "Undefined variable %.*s.", (int)name.length, name.str));
  }

  return clone_value(*val);
}

static Value evaluate_assignment(AssignmentExpr* expr, Environment* env) {
  Value value = TRY_EVAL(env, expr->value);

  Value* cur_var_value = env_assign(env, expr->name->lexeme, value);

  if (cur_var_value == NULL) {
    StrSlice name = expr->name->lexeme;
    return new_err_value(new_runtime_err(expr->name, "Undefined variable %.*s.", (int)name.length, name.str));
  }

  return value;
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