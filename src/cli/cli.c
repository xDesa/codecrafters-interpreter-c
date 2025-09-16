#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../lox/scanner.h"
#include "../lox/token.h"
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

void print_token(Token* token) {
  if (token->type == TOKEN_ERROR) {
    report_syntax_error(*token);
    return;
  }

  printf("%s %.*s ", TOKEN_NAMES[token->type], (int)token->length, token->lexeme);
  switch (token->type) {
    case TOKEN_STRING:
      printf("%s", token->literal.str);
      break;
    case TOKEN_NUMBER:
      printf("%.*f", num_precision(*token), token->literal.num);
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
  fprintf(stderr, "[line %zu] Error: %s\n", token.line, token.literal.error);
}