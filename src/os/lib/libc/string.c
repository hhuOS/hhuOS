#include "string.h"
#include "stdlib.h"
#include <stddef.h>

static char *strtok_pointer;
static char *strtok_start_pointer;

/**
 * Copies 'n' characters from 'src' to 'dest'.
 *
 * @param dest The destination. The characters will be copied to where 'dest' points to.
 * @param src The source. The characters will be copied from where 'src' points to.
 * @param n The amount of characters, that will be copied.
 *
 * @return The value of 'dest'.
 */
void *memcpy(void *dest, const void *src, size_t n) {
    size_t i;
    for(i = 0; i < n; i++) {
        ((char *)dest)[i] = ((char *)src)[i];
    }

    return dest;
}

/**
 * Copies 'n' characters from 'src' to a temporary buffer and the to 'dest'.
 *
 * @param dest The destination. The characters will be copied to where 'dest' points to.
 * @param src The source. The characters will be copied from where 'src' points to.
 * @param n The amount of characters, that will be copied.
 *
 * @return The value of 'dest'.
 */
void *memmove(void *dest, const void *src, size_t n) {
    char *tmp = (char *) malloc(n);
    size_t i;

    for(i = 0; i < n; i++) {
        tmp[i] = ((char *)src)[i];
    }

    for(i = 0; i < n; i++) {
        ((char *)dest)[i] = tmp[i];
    }

    free(tmp);
    return dest;
}

/**
 * Copies the string pointed to by 'src' (including the nullterminating character) to 'dest'.
 *
 * @param dest The destination. The string will be copied to where 'dest' points to.
 * @param src The string, that shall be copied to 'dest'.
 *
 * @return The value of 'dest'.
 */
char *strcpy(char *dest, const char *src) {
    size_t i = 0;
    while(src[i] != 0) {
        dest[i] = src[i];
        i++;
    }
    dest[i] = src[i];

    return dest;
}

/**
 * Copies not more than n characters from the string pointed to by 'src' to 'dest'. If the string pointed to by 'src'
 * is shorter than 'n' characters, the remaining characters of 'dest' will be filled with zeros.
 *
 * @param dest The destination. The string will be copied to where 'dest' points to.
 * @param src The string, that shall be copied to 'dest'.
 *
 * @return The value of 'dest'.
 */
char *strncpy(char *dest, const char *src, size_t n) {
    size_t i = 0;
    while(src[i] != 0 && i < n) {
        dest[i] = src[i];
        i++;
    }

    while(i < n) {
        dest[i] = 0;
        i++;
    }

    return dest;
}

/**
 * Appends the string pointed to by 's2' (including the nullterminating character) to the end of 's1',
 * overwriting its nullterminating character.
 *
 * @param s1 The string, at whose end 's2' will be appended.
 * @param s2 The string, that will be appended to 's1'.
 *
 * @return The value of 's1'.
 */
char *strcat(char *s1, const char *s2) {
    size_t len = strlen(s1);

    size_t j = 0;
    while(s2[j] != 0) {
        s1[len + j] = s2[j];
        j++;
    }
    s1[len + j] = 0;

    return s1;
}

/**
 * Appends not more than 'n' characters from the string pointed to by 's2' to the end of 's1',
 * overwriting its nullterminating character. A nullterminating character is always appended to the resulting string.
 *
 * @param s1 The string, at whose end 's2' will be appended.
 * @param s2 The string, that will be appended to 's1'.
 *
 * @return The value of 's1'.
 */
char *strncat(char *s1, const char *s2, size_t n) {
    size_t len = strlen(s1);

    size_t j = 0;
    while(s2[j] != 0 && j < n) {
        s1[len + j] = s2[j];
        j++;
    }

    if(j >= n) {
        s1[len + j] = 0;
    }

    return s1;
}

/**
 * Compares the first 'n' characters from the memory pointed to by 's1' to the memory pointed to by 's2'.
 *
 * @param s1 The first pointer.
 * @param s2 The second pointer.
 * @param n The amount of characters, that will be compared.
 *
 * @return The accumulated value of the first 'n' characters of 's1' minus the accumulated value of the first 'n' characters of 's2'.
 */
int memcmp(const void *s1, const void *s2, size_t n) {
    size_t i = 0;
    while(i < n && ((unsigned char *) s1)[i] == ((unsigned char *) s2)[i]) {
        i++;
    }

    if (i >= n) return 0;
    return ((unsigned char *) s1)[i] - ((unsigned char *) s2)[i];
}

