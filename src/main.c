#include <stdio.h>
#include <stdlib.h>
#include <sysexits.h>

#include "cli/cli.h"
#include "utils/panic.h"

int main(int argc, char* argv[]) {
  // Disable output buffering
  setbuf(stdout, NULL);
  setbuf(stderr, NULL);

  Command cmd = parse_args(argc, argv);

  CommandResult cmd_result;

  switch (cmd.type) {
    case COMMAND_ERR:
      help_cmd(argv[0]);
      cmd_result = CMD_FORMAT_ERR;
      break;
    case COMMAND_HELP:
      cmd_result = help_cmd(argv[0]);
      break;
    case COMMAND_TOKENIZE:
      cmd_result = tokenize_cmd(cmd.file_path);
      break;
    case COMMAND_PARSE:
      cmd_result = parse_cmd(cmd.file_path);
      break;
    case COMMAND_EVALUATE:
      cmd_result = evaluate_cmd(cmd.file_path);
      break;
    case COMMAND_RUN:
      cmd_result = run_cmd(cmd.file_path);
      break;
    default:
      unreachable_code();
  }

  switch (cmd_result) {
    case CMD_OK:
      return EXIT_SUCCESS;
    case CMD_FORMAT_ERR:
      return EX_USAGE; // exit code 64
    case CMD_SYNTAX_ERR:
      return EX_DATAERR; // exit code 65
    case CMD_RUNTIME_ERR:
      return EX_SOFTWARE; // exit code 70
  }

  return EXIT_SUCCESS;
}
