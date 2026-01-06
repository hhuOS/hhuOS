/*
 * Copyright (C) 2017-2026 Heinrich Heine University Düsseldorf,
 * Institute of Computer Science, Department Operating Systems
 * Main developers: Christian Gesse <christian.gesse@hhu.de>, Fabian Ruhland <ruhland@hhu.de>
 * Original development team: Burak Akguel, Christian Gesse, Fabian Ruhland, Filip Krakowski, Michael Schöttner
 * This project has been supported by several students.
 * A full list of integrated student theses can be found here: https://github.com/hhuOS/hhuOS/wiki/Student-theses
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
 *
 * The stanard C library is based on a master's thesis, written by Tobias Fabian Oehme.
 * The original source code can be found here: https://github.com/ToboterXP/hhuOS/tree/thesis
 */

#include "string.h"

#include "stdlib.h"
#include "ctype.h"
#include "util/base/Address.h"

// String manipulation
char* strcpy(char *dest, const char *src) {
	while (*src) {
		*dest = (*src);
		src++;
		dest++;
	}

	*dest = '\0';
	return dest;
}


char* strncpy(char *dest, const char *src, const size_t count) {
	for (size_t i = 0; i < count; i++) {
		*dest = *src;
		if (*src) {
			src++; // Only move further if end of string hasn't been reached
		}

		dest++;
	}

	return dest;
}

char* strcat(char *dest, const char *src) {
	char * c= dest;
	while (*c) {
		c++; // Get null-terminator of dest
	}

	strcpy(c, src);
	return dest;
}

char* strncat(char *dest, const char *src, const size_t count) {
	char * c = dest; 
	while (*c) {
		c++; // Get null-terminator of dest
	}
	
	for (size_t i=0; i<count; i++) {
		*c = *src;
		if (!*src) {
			return dest; // Terminate at null character
		}

		src++;
		c++;
	}

	*c = '\0'; // Add null character if none was written
	return dest;
	
}

// No transformation is performed, as locales aren't implemented
size_t strxfrm(char *dest, const char *src, const size_t count) {
	for (size_t i = 0; i < count; i++) {
		*dest = *src;
		if (!*src) {
			return i - 1;
		}

		src++;
		dest++;
	}

	return count - 1;
}

char* strdup(const char *str1) {
	auto *ret = static_cast<char*>(malloc(sizeof(char) * (strlen(str1) + 1)));
	strcpy(ret, str1);

	return ret;
}

// String examination
size_t strlen(const char *str) {
	size_t ret = 0;
	while (*str) {
		str++;
		ret++;
	}

	return ret;
}

int strcmp(const char *lhs, const char *rhs) {
	while (*lhs != '\0' || *rhs != '\0') {
		if (*lhs != *rhs) {
			return *lhs - *rhs;
		}

		lhs++;
		rhs++;
	}

	return 0;
}

int strncmp(const char *lhs, const char *rhs, const size_t count) {
	for (size_t i = 0; i < count; i++, lhs++, rhs++) {
		if (*lhs != *rhs) {
			return *lhs - *rhs;
		}

		if (*lhs == '\0' && *rhs=='\0') {
			return 0;
		}
	}

	return 0;
}

int strcasecmp(const char *lhs, const char *rhs) {
	while (*lhs != '\0' || *rhs != '\0') {
		if (tolower((*lhs)) != tolower(*rhs)) {
			return tolower(*lhs) - tolower(*rhs);
		}

		lhs++;
		rhs++;
	}

	return 0;
}

int strncasecmp(const char *lhs, const char *rhs, const size_t count) {
	for (size_t i=0; i<count; i++, lhs++, rhs++) {
		if (tolower(*lhs) != tolower(*rhs)) {
			return tolower(*lhs) - tolower(*rhs);
		}

		if (*lhs=='\0' && *rhs=='\0') {
			return 0;
		}
	}

	return 0;
}

int strcoll(const char *lhs, const char *rhs) {
	return strcmp(lhs, rhs); // Locale collation order currently unimplemented
}

// Find first occurrence of ch
char* strchr(const char *str, const int ch) {
	while (*str) {
		if (*str == static_cast<char>(ch)) {
			return const_cast<char*>(str);
		}

		str++;
	}

	if (ch == 0) {
		return const_cast<char*>(str);
	}

	return nullptr;
}

