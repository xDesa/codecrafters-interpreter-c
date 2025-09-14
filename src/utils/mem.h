#ifndef CLOX_MEM_H
#define CLOX_MEM_H

#include "common.h"

void* xmalloc(size_t size) __attribute__((malloc));

void* xcalloc(size_t nmemb, size_t size) __attribute__((malloc));

void* xrealloc(void* ptr, size_t size) __attribute__((malloc));

void* xmemdup(const void* ptr, size_t size) __attribute__((malloc));

char* xstrdup(const char* str) __attribute__((malloc));

char* xstrndup(const char* str, size_t len) __attribute__((malloc));

char* xstrcat(const char* str1, const char* str2) __attribute__((malloc));

void xmemcpy(void* dest, void* src, size_t n);

#endif /* CLOX_MEM_H */