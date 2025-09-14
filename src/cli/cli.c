#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cli.h"
#include "file.h"

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

CommandResult tokenize_cmd(const char* file_path) {
  char* file_contents = read_file_contents(file_path);

  if (strlen(file_contents) > 0) {
    fprintf(stderr, "Scanner not implemented\n");
    free(file_contents);
    return CMD_SYNTAX_ERR;
  }
  printf("EOF  null\n"); // Placeholder, replace this line when implementing the scanner

  free(file_contents);
  return CMD_OK;
}