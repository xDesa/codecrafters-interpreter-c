#include "parser.h"
#include <stdio.h>
#include "error.h"
#include "expr.h"
#include "stmt.h"
#include "token.h"

static inline Token* peek(Parser* parser);
static inline bool check(Parser* parser, TokenType type);
static Token* advance(Parser* parser);
static Token* consume(Parser* parser, TokenType type);
static inline bool is_parser_at_end(Parser* parser);
static void synchronize(Parser* parser) __attribute__((unused));

static ParseResult expression(Parser* parser, Expr** output, SyntaxError** err);
static ParseResult assignment(Parser* parser, Expr** output, SyntaxError** err);
static ParseResult ternary(Parser* parser, Expr** output, SyntaxError** err);
static ParseResult equality(Parser* parser, Expr** output, SyntaxError** err);
static ParseResult comparison(Parser* parser, Expr** output, SyntaxError** err);
static ParseResult term(Parser* parser, Expr** output, SyntaxError** err);
static ParseResult factor(Parser* parser, Expr** output, SyntaxError** err);
static ParseResult unary(Parser* parser, Expr** output, SyntaxError** err);
static ParseResult primary(Parser* parser, Expr** output, SyntaxError** err);

static ParseResult declaration(Parser* parser, Stmt** output, SyntaxError** err);
static ParseResult var_declaration(Parser* parser, Stmt** output, SyntaxError** err);
static ParseResult statement(Parser* parser, Stmt** output, SyntaxError** err);
static ParseResult print_stmt(Parser* parser, Stmt** output, SyntaxError** err);
static ParseResult expr_stmt(Parser* parser, Stmt** output, SyntaxError** err);
static ParseResult block_stmt(Parser* parser, Stmt** output, SyntaxError** err);
static ParseResult if_stmt(Parser* parser, Stmt** output, SyntaxError** err);

static ParseResult block(Parser* parser, List* stmts, SyntaxError** err);

#define TRY_PARSE(parser, parse_fn, err, before_return...)                \
  ({                                                                      \
    Expr* __internal_expr;                                                \
    ParseResult __internal_res = parse_fn(parser, &__internal_expr, err); \
    if (__internal_res != PARSE_OK) {                                     \
      before_return;                                                      \
      return __internal_res;                                              \
    }                                                                     \
    __internal_expr;                                                      \
  })

#define TRY_PARSE_STMT(parser, parse_fn, err, before_return...)           \
  ({                                                                      \
    Stmt* __internal_stmt;                                                \
    ParseResult __internal_res = parse_fn(parser, &__internal_stmt, err); \
    if (__internal_res != PARSE_OK) {                                     \
      before_return;                                                      \
      return __internal_res;                                              \
    }                                                                     \
    __internal_stmt;                                                      \
  })

#define OUTPUT(dest, src) \
  ({                      \
    if (dest != NULL) {   \
      *dest = src;        \
    }                     \
    PARSE_OK;             \
  })

#define SYNTAX_ERROR(dest, src) \
  ({                            \
    if (dest != NULL) {         \
      *dest = src;              \
    }                           \
    SYNTAX_ERROR;               \
  })

ParseResult parse(Parser* parser, List* output, List* errors) {
  while (!is_parser_at_end(parser)) {
    Stmt* stmt;
    SyntaxError* err;

    if (declaration(parser, &stmt, &err) == PARSE_OK) {
      list_append(output, stmt);
    } else {
      list_append(errors, err);
      synchronize(parser);
    }
  }

  if (errors->size > 0) {
    return SYNTAX_ERROR;
  }

  return PARSE_OK;
}

static ParseResult declaration(Parser* parser, Stmt** output, SyntaxError** err) {
  if (consume(parser, TOKEN_VAR) != NULL) {
    return var_declaration(parser, output, err);
  }

  return statement(parser, output, err);
}

static ParseResult var_declaration(Parser* parser, Stmt** output, SyntaxError** err) {
  Token* name = consume(parser, TOKEN_IDENTIFIER);

  if (name == NULL) {
    return SYNTAX_ERROR(err, new_syntax_err(peek(parser), "Expect variable name."));
  }

  Expr* initializer = NULL;
  if (consume(parser, TOKEN_EQUAL) != NULL) {
    initializer = TRY_PARSE(parser, expression, err);
  }

  if (consume(parser, TOKEN_SEMICOLON) == NULL) {
    free_expr(initializer);
    return SYNTAX_ERROR(err, new_syntax_err(peek(parser), "Expect ';' after value."));
  }

  return OUTPUT(output, new_var_decl_stmt(name, initializer));
}

static ParseResult statement(Parser* parser, Stmt** output, SyntaxError** err) {
  if (consume(parser, TOKEN_PRINT) != NULL) {
    return print_stmt(parser, output, err);
  }

  if (consume(parser, TOKEN_LEFT_BRACE) != NULL) {
    return block_stmt(parser, output, err);
  }

  if (consume(parser, TOKEN_IF) != NULL) {
    return if_stmt(parser, output, err);
  }

  return expr_stmt(parser, output, err);
}

