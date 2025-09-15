#include "scanner.h"
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "token.h"

static Token* scan_token(Scanner* scanner);
static void skip_whitespaces_and_comments(Scanner* scanner);
static Token* string(Scanner* scanner);
static Token* number(Scanner* scanner);
static Token* identifier(Scanner* scanner);

static inline bool is_scanner_at_end(Scanner* scanner);
static inline void start_scanner(Scanner* scanner);
static char advance(Scanner* scanner);
static char peek(Scanner* scanner);
static char peek_next(Scanner* scanner);
static bool match(Scanner* scanner, char expected);
static inline char first(Scanner* scanner);
static inline char first_next(Scanner* scanner);

static inline bool is_digit(char c);
static inline bool is_alphabetic(char c);
static inline bool is_alphanumeric(char c);

static inline size_t token_length(Scanner* scanner);
static Token* create_token(Scanner* scanner, TokenType type, TokenLiteral literal);
static inline Token* create_nil_token(Scanner* scanner, TokenType type);

List scan_tokens(Scanner* scanner) {
  List tokens = new_list();

  while (!is_scanner_at_end(scanner)) {
    list_append(&tokens, scan_token(scanner));
  }

  return tokens;
}

static Token* scan_token(Scanner* scanner) {
  skip_whitespaces_and_comments(scanner);

  start_scanner(scanner);

  if (is_scanner_at_end(scanner)) {
    return create_nil_token(scanner, TOKEN_EOF);
  }

  const char curr = advance(scanner);

  if (is_digit(curr)) {
    return number(scanner);
  } else if (is_alphabetic(curr)) {
    return identifier(scanner);
  }

  switch (curr) {
    case '(':
      return create_nil_token(scanner, TOKEN_LEFT_PAREN);
    case ')':
      return create_nil_token(scanner, TOKEN_RIGHT_PAREN);
    case '{':
      return create_nil_token(scanner, TOKEN_LEFT_BRACE);
    case '}':
      return create_nil_token(scanner, TOKEN_RIGHT_BRACE);
    case ',':
      return create_nil_token(scanner, TOKEN_COMMA);
    case ';':
      return create_nil_token(scanner, TOKEN_SEMICOLON);
    case '.':
      return create_nil_token(scanner, TOKEN_DOT);
    case '+':
      return create_nil_token(scanner, TOKEN_PLUS);
    case '-':
      return create_nil_token(scanner, TOKEN_MINUS);
    case '*':
      return create_nil_token(scanner, TOKEN_STAR);
    case '/':
      return create_nil_token(scanner, TOKEN_SLASH);
    case '=':
      return create_nil_token(scanner, match(scanner, '=') ? TOKEN_EQUAL_EQUAL : TOKEN_EQUAL);
    case '!':
      return create_nil_token(scanner, match(scanner, '=') ? TOKEN_BANG_EQUAL : TOKEN_BANG);
    case '<':
      return create_nil_token(scanner, match(scanner, '=') ? TOKEN_LESS_EQUAL : TOKEN_LESS);
    case '>':
      return create_nil_token(scanner, match(scanner, '=') ? TOKEN_GREATER_EQUAL : TOKEN_GREATER);
    case '\"':
      return string(scanner);
    default:
      return create_token(scanner, TOKEN_ERROR, new_err_literal("Unexpected character: '%c'", curr));
  }
}

static void skip_whitespaces_and_comments(Scanner* scanner) {
  while (true) {
    switch (peek(scanner)) {
      case ' ':
      case '\r':
      case '\t':
      case '\n':
        advance(scanner);
        break;
      case '/':
        if (peek_next(scanner) == '/') {
          while (peek(scanner) != '\n' && !is_scanner_at_end(scanner)) {
            advance(scanner);
          }
        } else {
          // exit function, next lexeme is not a comment
          return;
        }
        break;
      default:
        // exit function, next lexeme is not whitespace or comment
        return;
    }
  }
}

static Token* string(Scanner* scanner) {
  while (peek(scanner) != '\"' && !is_scanner_at_end(scanner)) {
    advance(scanner);
  }

  if (is_scanner_at_end(scanner)) {
    return create_token(scanner, TOKEN_ERROR, new_err_literal("Unterminated string."));
  }

  advance(scanner); // consume closing double-quotes

  const char* str_literal_start = scanner->start + 1;
  size_t str_literal_length = token_length(scanner) - 2;

  return create_token(scanner, TOKEN_STRING, new_str_literal(str_literal_start, str_literal_length));
}

