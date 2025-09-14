#ifndef CLOX_ERROR_H
#define CLOX_ERROR_H

#include <err.h>
#include <sysexits.h>

#include "common.h"

#ifndef ERROR_MSG_BUFFER_LENGTH
#define ERROR_MSG_BUFFER_LENGTH 1024
#endif

#define unreachable_code() (errx(EX_SOFTWARE, "Error: reach unreachable code in %s at %s:%d", __FUNCTION__, __FILE__, __LINE__))

char* vcreate_error_msg(const char* format, va_list args);

#endif /* CLOX_ERROR_H */