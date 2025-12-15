#include "interpreter.h"
#include <stdlib.h>
#include "../utils/panic.h"
#include "environment.h"
#include "error.h"
#include "expr.h"
#include "stmt.h"
#include "token.h"
#include "value.h"

#define TRY_EVAL(interpreter, expr)                       \
  ({                                                      \
    Value __internal_value = evaluate(interpreter, expr); \
    if (is_value_type(__internal_value, VALUE_ERR)) {     \
      return __internal_value;                            \
    }                                                     \
    __internal_value;                                     \
  })

static Value execute(Interpreter* interpreter, Stmt* stmt);
static Value execute_expr(Interpreter* interpreter, ExprStmt* stmt);
static Value execute_print(Interpreter* interpreter, PrintStmt* stmt);
static Value execute_var_decl(Interpreter* interpreter, VarDeclStmt* stmt);
static Value execute_block(Interpreter* interpreter, BlockStmt* stmt);
static Value execute_if(Interpreter* interpreter, IfStmt* stmt);
static Value execute_while(Interpreter* interpreter, WhileStmt* stmt);

static Value evaluate_literal(LiteralExpr* expr);
static Value evaluate_unary(Interpreter* interpreter, UnaryExpr* expr);
static Value evaluate_binary(Interpreter* interpreter, BinaryExpr* expr);
static Value evaluate_logical_binary(Interpreter* interpreter, LogicalBinaryExpr* expr);
static Value evaluate_ternary(Interpreter* interpreter, TernaryExpr* expr);
static Value evaluate_var(Interpreter* interpreter, VarExpr* expr);
static Value evaluate_assignment(Interpreter* interpreter, AssignmentExpr* expr);

static bool is_truthy(Value value);

bool interpret(Interpreter* interpreter, List* stmts, RuntimeError* err) {
  for (ListNode* curr = stmts->head; curr != NULL; curr = curr->next) {
    Value val = execute(interpreter, curr->data);

    if (is_value_type(val, VALUE_ERR)) {
      *err = as_err_value(val);
      return false;
    }

    free_value(val);
  }

  return true;
}

static Value execute(Interpreter* interpreter, Stmt* stmt) {
  switch (stmt->type) {
    case STMT_EXPR:
      return execute_expr(interpreter, as_expr_stmt(stmt));
    case STMT_PRINT:
      return execute_print(interpreter, as_print_stmt(stmt));
    case STMT_VAR_DECL:
      return execute_var_decl(interpreter, as_var_decl_stmt(stmt));
    case STMT_BLOCK:
      return execute_block(interpreter, as_block_stmt(stmt));
    case STMT_IF:
      return execute_if(interpreter, as_if_stmt(stmt));
    case STMT_WHILE:
      return execute_while(interpreter, as_while_stmt(stmt));
    default:
      unreachable_code();
  }
}

static Value execute_expr(Interpreter* interpreter, ExprStmt* stmt) {
  return evaluate(interpreter, stmt->expr);
}
static Value execute_print(Interpreter* interpreter, PrintStmt* stmt) {
  Value val = TRY_EVAL(interpreter, stmt->expr);

  print_value(val);

  free_value(val);

  return new_nil_value();
}

static Value execute_var_decl(Interpreter* interpreter, VarDeclStmt* stmt) {
  Value value = new_nil_value();

  if (stmt->initializer != NULL) {
    value = TRY_EVAL(interpreter, stmt->initializer);
  }

  env_define(&interpreter->env, stmt->name->lexeme, value);

  return new_nil_value();
}

static Value execute_block(Interpreter* interpreter, BlockStmt* stmt) {
  Interpreter block_interpreter = new_interpreter(&interpreter->env);
  RuntimeError err;

  bool is_interpreter_ok
      = interpret(&block_interpreter, &stmt->stmts, &err);

  free_interpreter(block_interpreter);

  if (!is_interpreter_ok) {
    return new_err_value(err);
  }

  return new_nil_value();
}

