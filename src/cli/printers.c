#include "printers.h"
#include <assert.h>
#include <stdio.h>

#include "../lox/token.h"
#include "../utils/panic.h"

static inline bool is_int(double num) {
  return num == (int)num;
}

static const char* token_type_to_string(TokenType type);

static inline void print_token_lexeme(StrSlice lexeme) {
  printf("%.*s", (int)lexeme.length, lexeme.str);
}

void report_syntax_error(SyntaxError err) {
  Token* token = err.token;
  switch (token->type) {
    case TOKEN_ERROR:
      fprintf(stderr, "[line %zu] Error: %s\n", token->line, token->literal.data.error);
      break;
    case TOKEN_EOF:
      fprintf(stderr, "[line %zu] Error at end: %s\n", token->line, err.message);
      break;
    default:
      fprintf(stderr, "[line %zu] Error at '%.*s': %s\n", token->line, (int)token->lexeme.length, token->lexeme.str, err.message);
      break;
  }
}

void report_runtime_error(RuntimeError err) {
  fprintf(stderr, "%s\n[line %zu]\n", err.message, err.token->line);
}

static void print_literal_token(Token token) {
  assert(is_literal_token(token));

  switch (token.type) {
    case TOKEN_NIL:
      printf("nil");
      break;
    case TOKEN_NUMBER:
      if (is_int(token.literal.data.num)) {
        printf("%d.0", (int)token.literal.data.num);
      } else {
        print_token_lexeme(token.lexeme);
      }
      break;
    case TOKEN_TRUE:
    case TOKEN_FALSE:
      printf("%s", token.literal.data.boolean == true ? "true" : "false");
      break;
    case TOKEN_STRING:
      printf("%s", token.literal.data.str);
      break;
    default:
      unreachable_code();
      break;
  }
}

void print_token(Token token) {
  if (token.type == TOKEN_ERROR) {
    report_syntax_error(new_syntax_err(&token, NULL));
    return;
  }

  // print format: <TOKEN_TYPE> <LEXEME> <LITERAL|"null">

  printf("%s ", token_type_to_string(token.type));
  print_token_lexeme(token.lexeme);
  printf(" ");

  // TOKEN_NIL literal is "null" in tokenize cmd, like non literal values tokens
  if (is_literal_token(token) && token.type != TOKEN_NIL) {
    print_literal_token(token);
  } else {
    printf("null");
  }

  printf("\n");
}

static const char* token_type_to_string(TokenType type) {
  switch (type) {
      // clang-format off
    case TOKEN_LEFT_PAREN:    return "LEFT_PAREN";
    case TOKEN_RIGHT_PAREN:   return "RIGHT_PAREN";
    case TOKEN_LEFT_BRACE:    return "LEFT_BRACE";
    case TOKEN_RIGHT_BRACE:   return "RIGHT_BRACE";
    case TOKEN_COMMA:         return "COMMA";
    case TOKEN_DOT:           return "DOT";
    case TOKEN_MINUS:         return "MINUS";
    case TOKEN_PLUS:          return "PLUS";
    case TOKEN_SEMICOLON:     return "SEMICOLON";
    case TOKEN_SLASH:         return "SLASH";
    case TOKEN_STAR:          return "STAR";
    case TOKEN_BANG:          return "BANG";
    case TOKEN_BANG_EQUAL:    return "BANG_EQUAL";
    case TOKEN_EQUAL:         return "EQUAL";
    case TOKEN_EQUAL_EQUAL:   return "EQUAL_EQUAL";
    case TOKEN_GREATER:       return "GREATER";
    case TOKEN_GREATER_EQUAL: return "GREATER_EQUAL";
    case TOKEN_LESS:          return "LESS";
    case TOKEN_LESS_EQUAL:    return "LESS_EQUAL";
    case TOKEN_IDENTIFIER:    return "IDENTIFIER";
    case TOKEN_STRING:        return "STRING";
    case TOKEN_NUMBER:        return "NUMBER";
    case TOKEN_AND:           return "AND";
    case TOKEN_CLASS:         return "CLASS";
    case TOKEN_ELSE:          return "ELSE";
    case TOKEN_FALSE:         return "FALSE";
    case TOKEN_FOR:           return "FOR";
    case TOKEN_FUN:           return "FUN";
    case TOKEN_IF:            return "IF";
    case TOKEN_NIL:           return "NIL";
    case TOKEN_OR:            return "OR";
    case TOKEN_PRINT:         return "PRINT";
    case TOKEN_RETURN:        return "RETURN";
    case TOKEN_SUPER:         return "SUPER";
    case TOKEN_THIS:          return "THIS";
    case TOKEN_TRUE:          return "TRUE";
    case TOKEN_VAR:           return "VAR";
    case TOKEN_WHILE:         return "WHILE";
    case TOKEN_ERROR:         return "ERROR";
    case TOKEN_EOF:           return "EOF";
    default:                  unreachable_code();
      // clang-format on
  }
}

