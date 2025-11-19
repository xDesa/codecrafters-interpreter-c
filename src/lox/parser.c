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

static ParseResult expression(Parser* parser, Expr** output, SyntaxError* err);
static ParseResult ternary(Parser* parser, Expr** output, SyntaxError* err);
static ParseResult equality(Parser* parser, Expr** output, SyntaxError* err);
static ParseResult comparison(Parser* parser, Expr** output, SyntaxError* err);
static ParseResult term(Parser* parser, Expr** output, SyntaxError* err);
static ParseResult factor(Parser* parser, Expr** output, SyntaxError* err);
static ParseResult unary(Parser* parser, Expr** output, SyntaxError* err);
static ParseResult primary(Parser* parser, Expr** output, SyntaxError* err);

static ParseResult statement(Parser* parser, Stmt** output, SyntaxError* err);
static ParseResult print_stmt(Parser* parser, Stmt** output, SyntaxError* err);
static ParseResult expr_stmt(Parser* parser, Stmt** output, SyntaxError* err);

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
  List stmt_list = new_list();
  List errors_list = new_list();

  while (!is_parser_at_end(parser)) {
    Stmt* stmt;
    SyntaxError err;

    if (statement(parser, &stmt, &err) == PARSE_OK) {
      list_append(&stmt_list, stmt);
    } else {
      SyntaxError* heap_err = xmalloc(sizeof(SyntaxError));
      *heap_err = err;
      list_append(&errors_list, heap_err);
    }
  }

  if (errors_list.size > 0) {
    return SYNTAX_ERROR(errors, errors_list);
  }

  return OUTPUT(output, stmt_list);
}

static ParseResult statement(Parser* parser, Stmt** output, SyntaxError* err) {
  if (consume(parser, TOKEN_PRINT) != NULL) {
    return print_stmt(parser, output, err);
  }

  return expr_stmt(parser, output, err);
}

static ParseResult print_stmt(Parser* parser, Stmt** output, SyntaxError* err) {
  Expr* expr = TRY_PARSE(parser, expression, err);

  if (consume(parser, TOKEN_SEMICOLON) == NULL) {
    free_expr(expr);
    return SYNTAX_ERROR(err, new_syntax_err(peek(parser), "Expect ';' after value."));
  }

  return OUTPUT(output, new_print_stmt(expr));
}

static ParseResult expr_stmt(Parser* parser, Stmt** output, SyntaxError* err) {
  Expr* expr = TRY_PARSE(parser, expression, err);

  if (consume(parser, TOKEN_SEMICOLON) == NULL) {
    free_expr(expr);
    return SYNTAX_ERROR(err, new_syntax_err(peek(parser), "Expect ';' after value."));
  }

  return OUTPUT(output, new_expr_stmt(expr));
}

ParseResult parse_expr(Parser* parser, Expr** output, SyntaxError* err) {
  return expression(parser, output, err);
}

static ParseResult expression(Parser* parser, Expr** output, SyntaxError* err) {
  Expr* left = TRY_PARSE(parser, ternary, err);

  while (check(parser, TOKEN_COMMA)) {
    Token* comma = advance(parser);
    Expr* right = TRY_PARSE(parser, ternary, err, {
      free_expr(left);
    });
    left = new_binary_expr(left, comma, right);
  }

  return OUTPUT(output, left);
}

static ParseResult ternary(Parser* parser, Expr** output, SyntaxError* err) {
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

static ParseResult equality(Parser* parser, Expr** output, SyntaxError* err) {
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

static ParseResult comparison(Parser* parser, Expr** output, SyntaxError* err) {
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

static ParseResult term(Parser* parser, Expr** output, SyntaxError* err) {
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

static ParseResult factor(Parser* parser, Expr** output, SyntaxError* err) {
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

static ParseResult unary(Parser* parser, Expr** output, SyntaxError* err) {
  if (check(parser, TOKEN_MINUS) || check(parser, TOKEN_BANG)) {
    Token* operator = advance(parser);
    Expr* right = TRY_PARSE(parser, unary, err);
    return OUTPUT(output, new_unary_expr(operator, right));
  }

  return OUTPUT(output, TRY_PARSE(parser, primary, err, {}));
}

static ParseResult primary(Parser* parser, Expr** output, SyntaxError* err) {
  Token* token = advance(parser);

  if (is_token_literal_value(token)) {
    return OUTPUT(output, new_literal_expr(token));
  } else if (token->type == TOKEN_LEFT_PAREN) {
    Expr* expr = TRY_PARSE(parser, expression, err);

    if (consume(parser, TOKEN_RIGHT_PAREN) == NULL) {
      return SYNTAX_ERROR(err, new_syntax_err(peek(parser), "Expect ')' after expression."));
    }

    return OUTPUT(output, new_grouping_expr(expr));
  } else if (token->type == TOKEN_ERROR) {
    return SYNTAX_ERROR(err, new_syntax_err(token, NULL));
  } else {
    return SYNTAX_ERROR(err, new_syntax_err(token, "Expect expression."));
  }
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