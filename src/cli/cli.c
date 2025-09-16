#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../lox/scanner.h"
#include "../lox/token.h"
#include "../utils/error.h"
#include "../utils/list.h"
#include "cli.h"
#include "file.h"

void report_syntax_error(Token token);

#define STR_EQ(left, right) strcmp(left, right) == 0

Command parse_args(int argc, char* argv[]) {
  if (argc < 2) {
    fprintf(stderr, "Missing command\n");
    return NEW_ERR_COMMAND;
  }

  const char* cmd = argv[1];

  if (STR_EQ(cmd, "help")) {
    return NEW_HELP_COMMAND;
  } else if (STR_EQ(cmd, "tokenize")) {
    if (argc < 3) {
      fprintf(stderr, "Missing source file path\n");
      return NEW_ERR_COMMAND;
    }
    return NEW_COMMAND(COMMAND_TOKENIZE, argv[2]);
  } else {
    fprintf(stderr, "Uknown command \"%s\"\n", cmd);
    return NEW_ERR_COMMAND;
  }
}

CommandResult help_cmd(const char* lox_program_name) {
#define TAB "   "
  printf("Usage: %s <COMMAND> [ARGS]\n", lox_program_name);
  printf("\n");
  printf("Commands: \n");
  printf(TAB "help\n");
  printf(TAB "tokenize <FILE>\n");

  return CMD_OK;
}

#define MIN_PRECISION 1
int num_precision(Token token) {
  int precision = MIN_PRECISION;

  char* dot = strchr(token.lexeme, '.');

  if (dot != NULL) {
    precision = token.length - (strchr(token.lexeme, '.') - token.lexeme) - 1;
  }

  return precision < MIN_PRECISION ? MIN_PRECISION : precision;
}

const char* token_to_string(TokenType type) {
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

void print_token(Token* token) {
  if (token->type == TOKEN_ERROR) {
    report_syntax_error(*token);
    return;
  }

  printf("%s %.*s ", token_to_string(token->type), (int)token->length, token->lexeme);
  switch (token->type) {
    case TOKEN_STRING:
      printf("%s", as_str_value(token->literal));
      break;
    case TOKEN_NUMBER:
      printf("%.*f", num_precision(*token), as_num_value(token->literal));
      break;
    case TOKEN_TRUE:
      printf("true");
      break;
    case TOKEN_FALSE:
      printf("false");
      break;
    default:
      printf("null");
      break;
  }
  printf("\n");
}

CommandResult tokenize_cmd(const char* file_path) {
  char* file_contents = read_file_contents(file_path);

  Scanner scanner = new_scanner(file_contents);

  List tokens = scan_tokens(&scanner);

  list_foreach(tokens, (Iterator)print_token);

  free_list(&tokens, (Iterator)free_token);
  free(file_contents);

  return has_scanner_error(scanner) ? CMD_SYNTAX_ERR : CMD_OK;
}

void report_syntax_error(Token token) {
  assert(token.type == TOKEN_ERROR);
  fprintf(stderr, "[line %zu] Error: %s\n", token.line, as_str_value(token.literal));
}