static void print_binary_expr(BinaryExpr* expr);
static void print_unary_expr(UnaryExpr* expr);
static void print_grouping_expr(GroupingExpr* expr);
static void print_literal_expr(LiteralExpr* expr);
static void print_ternary_expr(TernaryExpr* expr);
static void print_var_expr(VarExpr* expr);
static void print_assignment_expr(AssignmentExpr* expr);

void print_expr(Expr* expr) {
  switch (expr->type) {
    case EXPR_BINARY:
      print_binary_expr(as_binary_expr(expr));
      break;
    case EXPR_UNARY:
      print_unary_expr(as_unary_expr(expr));
      break;
    case EXPR_GROUPING:
      print_grouping_expr(as_grouping_expr(expr));
      break;
    case EXPR_LITERAL:
      print_literal_expr(as_literal_expr(expr));
      break;
    case EXPR_TERNARY:
      print_ternary_expr(as_ternary_expr(expr));
      break;
    case EXPR_VAR:
      print_var_expr(as_var_expr(expr));
      break;
    case EXPR_ASSIGNMENT:
      print_assignment_expr(as_assignment_expr(expr));
      break;
    default:
      unreachable_code();
      break;
  }
}

static void print_binary_expr(BinaryExpr* expr) {
  printf("(");
  print_token_lexeme(expr->operator->lexeme);
  printf(" ");
  print_expr(expr->left);
  printf(" ");
  print_expr(expr->right);
  printf(")");
}

static void print_unary_expr(UnaryExpr* expr) {
  printf("(");
  print_token_lexeme(expr->operator->lexeme);
  printf(" ");
  print_expr(expr->right);
  printf(")");
}

static void print_grouping_expr(GroupingExpr* expr) {
  printf("(");
  printf("group");
  printf(" ");
  print_expr(expr->subexpr);
  printf(")");
}

static void print_literal_expr(LiteralExpr* expr) {
  assert(expr->literal_token != NULL);
  print_literal_token(*expr->literal_token);
}

static void print_ternary_expr(TernaryExpr* expr) {
  printf("(");
  printf("?:");
  printf(" ");
  print_expr(expr->condition);
  printf(" ");
  print_expr(expr->expr_if_true);
  printf(" ");
  print_expr(expr->expr_if_false);
  printf(")");
}

static void print_var_expr(VarExpr* expr) {
  printf("(");
  printf("var");
  printf(" ");
  print_token_lexeme(expr->name->lexeme);
  printf(")");
}

static void print_assignment_expr(AssignmentExpr* expr) {
  printf("(");
  printf("=");
  printf(" ");
  print_token_lexeme(expr->name->lexeme);
  printf(" ");
  print_expr(expr->value);
  printf(")");
}

static void rpn_print_binary_expr(BinaryExpr* expr);
static void rpn_print_unary_expr(UnaryExpr* expr);
static void rpn_print_literal_expr(LiteralExpr* expr);
static void rpn_print_ternary_expr(TernaryExpr* expr);
static void rpn_print_var_expr(VarExpr* expr);
static void rpn_print_assignment_expr(AssignmentExpr* expr);

void rpn_print_expr(Expr* expr) {
  switch (expr->type) {
    case EXPR_BINARY:
      rpn_print_binary_expr(as_binary_expr(expr));
      break;
    case EXPR_UNARY:
      rpn_print_unary_expr(as_unary_expr(expr));
      break;
    case EXPR_GROUPING:
      rpn_print_expr(as_grouping_expr(expr)->subexpr);
      break;
    case EXPR_LITERAL:
      rpn_print_literal_expr(as_literal_expr(expr));
      break;
    case EXPR_TERNARY:
      rpn_print_ternary_expr(as_ternary_expr(expr));
      break;
    case EXPR_VAR:
      rpn_print_var_expr(as_var_expr(expr));
      break;
    case EXPR_ASSIGNMENT:
      rpn_print_assignment_expr(as_assignment_expr(expr));
      break;
    default:
      unreachable_code();
      break;
  }
}

static void rpn_print_binary_expr(BinaryExpr* expr) {
  rpn_print_expr(expr->left);
  printf(" ");
  rpn_print_expr(expr->right);
  printf(" ");
  print_token_lexeme(expr->operator->lexeme);
}

static void rpn_print_unary_expr(UnaryExpr* expr) {
  rpn_print_expr(expr->right);
  printf(" ");
  print_token_lexeme(expr->operator->lexeme);
}

static void rpn_print_literal_expr(LiteralExpr* expr) {
  assert(expr->literal_token != NULL);
  print_literal_token(*expr->literal_token);
}

static void rpn_print_ternary_expr(TernaryExpr* expr) {
  rpn_print_expr(expr->condition);
  printf(" ");
  rpn_print_expr(expr->expr_if_true);
  printf(" ");
  rpn_print_expr(expr->expr_if_false);
  printf(" ");
  printf("?:");
}

static void rpn_print_var_expr(VarExpr* expr) {
  print_token_lexeme(expr->name->lexeme);
  printf(" ");
  printf("var");
}

static void rpn_print_assignment_expr(AssignmentExpr* expr) {
  print_token_lexeme(expr->name->lexeme);
  printf(" ");
  rpn_print_expr(expr->value);
  printf(" ");
  printf("=");
}