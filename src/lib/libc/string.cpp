#include <cstdint>
#include "lib/libc/string.h"

#include "lib/util/base/Address.h"

//string manipulation
char * strcpy(char * dest, const char* src) {
	while (*src) {
		(*dest) = (*src);
		src++;
		dest++;
	}
	(*dest) = '\0'; 
	return dest;
}


char * strncpy(char * dest, const char* src, size_t count) {
	for (size_t i=0; i<count; i++) {
		(*dest) = (*src);
		if (*src) src++; //only move further if end of string hasn't been reached
		dest++;
	}
	return dest;
}

char * strcat(char * dest, const char * src) {
	char * c = dest; 
	while (*c) c++; //get null-terminator of dest
	strcpy(c, src);
	return dest;
}

char * strncat(char * dest, const char * src, size_t count) {
	char * c = dest; 
	while (*c) c++; //get null-terminator of dest
	
	for (size_t i=0; i<count; i++) {
		(*c) = (*src);
		if (!(*src)) return dest; //terminate at null character
		src++;
		c++;
	}
	(*c) = '\0'; //add null character if none was written
	return dest;
	
}

//no transformation is performed, as locales aren't implemented
size_t strxfrm(char * dest, const char *src, size_t count) {
	for (size_t i=0; i<count; i++) {
		(*dest) = (*src);
		if (!(*src)) return i-1; 
		src++;
		dest++;
	}
	return count-1;
}



//string examination
size_t strlen(const char* str) {
	size_t ret = 0;
	while (*str) {
		str++;
		ret++;
	}
	return ret;
}

int strcmp(const char* lhs, const char* rhs) {
	while ((*lhs) != '\0' || (*rhs) != '\0') {
		if ((*lhs) != (*rhs)) return (*lhs) - (*rhs);
		lhs++;
		rhs++;
	}
	return 0;
}

int strncmp(const char* lhs, const char* rhs, size_t count) {
	for (size_t i=0; i<count; i++, lhs++, rhs++) {
		if ((*lhs) != (*rhs)) return (*lhs) - (*rhs);
		if ((*lhs)=='\0' && (*rhs)=='\0') return 0;
	}
	return 0;
}

int strcoll(const char* lhs, const char* rhs) {
	return strcmp(lhs, rhs); //locale collation order currently unimplemented
}

//find first occurence of ch
char * strchr(const char * str, int ch) {
	while (*str) {
		if ((*str) == (char)ch) return (char*) str;
		str++;
	}
	if (ch == 0) return (char*)str;
	return NULL;
	
}

//find last occurence of ch
char * strrchr(const char * str, int ch) {
	char * ret = NULL;
	while (*str) {
		if ((*str) == (char)ch) ret = (char*)str;
		str++;
	}
	if (ch == 0) return (char*)str;
	return ret;
}

//number of initial character in dest that appear in src
size_t strspn(const char* dest, const char* src) {
	size_t ret = 0;
	while (*dest) {
		if (!strchr(src, *dest)) return ret; //return if character not in src
		dest++;
		ret++;
	}
}

//number of initial character in dest that don't appear in src
size_t strcspn(const char *dest, const char * src) {
	size_t ret = 0;
	while (*dest) {
		if (strchr(src, *dest)) return ret; //return if character in src
		dest++;
		ret++;
	}
}

//return pointer to first char in dest that appears in breakset
char * strpbrk(const char * dest, const char *breakset) {
	while (*dest) {
		if (strchr(breakset, *dest)) return (char*)dest; //return if character in breakset
		dest++;
	}
	return NULL;
}

//find first occurence of substring substr
char * strstr(const char * str, const char *substr) {
	if ((*substr) == '\0') return (char*)str;
	
	char * current = (char*)str;
	char * next = NULL;
	
	while (1) {
		char * substrCurrent = (char*) substr;
		char * strCurrent = current;
		next = NULL;
		int fault = 0;
		while ((*substrCurrent) && (*strCurrent)) {
			if ((*substrCurrent) != (*strCurrent)) {
				fault = 1;
				break;
			}
			substrCurrent++;
			strCurrent++;
			if (!next && (*strCurrent) == (*substr)) next = strCurrent; //find next occurence of first substr char
		}
		
		if (!fault) return current; //substring found
		if (!(*strCurrent)) return NULL; //string end reached 
		
		if (next) {
			current = next;
		} else {
			current++;
		}
	}
}

//return pointer to first character not in delim
char * _strtok_strcpbrk(const char * dest, const char *delim) {
	while (*dest) {
		if (!strchr(delim, *dest)) return (char*)dest; //return if character not in delim
		dest++;
	}
	return NULL;
}

char * _strtok_next_token = NULL;


char * strtok(char * str, const char * delim) {
	if (str) {
		char* tokStart = _strtok_strcpbrk(str, delim);
		if (!tokStart) return NULL; //no tokens in str 
		
		char * tokEnd = strpbrk(tokStart, delim);
		if (tokEnd) {
			(*tokEnd) = '\0'; //replace delimiter with null
			_strtok_next_token = tokEnd+1;
		} else {
			_strtok_next_token = NULL;
		}
		return tokStart;
		
	} else {
		if (!_strtok_next_token) return NULL;
		return strtok(_strtok_next_token, delim); //process next token in line
	}
}

//memory manipulation 
void * memchr(const void* ptr, int ch, size_t count) {
	const unsigned char * p = (const unsigned char *)ptr;
	for (size_t i=0; i<count;i++,p++) {
		if ((*p) == (unsigned char)ch) return (void*)p;
	}
	return NULL;
}

int memcmp(const void * lhs, const void* rhs, size_t count) {
	return (int)Util::Address((uint32_t)lhs).compareRange(Util::Address((uint32_t)rhs), (uint32_t)count);
}

void * memset(void * dest, int ch, size_t count) {
	Util::Address((uint32_t)dest).setRange(ch, (uint32_t)count);
	return dest;
}
void * memcpy(void * dest, const void * src, size_t count) {
	Util::Address((uint32_t)dest).copyRange(Util::Address((uint32_t)src), (uint32_t)count);
	return dest;
}

void * memmove(void * dest, const void * src, size_t count) {
	char * tempBuf = new char[count]();
	memcpy(tempBuf, src, count);
	memcpy(dest, tempBuf, count);
	delete [] tempBuf;
	return dest;
}


//error handling

char * errorStrings[] = {
	"",
	"Function Domain Error",
	"Function Range Error",
	"Illegal Byte Sequence"
};

char * strerror(int errnum) {
	if (errnum>0 && errnum <4) return errorStrings[errnum];
	return "No Error";
}
