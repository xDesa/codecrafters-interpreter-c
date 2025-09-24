#ifndef CLOX_CLI_H
#define CLOX_CLI_H

typedef enum {
  COMMAND_ERR,
  COMMAND_HELP,
  COMMAND_TOKENIZE,
  COMMAND_PARSE,
  COMMAND_EVALUATE,
} CommandType;

typedef const char* FilePath;

typedef struct {
  CommandType type;
  FilePath file_path;
} Command;

typedef enum {
  CMD_OK,
  CMD_FORMAT_ERR,
  CMD_SYNTAX_ERR,
  CMD_RUNTIME_ERR,
} CommandResult;

#define NEW_COMMAND(type, path) \
  (Command) { type, path }

#define NEW_HELP_COMMAND NEW_COMMAND(COMMAND_HELP, NULL)
#define NEW_ERR_COMMAND NEW_COMMAND(COMMAND_ERR, NULL)

Command parse_args(int argc, char* argv[]);

CommandResult help_cmd(const char* lox_program_name);

CommandResult tokenize_cmd(FilePath file_path);

CommandResult parse_cmd(FilePath file_path);

CommandResult evaluate_cmd(FilePath file_path);

#endif /* CLOX_CLI_H */