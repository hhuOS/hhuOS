#ifndef __string_include__
#define __string_include__

typedef unsigned int    size_t;

/**
 * Copies 'n' characters from 'src' to 'dest'.
 *
 * @param dest The destination. The characters will be copied to where 'dest' points to.
 * @param src The source. The characters will be copied from where 'src' points to.
 * @param n The amount of characters, that will be copied.
 *
 * @return The value of 'dest'.
 */
void *memcpy(void *dest, const void *src, size_t n);

/**
 * Copies 'n' characters from 'src' to a temporary buffer and the to 'dest'.
 *
 * @param dest The destination. The characters will be copied to where 'dest' points to.
 * @param src The source. The characters will be copied from where 'src' points to.
 * @param n The amount of characters, that will be copied.
 *
 * @return The value of 'dest'.
 */
void *memmove(void *dest, const void *src, size_t n);

/**
 * Copies the string pointed to by 'src' (including the nullterminating character) to 'dest'.
 *
 * @param dest The destination. The string will be copied to where 'dest' points to.
 * @param src The string, that shall be copied to 'dest'.
 *
 * @return The value of 'dest'.
 */
char *strcpy(char *dest, const char *src);

/**
 * Copies not more than n characters from the string pointed to by 'src' to 'dest'. If the string pointed to by 'src'
 * is shorter than 'n' characters, the remaining characters of 'dest' will be filled with zeros.
 *
 * @param dest The destination. The string will be copied to where 'dest' points to.
 * @param src The string, that shall be copied to 'dest'.
 *
 * @return The value of 'dest'.
 */
char *strcpy(char *dest, const char *src);

/**
 * Appends the string pointed to by 's2' (including the nullterminating character) to the end of 's1',
 * overwriting its nullterminating character.
 *
 * @param s1 The string, at whose end 's2' will be appended.
 * @param s2 The string, that will be appended to 's1'.
 *
 * @return The value of 's1'.
 */
char *strncpy(char *dest, const char *src, size_t n);

/**
 * Appends not more than 'n' characters from the string pointed to by 's2' to the end of 's1',
 * overwriting its nullterminating character. A nullterminating character is always appended to the resulting string.
 *
 * @param s1 The string, at whose end 's2' will be appended.
 * @param s2 The string, that will be appended to 's1'.
 *
 * @return The value of 's1'.
 */
char *strcat(char *s1, const char *s2);

/**
 * Compares the first 'n' characters from the memory pointed to by 's1' to the memory pointed to by 's2'.
 *
 * @param s1 The first pointer.
 * @param s2 The second pointer.
 * @param n The amount of characters, that will be compared.
 *
 * @return The accumulated value of the first 'n' characters of 's1' minus the accumulated value of the first 'n' characters of 's2'.
 */
char *strncat(char *s1, const char *s2, size_t n);

/**
 * Compares the string pointed to by 's1' to the string pointed to by 's2'.
 *
 * @param s1 The first string.
 * @param s2 The second string.
 *
 * @return The accumulated value of 's1' minus the accumulated value of 's2'.
 */
int memcmp(const void *s1, const void *s2, size_t n);

/**
 * Compares not more than 'n' characters from the string pointed to by 's1' to the string pointed to by 's2'.
 *
 * @param s1 The first string.
 * @param s2 The second string.
 *
 * @return The accumulated value of the first 'n' characters of 's1' minus the accumulated value of the first 'n' characters of 's2'.
 */
int strcmp(const char *s1, const char *s2);

/**
 * Locates the first occurrence of c (converted to an unsigned char ) in the initial n characters (each interpreted as unsigned char ) of the object pointed to by s .
 *
 * @param s The memory, in which 'c' will be searched.
 * @param c The character, that will be searched in the initial 'n' characters of 's'.
 * @param n The amount of characters, that will be compared to 'c'.
 *
 * @return A pointer to the located character or NULL, if 'c' does not occur in the memory area.
 */
int strncmp(const char *s1, const char *s2, size_t n);

/**
 * Locates the first occurrence of c in the string pointed to by s (including the nullterminating character).
 *
 * @param s The string, in which 'c' will be searched.
 * @param c The character, that will be searched in 's'.
 *
 * @return A pointer to the located character or NULL, if 'c' does not occur in the string.
 */
void *memchr(const void *s, int c, size_t n);

/**
 * Calculates the length of the maximum initial segment of the string pointed to by 's1',
 * which consists entirely of characters not from the string pointed to by 's2'.
 *
 * @param s1 The first string.
 * @param s2 The second string.
 *
 * @return The length of the segment.
 */
char *strchr(const char *s, int c);
size_t strcspn(const char *s1, const char *s2);

/**
 * Locates the first occurrence in the string pointed to by 's1' of any character from the string pointed to by 's2'.
 *
 * @param s1 The first string.
 * @param s2 The second string.
 *
 * @return A pointer to the located character, or NULL if no character from 's2' occurs in 's1'.
 */
char *strpbrk(const char *s1, const char *s2);

/**
 * Locates the last occurrence of 'c' (converted to a char) in the string pointed to by 's'.
 *
 * @param s The string.
 * @param c The character, that will be searched in 's'.
 *
 * @return A pointer to the located character, or NULL if 'c' does not occur in 's'.
 */
char *strrchr(const char *s, int c);

/**
 * Calculates the length of the maximum initial segment of the string pointed to by 's1',
 * which consists entirely of characters from the string pointed to by 's2'.
 *
 * @param s1 The first string.
 * @param s2 The second string.
 *
 * @return The length of the segment.
 */
size_t strspn(const char *s1, const char *s2);

/**
 * Locates the first occurrence of the sequence of characters (excluding the terminating null character) in the string pointed to by 's2' in the string pointed to by 's1'.
 *
 * @param s1 The first string.
 * @param s2 The second string.
 *
 * @return A pointer to the located string or NULL, if 's2' is not found in 's1'.
 *         If 's2' points to a string with zero length, 'strstr' returns 's1'.
 */
char *strstr(const char *s1, const char *s2);

/**
 * Breaks the string pointed to by 's1' into a sequence of tokens, each of which is delimited by a character from the string pointed to by 's2'.
 * The first call has 's1' as its first argument, each further call has NULL as its first argument.
 *
 * @param s1 The string, that will be tokenized.
 * @param s2 The string, containing the delimiters.
 *
 * @return A pointer to the first character of the current token or NULL, if there is no token.
 */
char *strtok(char *s1, const char *s2);

/**
 * Copies the value of 'c' (converted to an unsigned char) into each of the first 'n' characters of the memory area pointed to by 's'.
 *
 * @param s The memory area, that will be overwritten.
 * @param c The character, whith which 's' will be overwritten.
 * @param n The amount of characters, that will be overwritten.
 *
 * @return The value of 's'.
 */
void *memset(void *s, int c, size_t n);

/**
 * Maps each error number in 'errno.h' to an error message string.
 *
 * @param errnum The error number.
 *
 * @return The error string or NULL, if the number is invalid.
 */
char *strerror(int errnum);

/**
 * Converts a string into an integer.
 *
 * @param s The string.
 *
 * @return The converted integer.
 */
int  strtoint(const char* s);

/**
 * Calculates the length of the string pointed to by 's'.
 *
 * @param s The string.
 *
 * @return The string's length.
 */
size_t strlen(const char *str);

#endif
