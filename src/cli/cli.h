#ifndef CLOX_CLI_H
#define CLOX_CLI_H

typedef enum {
  COMMAND_ERR,
  COMMAND_HELP,
  COMMAND_TOKENIZE
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
} CommandResult;

#define NEW_COMMAND(type, path) \
  (Command) { type, path }

#define NEW_HELP_COMMAND NEW_COMMAND(COMMAND_HELP, NULL)
#define NEW_ERR_COMMAND NEW_COMMAND(COMMAND_ERR, NULL)

Command parse_args(int argc, char* argv[]);

CommandResult help_cmd(const char* lox_program_name);

// clang-format off
static const char* TOKEN_NAMES[] = {
  "LEFT_PAREN", "RIGHT_PAREN",   "LEFT_BRACE", "RIGHT_BRACE", "COMMA",
  "DOT",        "MINUS",         "PLUS",       "SEMICOLON",   "SLASH",
  "STAR",       "BANG",          "BANG_EQUAL", "EQUAL",       "EQUAL_EQUAL",
  "GREATER",    "GREATER_EQUAL", "LESS",       "LESS_EQUAL",  "IDENTIFIER",
  "STRING",     "NUMBER",        "AND",        "CLASS",       "ELSE",
  "FALSE",      "FOR",           "FUN",        "IF",          "NIL",
  "OR",         "PRINT",         "RETURN",     "SUPER",       "THIS",
  "TRUE",       "VAR",           "WHILE",      "ERROR",       "EOF"};
// clang-format on

CommandResult tokenize_cmd(FilePath file_path);

#endif /* CLOX_CLI_H */