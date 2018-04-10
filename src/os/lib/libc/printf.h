#ifndef __lib_libc_printf_include__
#define __lib_libc_printf_include__

/// @todo first step to implement stdio or something like that

/// @todo did wie have to implement stdarg.h or cstdarg or float/double ?
#include <stdarg.h>
#include "lib/OutputStream.h"
#include "kernel/lock/Spinlock.h"
#include <devices/graphics/text/CgaText.h>

#ifdef __cplusplus
extern "C" {
#endif

extern Spinlock *printLock;
extern OutputStream *stdout;

/**
 * Write formatted data on kout or OutputStream.
 * After the format parameter, the function expects at least as many
 * additional arguments as needed for format.
 * 
 * @param format    C string that contains a format string that follows the same specifications as format in printf.
 * @param ...       Depending on the format string, the function may expect a sequence of additional arguments.
 * 
 * @return On success, the total number of characters written is returned. This count does not include the additional null-character automatically appended at the end of the string.
 * 
 */
void printf ( const char * format, ... );
void printfUpdateStdout();

#ifdef __cplusplus
}
#endif

#endif
