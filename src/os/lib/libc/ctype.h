#ifndef __ctype_include__
#define __ctype_include__

/**
 * Tests, if a given character is a letter or a digit.
 *
 * @param c The character to be tested.
 *
 * @return true, if c is a letter or a digit;
 *         false, if not.
 */
extern int isalnum(int c);

/**
 * Tests, if a given character is a letter.
 *
 * @param c The character to be tested.
 *
 * @return true, if c is a letter;
 *         false, if not.
 */
extern int isalpha(int c);

/**
 * Tests, if a given character is a control-character.
 *
 * @param c The character to be tested.
 *
 * @return true, if c is a control-character;
 *         false, if not.
 */
extern int iscntrl(int c);

/**
 * Tests, if a given character is a digit.
 *
 * @param c The character to be tested.
 *
 * @return true, if c is a digit;
 *         false, if not.
 */
extern int isdigit(int c);

/**
 * Tests, if a given character can be printed (except space).
 *
 * @param c The character to be tested.
 *
 * @return true, if c can be printed;
 *         false, if not.
 */
extern int isgraph(int c);

/**
 * Tests, if a given character is a lower-case letter.
 *
 * @param c The character to be tested.
 *
 * @return true, if c is a lower-case letter;
 *         false, if not.
 */
extern int islower(int c);

/**
 * Tests, if a given character can be printed (including space).
 *
 * @param c The character to be tested.
 *
 * @return true, if c can be printed;
 *         false, if not.
 */
extern int isprint(int c);

/**
 * Tests, if a given character is a punctuation character.
 *
 * @param c The character to be tested.
 *
 * @return true, if c is a punctuation character;
 *         false, if not.
 */
extern int ispunct(int c);

/**
 * Tests, if a given character is a white-space character.
 *
 * @param c The character to be tested.
 *
 * @return true, if c is a white-space character;
 *         false, if not.
 */
extern int isspace(int c);

/**
 * Tests, if a given character is an upper-case letter.
 *
 * @param c The character to be tested.
 *
 * @return true, if c is a upper-case letter;
 *         false, if not.
 */
extern int isupper(int c);

/**
 * Tests, if a given character is a hexadecimal-digit.
 *
 * @param c The character to be tested.
 *
 * @return true, if c is a hexadecimal-digit;
 *         false, if not.
 */
extern int isxdigit(int c);

#endif