/**
 * Compares the string pointed to by 's1' to the string pointed to by 's2'.
 *
 * @param s1 The first string.
 * @param s2 The second string.
 *
 * @return The accumulated value of 's1' minus the accumulated value of 's2'.
 */
int strcmp(const char *s1, const char *s2) {
    size_t i = 0;
    while(((unsigned char *) s1)[i] != 0 && ((unsigned char *) s1)[i] == ((unsigned char *) s2)[i]) {
        i++;
    }

    return ((unsigned char *) s1)[i] - ((unsigned char *) s2)[i];
}

/**
 * Compares not more than 'n' characters from the string pointed to by 's1' to the string pointed to by 's2'.
 *
 * @param s1 The first string.
 * @param s2 The second string.
 *
 * @return The accumulated value of the first 'n' characters of 's1' minus the accumulated value of the first 'n' characters of 's2'.
 */
int strncmp(const char *s1, const char *s2, size_t n) {
    size_t i = 0;
    while(((unsigned char *) s1)[i] != 0 && i < n && ((unsigned char *) s1)[i] == ((unsigned char *) s2)[i]) {
        i++;
    }

    if (i >= n) return 0;
    return ((unsigned char *) s1)[i] - ((unsigned char *) s2)[i];
}

/**
 * Locates the first occurrence of c (converted to an unsigned char ) in the initial n characters (each interpreted as unsigned char ) of the object pointed to by s .
 *
 * @param s The memory, in which 'c' will be searched.
 * @param c The character, that will be searched in the initial 'n' characters of 's'.
 * @param n The amount of characters, that will be compared to 'c'.
 *
 * @return A pointer to the located character or NULL, if 'c' does not occur in the memory area.
 */
void *memchr(const void *s, int c, size_t n) {
    size_t i;
    for(i = 0; i < n; i++) {
        if(((unsigned char *) s)[i] == (unsigned char) c) {
            return &((unsigned char *) s)[i];
        }
    }

    return NULL;
}

/**
 * Locates the first occurrence of c in the string pointed to by s (including the nullterminating character).
 *
 * @param s The string, in which 'c' will be searched.
 * @param c The character, that will be searched in 's'.
 *
 * @return A pointer to the located character or NULL, if 'c' does not occur in the string.
 */
char *strchr(const char *s, int c) {
    size_t i = 0;
    while(s[i] != (char) c && s[i] != 0) {
        i++;
    }

    if(s[i] == 0 && c != 0) {
        return NULL;
    }

    return ((char *)&s[i]);
}

/**
 * Calculates the length of the maximum initial segment of the string pointed to by 's1',
 * which consists entirely of characters not from the string pointed to by 's2'.
 *
 * @param s1 The first string.
 * @param s2 The second string.
 *
 * @return The length of the segment.
 */
size_t strcspn(const char *s1, const char *s2) {
    size_t ret = strlen(s1);
    size_t i;
    for(i = 0; i < strlen(s2); i++) {
        char *tmp = strchr(s1, s2[i]);
        if(tmp != NULL) {
            if((size_t)(tmp - s1) < ret) {
                ret = tmp - s1;
            }
        }
    }

    return ret;
}

/**
 * Locates the first occurrence in the string pointed to by 's1' of any character from the string pointed to by 's2'.
 *
 * @param s1 The first string.
 * @param s2 The second string.
 *
 * @return A pointer to the located character, or NULL if no character from 's2' occurs in 's1'.
 */
char *strpbrk(const char *s1, const char *s2) {
    int index = -1;
    size_t i;
    for(i = 0; i < strlen(s2); i++) {
        size_t j;
        for(j = 0; j < strlen(s1); j++) {
            if(s1[j] == s2[i]) {
                if(index == -1 || (int)j < index) {
                    index = j;
                }
            }
        }
    }

    if(index == -1) {
        return NULL;
    } else {
        return ((char *)&s1[index]);
    }
}

/**
 * Locates the last occurrence of 'c' (converted to a char) in the string pointed to by 's'.
 *
 * @param s The string.
 * @param c The character, that will be searched in 's'.
 *
 * @return A pointer to the located character, or NULL if 'c' does not occur in 's'.
 */
char *strrchr(const char *s, int c) {
    int index = -1;
    size_t i;
    for(i = 0; i < strlen(s); i++) {
        if(s[i] == c) {
            index = i;
        }
    }

    if(index == -1) {
        return NULL;
    } else {
        return ((char *)&s[index]);
    }
}

