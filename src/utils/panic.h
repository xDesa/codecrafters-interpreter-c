#ifndef CLOX_PANIC_H
#define CLOX_PANIC_H

#include <err.h>
#include <sysexits.h>

#define panic(message) (errx(EX_SOFTWARE, "Error: program panic with message '%s' in %s at %s:%d", message, __FUNCTION__, __FILE__, __LINE__))

#define unreachable_code() (panic("reach unreachable code"))

#endif /* CLOX_PANIC_H */