static Value execute_if(Interpreter* interpreter, IfStmt* stmt) {
  if (is_truthy(TRY_EVAL(interpreter, stmt->condition))) {
    return execute(interpreter, stmt->then_branch);
  } else if (stmt->else_branch != NULL) {
    return execute(interpreter, stmt->else_branch);
  }

  return new_nil_value();
}

static Value execute_while(Interpreter* interpreter, WhileStmt* stmt) {
  while (is_truthy(TRY_EVAL(interpreter, stmt->condition))) {
    Value loop_res = execute(interpreter, stmt->body);

    if (is_value_type(loop_res, VALUE_ERR)) {
      return loop_res;
    }

    free_value(loop_res);
  }

  return new_nil_value();
}

Value evaluate(Interpreter* interpreter, Expr* expr) {
  switch (expr->type) {
    case EXPR_LITERAL:
      return evaluate_literal(as_literal_expr(expr));
    case EXPR_GROUPING:
      return evaluate(interpreter, as_grouping_expr(expr)->subexpr);
    case EXPR_UNARY:
      return evaluate_unary(interpreter, as_unary_expr(expr));
    case EXPR_BINARY:
      return evaluate_binary(interpreter, as_binary_expr(expr));
    case EXPR_LOGICAL_BINARY:
      return evaluate_logical_binary(interpreter, as_logical_binary_expr(expr));
    case EXPR_TERNARY:
      return evaluate_ternary(interpreter, as_ternary_expr(expr));
    case EXPR_VAR:
      return evaluate_var(interpreter, as_var_expr(expr));
    case EXPR_ASSIGNMENT:
      return evaluate_assignment(interpreter, as_assignment_expr(expr));
    default:
      unreachable_code();
  }
}

static Value evaluate_literal(LiteralExpr* expr) {
  switch (expr->literal.type) {
    case LITERAL_NIL:
      return new_nil_value();
    case LITERAL_NUM:
      return new_num_value(expr->literal.data.num);
    case LITERAL_BOOL:
      return new_bool_value(expr->literal.data.boolean);
    case LITERAL_STR:
      return new_str_value(expr->literal.data.str);
    default:
      unreachable_code();
  }
}

static Value evaluate_unary(Interpreter* interpreter, UnaryExpr* expr) {
  Value right = TRY_EVAL(interpreter, expr->right);

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

static Value evaluate_binary(Interpreter* interpreter, BinaryExpr* expr) {
  Value left = TRY_EVAL(interpreter, expr->left);
  Value right = TRY_EVAL(interpreter, expr->right);

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

static Value evaluate_logical_binary(Interpreter* interpreter, LogicalBinaryExpr* expr) {
  Value left = TRY_EVAL(interpreter, expr->left);

  if (expr->operator->type == TOKEN_OR) {
    if (is_truthy(left)) {
      return left;
    }
  } else {
    if (!is_truthy(left)) {
      return left;
    }
  }

  return evaluate(interpreter, expr->right);
}

static Value evaluate_ternary(Interpreter* interpreter, TernaryExpr* expr) {
  Value condition_value = TRY_EVAL(interpreter, expr->condition);

  Expr* expr_to_eval = is_truthy(condition_value)
      ? expr->expr_if_true
      : expr->expr_if_false;

  free_value(condition_value);

  return evaluate(interpreter, expr_to_eval);
}

static Value evaluate_var(Interpreter* interpreter, VarExpr* expr) {
  StrSlice name = expr->name->lexeme;

  Value* val = env_get(&interpreter->env, name);

  if (val == NULL) {
    return new_err_value(new_runtime_err(expr->name, "Undefined variable %.*s.", (int)name.length, name.str));
  }

  return clone_value(*val);
}

static Value evaluate_assignment(Interpreter* interpreter, AssignmentExpr* expr) {
  Value value = TRY_EVAL(interpreter, expr->value);

  Value* cur_var_value = env_assign(&interpreter->env, expr->name->lexeme, value);

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