static ParseResult print_stmt(Parser* parser, Stmt** output, SyntaxError** err) {
  Expr* expr = TRY_PARSE(parser, expression, err);

  if (consume(parser, TOKEN_SEMICOLON) == NULL) {
    free_expr(expr);
    return SYNTAX_ERROR(err, new_syntax_err(peek(parser), "Expect ';' after value."));
  }

  return OUTPUT(output, new_print_stmt(expr));
}

static ParseResult expr_stmt(Parser* parser, Stmt** output, SyntaxError** err) {
  Expr* expr = TRY_PARSE(parser, expression, err);

  if (consume(parser, TOKEN_SEMICOLON) == NULL) {
    free_expr(expr);
    return SYNTAX_ERROR(err, new_syntax_err(peek(parser), "Expect ';' after value."));
  }

  return OUTPUT(output, new_expr_stmt(expr));
}

static ParseResult block_stmt(Parser* parser, Stmt** output, SyntaxError** err) {
  List block_stmts = new_list();

  if (block(parser, &block_stmts, err) == SYNTAX_ERROR) {
    free_list(&block_stmts, (Iterator)free_stmt);
    return SYNTAX_ERROR;
  }

  return OUTPUT(output, new_block_stmt(block_stmts));
}

static ParseResult block(Parser* parser, List* stmts, SyntaxError** err) {
  while (!check(parser, TOKEN_RIGHT_BRACE) && !is_parser_at_end(parser)) {
    list_append(stmts, TRY_PARSE_STMT(parser, declaration, err));
  }

  if (consume(parser, TOKEN_RIGHT_BRACE) == NULL) {
    return SYNTAX_ERROR(err, new_syntax_err(peek(parser), "Expect '}' after block."));
  }

  return PARSE_OK;
}

static ParseResult if_stmt(Parser* parser, Stmt** output, SyntaxError** err) {
  if (consume(parser, TOKEN_LEFT_PAREN) == NULL) {
    return SYNTAX_ERROR(err, new_syntax_err(peek(parser), "Expect '(' after 'if'."));
  }

  Expr* condition = TRY_PARSE(parser, expression, err);

  if (consume(parser, TOKEN_RIGHT_PAREN) == NULL) {
    return SYNTAX_ERROR(err, new_syntax_err(peek(parser), "Expect ')' after if condition."));
  }

  Stmt* then_branch = TRY_PARSE_STMT(parser, statement, err, {
    free_expr(condition);
  });

  Stmt* else_branch = NULL;

  if (consume(parser, TOKEN_ELSE) != NULL) {
    else_branch = TRY_PARSE_STMT(parser, statement, err, {
      free_expr(condition);
      free_stmt(then_branch);
    });
  }

  return OUTPUT(output, new_if_stmt(condition, then_branch, else_branch));
}

ParseResult parse_expr(Parser* parser, Expr** output, SyntaxError** err) {
  return expression(parser, output, err);
}

static ParseResult expression(Parser* parser, Expr** output, SyntaxError** err) {
  Expr* left = TRY_PARSE(parser, assignment, err);

  while (check(parser, TOKEN_COMMA)) {
    Token* comma = advance(parser);
    Expr* right = TRY_PARSE(parser, assignment, err, {
      free_expr(left);
    });
    left = new_binary_expr(left, comma, right);
  }

  return OUTPUT(output, left);
}

static ParseResult assignment(Parser* parser, Expr** output, SyntaxError** err) {
  Expr* expr = TRY_PARSE(parser, ternary, err);

  if (check(parser, TOKEN_EQUAL)) {
    Token* equals = advance(parser);

    Expr* value = TRY_PARSE(parser, assignment, err, {
      free_expr(expr);
    });

    if (is_expr_type(expr, EXPR_VAR)) {
      Token* name = as_var_expr(expr)->name;
      free_expr(expr);

      return OUTPUT(output, new_assignment_expr(name, value));
    }

    return SYNTAX_ERROR(err, new_syntax_err(equals, "Invalid assignment target."));
  }

  return OUTPUT(output, expr);
}

static ParseResult ternary(Parser* parser, Expr** output, SyntaxError** err) {
  Expr* condition = TRY_PARSE(parser, equality, err);

  if (consume(parser, TOKEN_QUESTION_MARK) != NULL) {
    Expr* expr_if_true = TRY_PARSE(parser, expression, err, {
      free_expr(condition);
    });

    if (consume(parser, TOKEN_COLON) == NULL) {
      free_expr(condition);
      free_expr(expr_if_true);
      return SYNTAX_ERROR(err, new_syntax_err(peek(parser), "Expect ':' after expression."));
    }

    Expr* expr_if_false = TRY_PARSE(parser, ternary, err, {
      free_expr(condition);
      free_expr(expr_if_true);
    });

    return OUTPUT(output, new_ternary_expr(condition, expr_if_true, expr_if_false));
  }

  return OUTPUT(output, condition);
}