static Token* number(Scanner* scanner) {
  while (is_digit(peek(scanner))) {
    advance(scanner);
  }

  if (peek(scanner) == '.' && is_digit(peek_next(scanner))) {
    advance(scanner); // consume '.'
    while (is_digit(peek(scanner))) {
      advance(scanner);
    }
  }

  size_t num_length = token_length(scanner);

  char str_num[num_length + 1];

  strncpy(str_num, scanner->start, num_length);

  str_num[num_length] = '\0';

  double num = strtod(str_num, NULL);

  return create_token(scanner, TOKEN_NUMBER, new_num_literal(num));
}

static TokenType match_keyword_token_type(Scanner* scanner, const char* keyword, TokenType type) {
  size_t keyword_length = strlen(keyword);

  if (token_length(scanner) == keyword_length && memcmp(scanner->start, keyword, keyword_length) == 0) {
    return type;
  }

  return TOKEN_IDENTIFIER;
}

static TokenType identifier_type(Scanner* scanner) {
  switch (first(scanner)) {
    case 'a':
      return match_keyword_token_type(scanner, "and", TOKEN_AND);
    case 'c':
      return match_keyword_token_type(scanner, "class", TOKEN_CLASS);
    case 'e':
      return match_keyword_token_type(scanner, "else", TOKEN_ELSE);
    case 'f':
      if (token_length(scanner) > 1) {
        switch (first_next(scanner)) {
          case 'a':
            return match_keyword_token_type(scanner, "false", TOKEN_FALSE);
          case 'o':
            return match_keyword_token_type(scanner, "for", TOKEN_FOR);
          case 'u':
            return match_keyword_token_type(scanner, "fun", TOKEN_FUN);
        }
      }
      break;
    case 'i':
      return match_keyword_token_type(scanner, "if", TOKEN_IF);
    case 'n':
      return match_keyword_token_type(scanner, "nil", TOKEN_NIL);
    case 'o':
      return match_keyword_token_type(scanner, "or", TOKEN_OR);
    case 'p':
      return match_keyword_token_type(scanner, "print", TOKEN_PRINT);
    case 'r':
      return match_keyword_token_type(scanner, "return", TOKEN_RETURN);
    case 's':
      return match_keyword_token_type(scanner, "super", TOKEN_SUPER);
    case 't':
      if (token_length(scanner) > 1) {
        switch (first_next(scanner)) {
          case 'h':
            return match_keyword_token_type(scanner, "this", TOKEN_THIS);
          case 'r':
            return match_keyword_token_type(scanner, "true", TOKEN_TRUE);
        }
      }
      break;
    case 'v':
      return match_keyword_token_type(scanner, "var", TOKEN_VAR);
    case 'w':
      return match_keyword_token_type(scanner, "while", TOKEN_WHILE);
  }

  return TOKEN_IDENTIFIER;
}

static Token* identifier(Scanner* scanner) {
  while (is_alphanumeric(peek(scanner))) {
    advance(scanner);
  }

  return create_nil_token(scanner, identifier_type(scanner));
}

static inline bool is_scanner_at_end(Scanner* scanner) {
  return *scanner->current == '\0';
}

static inline void start_scanner(Scanner* scanner) {
  scanner->start = scanner->current;
}

static char advance(Scanner* scanner) {
  if (is_scanner_at_end(scanner)) {
    return '\0';
  }

  char curr_char = *scanner->current;
  scanner->current++;

  if (curr_char == '\n') {
    scanner->line++;
  }

  return curr_char;
}

static char peek(Scanner* scanner) {
  if (is_scanner_at_end(scanner)) {
    return '\0';
  }

  return *scanner->current;
}
static char peek_next(Scanner* scanner) {
  if (is_scanner_at_end(scanner)) {
    return '\0';
  }

  return *(scanner->current + 1);
}

static bool match(Scanner* scanner, char expected) {
  if (is_scanner_at_end(scanner) || peek(scanner) != expected) {
    return false;
  }

  advance(scanner);

  return true;
}

static inline char first(Scanner* scanner) {
  return *scanner->start;
}

static inline char first_next(Scanner* scanner) {
  return *(scanner->start + 1);
}

static inline bool is_digit(char c) {
  return c >= '0' && c <= '9';
}

static inline bool is_alphabetic(char c) {
  return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_';
}

static inline bool is_alphanumeric(char c) {
  return is_alphabetic(c) || is_digit(c);
}

static inline size_t token_length(Scanner* scanner) {
  return scanner->current - scanner->start;
}

static Token* create_token(Scanner* scanner, TokenType type, TokenLiteral literal) {
  if (type == TOKEN_ERROR) {
    scanner->has_error = true;
  }

  return new_token(type, literal, scanner->start, token_length(scanner), scanner->line);
}

static inline Token* create_nil_token(Scanner* scanner, TokenType type) {
  return create_token(scanner, type, new_nil_literal());
}