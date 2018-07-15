/*
 * Copyright (C) 2018 Burak Akguel, Christian Gesse, Fabian Ruhland, Filip Krakowski, Michael Schoettner
 * Heinrich-Heine University
 *
 * This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public
 * License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any
 * later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied
 * warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>
 */

/// @todo #include "lib/libc/snprintf.h" or #include <snprintf> ??
#include "snprintf.h"
#include "lib/StrFormat.h"

/// @todo did wie have to implement stdarg.h or cstdarg or float/double ?
#include <stdarg.h>

/// @todo #include "lib/libc/stdlib.h" or #include <stdlib.h> or <cstdlib>  ??
#include "lib/libc/stdlib.h" // size_t

static void _putc(char * strout, const char & character, int & n) {
    strout[n++] = character;
}

static void _nputs(char* strout, int nmax, const char* str, int & n) {
    for (unsigned int i = 0; str[i] != '\0'; ++i) {
        if ( n == (nmax-1) ) {
            // early terminating
            strout[n] = '\0';
            // written chars, but as negative number, because we want to write 1 more, but we can't
            n = -1 * (n-1); 
            return;
        }
        strout[n++] = str[i];
    }
    strout[n] = '\0';
}

static int _vsnprintf(char* strout, int nmax, const char* format, int & n, va_list * args) {
    const char* traverse;
    int i;
    char *s;
    int temp;
    unsigned char padding = 0;
    
    for(traverse = format; *traverse != '\0'; traverse++) {

        while( *traverse != '%' ) {

            if (*traverse == '\0') return n;

            if ( n == (nmax-1) ) {
                // there is a non \0 or non % char, but it could not be written (we use index nmax-1 to terminate string)

                // early terminating
                strout[n] = '\0';
                // negativ = fail
                n = -1 * (n-1); 
                return n;
            }
            _putc(strout, *traverse, n);
            traverse++;
        }

        traverse++;
        // if formating makes the string longer then nmax-1, we have "temp" as backup position
        temp = n;

        if ( n == (nmax-1) ) {
            // early terminating
            strout[n] = '\0';
            // negativ = fail
            n = -1 * temp; 
            return n;
        }
        // else: ok, 1 char is possible. We only have to check more than 1 char
        

        if (*traverse == '0') {
            traverse++; // ignore 0
            padding = (unsigned char) *traverse - '0';
            traverse++; // ignore 1-9 (padding number)
        }

        switch (*traverse) {
            case 'c' :
                i = va_arg(*args, int);
                _putc(strout, (char) i, n);
                break;
            case 'd' :
                i = va_arg(*args, int);
                if (i < 0) {
                    i = -i;
                    _putc(strout, '-', n);
                }
                _nputs(strout, nmax, StrFormat::convert(i, 10, padding), n);
                if (n < 0) {
                    n = -1 * temp; 
                    return n;
                }
                break;
            case 'o':
                i = va_arg(*args, unsigned int);
                _nputs(strout, nmax, StrFormat::convert(i, 8, padding), n);
                if (n < 0) {
                    n = -1 * temp; 
                    return n;
                }
                break;
            case 's':
                s = va_arg(*args, char *);
                _nputs(strout, nmax, s, n);
                if (n < 0) {
                    n = -1 * temp; 
                    return n;
                }
                break;
            case 'x':
                i = va_arg(*args, unsigned int);
                if (n < 0) {
                    n = -1 * temp; 
                    return n;
                }
                _nputs(strout, nmax, StrFormat::convert(i, 16, padding), n);
                if (n < 0) {
                    n = -1 * temp; 
                    return n;
                }
                break;
            case 'b':
                i = va_arg(*args, unsigned int);
                if (n < 0) {
                    n = -1 * temp; 
                    return n;
                }
                _nputs(strout, nmax, StrFormat::convert(i, 2, padding), n);
                if (n < 0) {
                    n = -1 * temp; 
                    return n;
                }
                break;
        }
    }
    return n;
}

int snprintf ( char * str, size_t nmax, const char * format, ... ) {
    int n = 0;
    va_list args;
    va_start(args, format);
    n = snprintf(str, nmax, format, args);
    va_end(args);
    return n;
}

int snprintf ( char * str, size_t nmax, const char * format, va_list args ) {
    int n = 0;
    _vsnprintf(str, (int) nmax, format, n, &args);
    return n;
}
