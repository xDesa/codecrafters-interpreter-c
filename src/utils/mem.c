#include "mem.h"

#include <err.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sysexits.h>

void* xmalloc(size_t size) {
  void* ptr = malloc(size);

  if (ptr == NULL) {
    err(EX_OSERR, "Unable to allocate memory with malloc");
  }

  return ptr;
}

void* xcalloc(size_t nmemb, size_t size) {
  void* ptr = calloc(nmemb, size);

  if (ptr == NULL) {
    err(EX_OSERR, "Unable to allocate memory with malloc");
  }

  return ptr;
}

void* xrealloc(void* ptr, size_t size) {
  ptr = realloc(ptr, size);

  if (ptr == NULL) {
    err(EX_OSERR, "Unable to reallocate memory with realloc");
  }

  return ptr;
}

void* xmemdup(const void* ptr, size_t size) {
  void* ptr2 = xmalloc(size);

  memcpy(ptr2, ptr, size);

  return ptr2;
}

char* xstrdup(const char* str) {
  char* str2 = strdup(str);

  if (str2 == NULL) {
    err(EX_OSERR, "Unable to duplicate string");
  }

  return str2;
}

char* xstrndup(const char* str, size_t len) {
  char* str2 = strndup(str, len);

  if (str2 == NULL) {
    err(EX_OSERR, "Unable to duplicate %zu characters of a string", len);
  }

  return str2;
}

char* xstrcat(const char* str1, const char* str2) {
  size_t str1_len = strlen(str1);
  size_t str2_len = strlen(str2);

  char* new_str = xmalloc(str1_len + str2_len + 1); // str1 + str2 + null char

  memset(new_str, 0, str1_len + str2_len + 1);

  strncpy(new_str, str1, str1_len);

  new_str[str1_len] = '\0'; // prevent garbage malloc data see as part of the new_str from strncat

  strncat(new_str, str2, str2_len);

  new_str[str1_len + str2_len] = '\0';

  return new_str;
}
