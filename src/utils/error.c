#include "error.h"
#include <stdio.h>

#include "mem.h"

char* vcreate_error_msg(const char* format, va_list args) {
  char buff[ERROR_MSG_BUFFER_LENGTH];

  vsnprintf(buff, sizeof(buff), format, args);

  return xstrdup(buff);
}