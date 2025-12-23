#ifndef CLOX_PARSER_H
#define CLOX_PARSER_H

#include "error.h"
#include "expr.h"
#include "stmt.h"
#include "token.h"

typedef struct {
  TokenVec tokens;
  Token* current;
} Parser;

typedef enum {
  PARSE_OK,
  SYNTAX_ERROR,
} ParseResult;

static inline Parser new_parser(TokenVec tokens) {
  return (Parser) { tokens, tokens.items };
}

typedef VecType(SyntaxError) ParseErrors;
#define free_parse_errors(vec) \
  free_vec_with(vec, SyntaxError, free_syntax_err)

ParseResult parse(Parser* parser, StmtVec* output, ParseErrors* errors);
ParseResult parse_expr(Parser* parser, Expr** output, SyntaxError* err);

#endif /* CLOX_PARSER_H */