#ifndef _LIB_STR_FORMAT_H
#define _LIB_STR_FORMAT_H 1

#include <cstdarg>

class StrFormat {

public:

    static char * convert(unsigned int number, unsigned int base, unsigned int padding = 0) {

        unsigned int length = 0;

        static char buffer[50];

        char *ptr;

        ptr = &buffer[49];

        *ptr = '\0';

        do {

            *--ptr = hex[number % base];

            number /= base;

            length++;

        } while (number != 0);
        
        while (length < padding) {

            *--ptr = hex[number % base];

            length++;
        }
        
        return(ptr);
    }

private:

    static constexpr const char* hex = "0123456789ABCDEF";
    
};

#endif
