#include "strformat.h"

static const char *hex = "0123456789ABCDEF";

char *convert(unsigned int number, unsigned int base, unsigned int padding) {

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