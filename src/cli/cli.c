#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../lox/interpreter.h"
#include "../lox/parser.h"
#include "../lox/scanner.h"
#include "../lox/token.h"
#include "../utils/list.h"
#include "cli.h"
#include "file.h"
#include "printers.h"

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
  } else if (STR_EQ(cmd, "parse")) {
    if (argc < 3) {
      fprintf(stderr, "Missing source file path\n");
      return NEW_ERR_COMMAND;
    }
    return NEW_COMMAND(COMMAND_PARSE, argv[2]);
  } else if (STR_EQ(cmd, "evaluate")) {
    if (argc < 3) {
      fprintf(stderr, "Missing source file path\n");
      return NEW_ERR_COMMAND;
    }
    return NEW_COMMAND(COMMAND_EVALUATE, argv[2]);
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

CommandResult tokenize_cmd(const char* file_path) {
  char* file_contents = read_file_contents(file_path);

  Scanner scanner = new_scanner(file_contents);

  List tokens = scan_tokens(&scanner);

  list_foreach(tokens, (Iterator)print_token);

  free_list(&tokens, (Iterator)free_token);
  free(file_contents);

  return !has_scanner_error(scanner) ? CMD_OK : CMD_SYNTAX_ERR;
}

CommandResult parse_cmd(const char* file_path) {
  char* file_contents = read_file_contents(file_path);

  Scanner scanner = new_scanner(file_contents);

  List tokens = scan_tokens(&scanner);

  Parser parser = new_parser(tokens);

  Expr* output = NULL;
  SyntaxError err;

  ParseResult res = parse_expr(&parser, &output, &err);

  if (res == PARSE_OK) {
    println_expr(output);
    free_expr(output);
  } else {
    report_syntax_error(err);
    free_syntax_err(err);
  }

  free_list(&tokens, (Iterator)free_token);
  free(file_contents);

  return res == PARSE_OK ? CMD_OK : CMD_SYNTAX_ERR;
}

CommandResult evaluate_cmd(FilePath file_path) {
  char* file_contents = read_file_contents(file_path);

  Scanner scanner = new_scanner(file_contents);
  List tokens = scan_tokens(&scanner);

  Parser parser = new_parser(tokens);
  Expr* parsed_expr = NULL;
  SyntaxError syntax_err;

  ParseResult parse_res = parse_expr(&parser, &parsed_expr, &syntax_err);

  if (parse_res != PARSE_OK) {
    report_syntax_error(syntax_err);

    free_syntax_err(syntax_err);
    free_list(&tokens, (Iterator)free_token);
    free(file_contents);

    return CMD_SYNTAX_ERR;
  }

  Value output_value;
  RuntimeError runtime_err;

  InterpreterResult inter_res = evaluate(parsed_expr, &output_value, &runtime_err);

  if (inter_res == INTERPRETER_OK) {
    print_value(output_value);
    free_value(output_value);
  } else {
    report_runtime_error(runtime_err);
    free_runtime_err(runtime_err);
  }

  free_expr(parsed_expr);
  free_list(&tokens, (Iterator)free_token);
  free(file_contents);

  return inter_res == INTERPRETER_OK ? CMD_OK : CMD_RUNTIME_ERR;
}