// Find last occurrence of ch
char* strrchr(const char *str, const int ch) {
	char *ret = nullptr;
	while (*str) {
		if (*str == static_cast<char>(ch)) ret = const_cast<char*>(str);
		str++;
	}

	if (ch == 0) {
		return const_cast<char*>(str);
	}

	return ret;
}

// Number of initial characters in dest that appear in src
size_t strspn(const char *dest, const char *src) {
	size_t ret = 0;
	while (*dest) {
		if (!strchr(src, *dest)) {
			return ret; // Return if character not in src
		}

		dest++;
		ret++;
	}

	return ret;
}

// Number of initial characters in dest that don't appear in src
size_t strcspn(const char *dest, const char * src) {
	size_t ret = 0;
	while (*dest) {
		if (strchr(src, *dest)) {
			return ret; // Return if character in src
		}

		dest++;
		ret++;
	}

	return ret;
}

// Return pointer to first char in dest that appears in breakset
char* strpbrk(const char *dest, const char *breakset) {
	while (*dest) {
		if (strchr(breakset, *dest)) {
			return const_cast<char*>(dest); // Return if character in breakset
		}

		dest++;
	}

	return nullptr;
}

// Find first occurrence of substring substr
char* strstr(const char *str, const char *substr) {
	if (*substr == '\0') {
		return const_cast<char*>(str);
	}
	
	auto *current = str;
	
	while (true) {
		auto *substrCurrent = substr;
		auto *strCurrent = current;
		const char *next = nullptr;
		bool fault = false;

		while (*substrCurrent && *strCurrent) {
			if (*substrCurrent != *strCurrent) {
				fault = true;
				break;
			}

			substrCurrent++;
			strCurrent++;
			if (!next && *strCurrent == *substr) {
				next = strCurrent; // Find next occurrence of first substr char
			}
		}

        if (!*strCurrent && *substrCurrent) {
	        return nullptr; // String end reached
        }

		if (!fault) {
			return const_cast<char*>(current); // Substring found
		}
		
		if (next) {
			current = next;
		} else {
			current++;
		}
	}
}

// Return pointer to first character not in delim
char* strtok_strcpbrk(const char *dest, const char *delim) {
	while (*dest) {
		if (!strchr(delim, *dest)) {
			return const_cast<char*>(dest); // Return if character not in delim
		}

		dest++;
	}

	return nullptr;
}

char *strtok_next_token = nullptr;

char* strtok(char *str, const char * delim) {
	if (str) {
		char *tokStart = strtok_strcpbrk(str, delim);
		if (!tokStart) {
			return nullptr; // No tokens in str
		}
		
		char *tokEnd = strpbrk(tokStart, delim);
		if (tokEnd) {
			*tokEnd = '\0'; // Replace delimiter with null
			strtok_next_token = tokEnd + 1;
		} else {
			strtok_next_token = nullptr;
		}

		return tokStart;
	}

	if (!strtok_next_token) {
		return nullptr;
	}

	return strtok(strtok_next_token, delim); // Process next token in line
}

// Memory manipulation
void* memchr(const void *ptr, const int ch, const size_t count) {
	auto *p = static_cast<const unsigned char*>(ptr);
	for (size_t i = 0; i < count; i++, p++) {
		if (*p == static_cast<unsigned char>(ch)) {
			return const_cast<unsigned char*>(p);
		}
	}

	return nullptr;
}

int memcmp(const void *lhs, const void *rhs, const size_t count) {
	return Util::Address(lhs).compareRange(Util::Address(rhs), count);
}

void* memset(void *dest, const int ch, const size_t count) {
	Util::Address(dest).setRange(ch, count);
	return dest;
}
void* memcpy(void *dest, const void *src, const size_t count) {
	Util::Address(dest).copyRange(Util::Address(src), count);
	return dest;
}

void* memmove(void *dest, const void *src, const size_t count) {
	auto *tempBuf = new char[count];
	memcpy(tempBuf, src, count);
	memcpy(dest, tempBuf, count);

	delete[] tempBuf;
	return dest;
}


// Error handling
const char *errorStrings[] = {
	"No Error",
	"Function Domain Error",
	"Function Range Error",
	"Illegal Byte Sequence",
	"File Not Found",
	"File Already Exists"
};

char errorString[100];

char* strerror(int errnum) {
	if (errnum > 0 && errnum < 6) {
		strcpy(errorString, errorStrings[errnum]);
	} else {
		strcpy(errorString, errorStrings[0]);
	}

	return errorString;
}