static ParseResult equality(Parser* parser, Expr** output, SyntaxError** err) {
  Expr* left = TRY_PARSE(parser, comparison, err);

  while (check(parser, TOKEN_BANG_EQUAL) || check(parser, TOKEN_EQUAL_EQUAL)) {
    Token* operator = advance(parser);
    Expr* right = TRY_PARSE(parser, comparison, err, {
      free_expr(left);
    });

    left = new_binary_expr(left, operator, right);
  }

  return OUTPUT(output, left);
}

static ParseResult comparison(Parser* parser, Expr** output, SyntaxError** err) {
  Expr* left = TRY_PARSE(parser, term, err);

  while (
      check(parser, TOKEN_GREATER)
      || check(parser, TOKEN_GREATER_EQUAL)
      || check(parser, TOKEN_LESS)
      || check(parser, TOKEN_LESS_EQUAL)) {
    Token* operator = advance(parser);
    Expr* right = TRY_PARSE(parser, term, err, {
      free_expr(left);
    });
    left = new_binary_expr(left, operator, right);
  }

  return OUTPUT(output, left);
}

static ParseResult term(Parser* parser, Expr** output, SyntaxError** err) {
  Expr* left = TRY_PARSE(parser, factor, err);

  while (
      check(parser, TOKEN_PLUS) || check(parser, TOKEN_MINUS)) {
    Token* operator = advance(parser);
    Expr* right = TRY_PARSE(parser, factor, err, {
      free_expr(left);
    });
    left = new_binary_expr(left, operator, right);
  }

  return OUTPUT(output, left);
}

static ParseResult factor(Parser* parser, Expr** output, SyntaxError** err) {
  Expr* left = TRY_PARSE(parser, unary, err);

  while (
      check(parser, TOKEN_STAR) || check(parser, TOKEN_SLASH)) {
    Token* operator = advance(parser);
    Expr* right = TRY_PARSE(parser, unary, err, {
      free_expr(left);
    });
    left = new_binary_expr(left, operator, right);
  }

  return OUTPUT(output, left);
}

static ParseResult unary(Parser* parser, Expr** output, SyntaxError** err) {
  if (check(parser, TOKEN_MINUS) || check(parser, TOKEN_BANG)) {
    Token* operator = advance(parser);
    Expr* right = TRY_PARSE(parser, unary, err);
    return OUTPUT(output, new_unary_expr(operator, right));
  }

  return OUTPUT(output, TRY_PARSE(parser, primary, err, {}));
}

static ParseResult primary(Parser* parser, Expr** output, SyntaxError** err) {
  Token* token = advance(parser);

  if (is_token_literal_value(token)) {
    return OUTPUT(output, new_literal_expr(token));
  }

  if (token->type == TOKEN_IDENTIFIER) {
    return OUTPUT(output, new_var_expr(token));
  }

  if (token->type == TOKEN_LEFT_PAREN) {
    Expr* expr = TRY_PARSE(parser, expression, err);

    if (consume(parser, TOKEN_RIGHT_PAREN) == NULL) {
      return SYNTAX_ERROR(err, new_syntax_err(peek(parser), "Expect ')' after expression."));
    }

    return OUTPUT(output, new_grouping_expr(expr));
  }

  if (token->type == TOKEN_ERROR) {
    return SYNTAX_ERROR(err, new_syntax_err(token, NULL));
  }

  return SYNTAX_ERROR(err, new_syntax_err(token, "Expect expression."));
}

static inline Token* peek(Parser* parser) {
  return (Token*)parser->current->data;
}

static inline bool check(Parser* parser, TokenType type) {
  return peek(parser)->type == type;
}

static Token* advance(Parser* parser) {
  Token* curr = peek(parser);
  parser->current = parser->current->next;
  return curr;
}

static Token* consume(Parser* parser, TokenType type) {
  if (!check(parser, type)) {
    return NULL;
  }

  return advance(parser);
}

static inline bool is_parser_at_end(Parser* parser) {
  return check(parser, TOKEN_EOF);
}

static void synchronize(Parser* parser) {
  while (!is_parser_at_end(parser)) {
    switch (peek(parser)->type) {
      case TOKEN_SEMICOLON:
      case TOKEN_CLASS:
      case TOKEN_FUN:
      case TOKEN_VAR:
      case TOKEN_FOR:
      case TOKEN_IF:
      case TOKEN_WHILE:
      case TOKEN_PRINT:
      case TOKEN_RETURN:
        return;
      default:
        advance(parser);
        break;
    }
  }
}