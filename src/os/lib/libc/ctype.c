#include "ctype.h"

/**
 * Tests, if a given character is a letter or a digit.
 * 
 * @param c The character to be tested.
 * 
 * @return true, if c is a letter or a digit;
 *         false, if not.
 */
int isalnum(int c) {
    return isalpha(c) || isdigit(c);
}

/**
 * Tests, if a given character is a letter.
 * 
 * @param c The character to be tested.
 * 
 * @return true, if c is a letter;
 *         false, if not.
 */
int isalpha(int c) {
    return isupper(c) || islower(c);
}

/**
 * Tests, if a given character is a control-character.
 * 
 * @param c The character to be tested.
 * 
 * @return true, if c is a control-character;
 *         false, if not.
 */
int iscntrl(int c) {
    return !isprint(c);
}

/**
 * Tests, if a given character is a digit.
 * 
 * @param c The character to be tested.
 * 
 * @return true, if c is a digit;
 *         false, if not.
 */
int isdigit(int c) {
    return c >= 0x30 && c <= 0x39;
}

/**
 * Tests, if a given character can be printed (except space).
 * 
 * @param c The character to be tested.
 * 
 * @return true, if c can be printed;
 *         false, if not.
 */
int isgraph(int c) {
    return c >= 0x21 && c <= 0x7e;
}

/**
 * Tests, if a given character is a lower-case letter.
 * 
 * @param c The character to be tested.
 * 
 * @return true, if c is a lower-case letter;
 *         false, if not.
 */
int islower(int c) {
    return c >= 0x61 && c <= 0x7a;
}

/**
 * Tests, if a given character can be printed (including space).
 * 
 * @param c The character to be tested.
 * 
 * @return true, if c can be printed;
 *         false, if not.
 */
int isprint(int c) {
    return c >= 0x20 && c <= 0x7e;
}

/**
 * Tests, if a given character is a punctuation character.
 * 
 * @param c The character to be tested.
 * 
 * @return true, if c is a punctuation character;
 *         false, if not.
 */
int ispunct(int c) {
    return isgraph(c) && !isalnum(c);
}

/**
 * Tests, if a given character is a white-space character.
 * 
 * @param c The character to be tested.
 * 
 * @return true, if c is a white-space character;
 *         false, if not.
 */
int isspace(int c) {
    return c >= 0x09 && c <= 0x0d;
}

/**
 * Tests, if a given character is an upper-case letter.
 * 
 * @param c The character to be tested.
 * 
 * @return true, if c is a upper-case letter;
 *         false, if not.
 */
int isupper(int c) {
    return c >= 0x41 && c <= 0x5a;
}

/**
 * Tests, if a given character is a hexadecimal-digit.
 * 
 * @param c The character to be tested.
 * 
 * @return true, if c is a hexadecimal-digit;
 *         false, if not.
 */
int isxdigit(int c) {
    return isdigit(c) || (c >= 0x41 && c <= 0x46) || (c >= 0x61 && c <= 0x66);
} 
