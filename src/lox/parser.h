#ifndef CLOX_PARSER_H
#define CLOX_PARSER_H

#include "../utils/list.h"
#include "error.h"
#include "expr.h"

typedef struct {
  List tokens;
  ListNode* current;
} Parser;

typedef enum {
  PARSE_OK,
  SYNTAX_ERROR,
} ParseResult;

static inline Parser new_parser(List tokens) {
  return (Parser) { tokens, tokens.head };
}

ParseResult parse(Parser* parser, List* output, List* errors);
ParseResult parse_expr(Parser* parser, Expr** output, SyntaxError** err);

#endif /* CLOX_PARSER_H */