#ifndef __lib_libc_sprintf_include__
#define __lib_libc_sprintf_include__

/// @todo first step to implement stdio or something like that

/// @todo did wie have to implement stdarg.h or cstdarg or float/double ?
#include <stdarg.h>
//#include <cstdarg>

/**
 * Write formatted data to string.
 * Composes a string with the same text that would be printed if format
 * was used on printf, but instead of being printed, the content is
 * stored as a C string in the buffer pointed by str.
 * 
 * The size of the buffer should be large enough to contain the entire
 * resulting string.
 * 
 * A terminating null character is automatically appended after the content.
 * 
 * After the format parameter, the function expects at least as many
 * additional arguments as needed for format.
 * 
 * @param str       Pointer to a buffer where the resulting C-string is stored. The buffer should be large enough to contain the resulting string.
 * @param format    C string that contains a format string that follows the same specifications as format in printf.
 * @param ...       Depending on the format string, the function may expect a sequence of additional arguments.
 * 
 * @return On success, the total number of characters written is returned. This count does not include the additional null-character automatically appended at the end of the string.
 * 
 * @todo On failure, a negative number is returned (with the size of characters, that were printable).
 * @todo implement float numbers and e.g. 10.2f (float 10 chars with 2 after the dot
 */

int sprintf ( char * str, const char * format, ... );
int sprintf ( char * str, const char * format, va_list &args);

#endif