/**
 * Calculates the length of the maximum initial segment of the string pointed to by 's1',
 * which consists entirely of characters from the string pointed to by 's2'.
 *
 * @param s1 The first string.
 * @param s2 The second string.
 *
 * @return The length of the segment.
 */
size_t strspn(const char *s1, const char *s2) {
    size_t ret = 0;
    size_t i;
    for(i = 0; i < strlen(s1); i++) {
        unsigned char found = 0;
        size_t j;
        for(j = 0; j < strlen(s2); j++) {
            if(s1[i] == s2[j]) {
                ret++;
                found = 1;
                break;
            }
        }
        if(!found) {
            return ret;
        }
    }
    return ret;
}

/**
 * Locates the first occurrence of the sequence of characters (excluding the terminating null character) in the string pointed to by 's2' in the string pointed to by 's1'.
 *
 * @param s1 The first string.
 * @param s2 The second string.
 *
 * @return A pointer to the located string or NULL, if 's2' is not found in 's1'.
 *         If 's2' points to a string with zero length, 'strstr' returns 's1'.
 */
char *strstr(const char *s1, const char *s2) {
    size_t i;
    for(i = 0; i < strlen(s1); i++) {
        unsigned char found = 1;
        size_t j;
        for(j = 0; j < strlen(s2); j++) {
            if(s1[i + j] != s2[j]) {
                found = 0;
                break;
            }
        }
        if(found) {
            return ((char *)&s1[i]);
        }
    }
    return NULL;
}

/**
 * Breaks the string pointed to by 's1' into a sequence of tokens, each of which is delimited by a character from the string pointed to by 's2'.
 * The first call has 's1' as its first argument, each further call has NULL as its first argument.
 *
 * @param s1 The string, that will be tokenized.
 * @param s2 The string, containing the delimiters.
 *
 * @return A pointer to the first character of the current token or NULL, if there is no token.
 */
char *strtok(char *s1, const char *s2) {

    if(s1 != NULL) {
        if(strtok_pointer)
            free(strtok_start_pointer);
        strtok_pointer = malloc(strlen(s1) + 1);
        strtok_start_pointer = strtok_pointer;
        strcpy(strtok_pointer, s1);
    }

    // TODO(ruhland):
    //  Check if this is the right thing to do
    if (strtok_pointer == NULL) {
        return NULL;
    }

    strtok_pointer = &(strtok_pointer[strspn(strtok_pointer, s2)]);

    size_t end_index = strcspn(strtok_pointer, s2);
    if(end_index == 0) {
        return NULL;
    }

    char *ret = strtok_pointer;
    if(ret[end_index] == 0) {
        strtok_pointer = NULL;
        return ret;
    }

    ret[end_index] = 0;

    strtok_pointer += end_index + 1;

    return ret;
}

/**
 * Copies the value of 'c' (converted to an unsigned char) into each of the first 'n' characters of the memory area pointed to by 's'.
 *
 * @param s The memory area, that will be overwritten.
 * @param c The character, whith which 's' will be overwritten.
 * @param n The amount of characters, that will be overwritten.
 *
 * @return The value of 's'.
 */
void *memset(void *s, int c, size_t n) {
    size_t i;
    for(i = 0; i < n; i++) {
        ((unsigned char *)(s))[i] = (unsigned char)c;
    }

    return s;
}

/**
 * Maps each error number in 'errno.h' to an error message string.
 *
 * @param errnum The error number.
 *
 * @return The error string or NULL, if the number is invalid.
 */
char *strerror(int errnum) {
    switch(errnum) {
        case 1: return "Domain error!"; break;
        case 2: return "Range error!"; break;
        default: return NULL; break;
    }
}

/**
 * Calculates the length of the string pointed to by 's'.
 *
 * @param s The string.
 *
 * @return The string's length.
 */
size_t strlen(const char *s) {
    size_t i = 0;
    while(s[i] != 0) {
        i++;
    }

    return i;
}

int strtoint(const char* str) {
    size_t length = strlen(str);
    int limit = 0, modifier = 1, result = 0;

    if (str[0] == '-') {
        limit = 1;
        modifier = -1;
    }
    int j = 1;
    int num = 0;
    for(int i = length - 1; i >= limit; i--) {
        num = (str[i] - '0');
        if (num >= 0 && num <= 10) {
            result += num * j;
            j *= 10;
        }
    }

    return result * modifier;
}
