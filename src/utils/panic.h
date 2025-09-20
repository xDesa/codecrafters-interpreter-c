#ifndef CLOX_PANIC_H
#define CLOX_PANIC_H

#include <err.h>
#include <sysexits.h>

#define unreachable_code() (errx(EX_SOFTWARE, "Error: reach unreachable code in %s at %s:%d", __FUNCTION__, __FILE__, __LINE__))

#endif /* CLOX_PANIC_H */