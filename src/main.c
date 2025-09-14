#include <stdio.h>
#include <stdlib.h>
#include <sysexits.h>

#include "cli/cli.h"
#include "utils/error.h"

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
    default:
      unreachable_code();
  }

  if (cmd_result == CMD_FORMAT_ERR) {
    return EX_USAGE;
  }
  if (cmd_result == CMD_SYNTAX_ERR) {
    return EX_DATAERR;
  }

  return EXIT_SUCCESS;
}
