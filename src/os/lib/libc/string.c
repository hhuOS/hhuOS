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

#include "string.h"
#include "stdlib.h"
#include <stddef.h>

static char *strtok_pointer;
static char *strtok_start_pointer;

void *memcpy(void *dest, const void *src, size_t n) {
    size_t i;

    for(i = 0; i < n; i++) {
        ((char *)dest)[i] = ((char *)src)[i];
    }

    return dest;
}

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

char *strcpy(char *dest, const char *src) {
    size_t i = 0;

    while(src[i] != 0) {
        dest[i] = src[i];
        i++;
    }
    dest[i] = src[i];

    return dest;
}

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

int memcmp(const void *s1, const void *s2, size_t n) {
    size_t i = 0;

    while(i < n && ((unsigned char *) s1)[i] == ((unsigned char *) s2)[i]) {
        i++;
    }

    if (i >= n) return 0;
    return ((unsigned char *) s1)[i] - ((unsigned char *) s2)[i];
}

int strcmp(const char *s1, const char *s2) {
    size_t i = 0;

    while(((unsigned char *) s1)[i] != 0 && ((unsigned char *) s1)[i] == ((unsigned char *) s2)[i]) {
        i++;
    }

    return ((unsigned char *) s1)[i] - ((unsigned char *) s2)[i];
}

int strncmp(const char *s1, const char *s2, size_t n) {
    size_t i = 0;

    while(((unsigned char *) s1)[i] != 0 && i < n && ((unsigned char *) s1)[i] == ((unsigned char *) s2)[i]) {
        i++;
    }

    if (i >= n) return 0;
    return ((unsigned char *) s1)[i] - ((unsigned char *) s2)[i];
}

void *memchr(const void *s, int c, size_t n) {
    size_t i;

    for(i = 0; i < n; i++) {
        if(((unsigned char *) s)[i] == (unsigned char) c) {
            return &((unsigned char *) s)[i];
        }
    }

    return NULL;
}

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

void *memset(void *s, int c, size_t n) {
    size_t i;
    for(i = 0; i < n; i++) {
        ((unsigned char *)(s))[i] = (unsigned char)c;
    }

    return s;
}

char *strerror(int errnum) {
    switch(errnum) {
        case 1: return "Domain error!"; break;
        case 2: return "Range error!"; break;
        default: return NULL; break;
    }
}

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
