#ifndef CLOX_SCANNER_H
#define CLOX_SCANNER_H

#include "token.h"

typedef struct {
  const char* source;
  const char* start;
  const char* current;
  size_t line;
  bool has_error;
} Scanner;

static inline Scanner new_scanner(const char* source) {
  return ((Scanner) { source, source, source, 1, false });
}

static inline bool has_scanner_error(Scanner scanner) {
  return scanner.has_error;
}

TokenVec scan_tokens(Scanner* scanner);

#endif /* CLOX_SCANNER_H */