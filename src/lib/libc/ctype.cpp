#include "lib/libc/ctype.h"


int isalnum(int c) {
	return (c>=48 && c<=57) || (c>=65 && c<=90) || (c>=97 && c<=122);
}

int isalpha(int c) {
	return (c>=65 && c<=90) || (c>=97 && c<=122);
}

int isdigit(int c) {
	return (c>=48 && c<=57);
}

int islower(int c) {
	return (c>=97 && c<=122);
}

int isupper(int c) {
	return (c>=65 && c<=90);
}

int isxdigit(int c) {
	return (c>=48 && c<=57) || (c>=65 && c<=70) || (c>=97 && c<=102);
}

int iscntrl(int c) {
	return (c>=0 && c<=31) || c==127;
}

int isgraph(int c) {
	return c>=33 && c <=126;
}

int isspace(int c) {
	return (c>=9 && c <=13) || c==32;
}

int isblank(int c) {
	return c==9 || c==32;
}

int isprint(int c) {
	return c>=32 && c<=126;
}

int ispunct(int c) {
	return (c>=33 && c<=47) || (c>=58 && c<=64) || (c>=91 && c<=96) || (c>=123 && c<=126);
}

int tolower(int c) {
	return isupper(c) ? c + 32 : c;
}
int toupper(int c) {
	return islower(c) ? c - 32 : c;
}