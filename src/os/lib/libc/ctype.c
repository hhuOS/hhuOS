#include "ctype.h"

int isalnum(int c) {
    return isalpha(c) || isdigit(c);
}

int isalpha(int c) {
    return isupper(c) || islower(c);
}

int iscntrl(int c) {
    return !isprint(c);
}

int isdigit(int c) {
    return c >= 0x30 && c <= 0x39;
}

int isgraph(int c) {
    return c >= 0x21 && c <= 0x7e;
}

int islower(int c) {
    return c >= 0x61 && c <= 0x7a;
}

int isprint(int c) {
    return c >= 0x20 && c <= 0x7e;
}

int ispunct(int c) {
    return isgraph(c) && !isalnum(c);
}

int isspace(int c) {
    return c >= 0x09 && c <= 0x0d;
}

int isupper(int c) {
    return c >= 0x41 && c <= 0x5a;
}

int isxdigit(int c) {
    return isdigit(c) || (c >= 0x41 && c <= 0x46) || (c >= 0x61 && c <= 0x66);
} 
