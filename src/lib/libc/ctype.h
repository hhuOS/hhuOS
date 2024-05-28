#ifndef _LIBC_CTYPE
#define _LIBC_CTYPE

extern "C" int isalnum(int c);
extern "C" int isalpha(int c);
extern "C" int islower(int c);
extern "C" int isupper(int c);
extern "C" int isdigit(int c);
extern "C" int isxdigit(int c);
extern "C" int iscntrl(int c);
extern "C" int isgraph(int c);
extern "C" int isspace(int c);
extern "C" int isblank(int c);
extern "C" int isprint(int c);
extern "C" int ispunct(int c);
extern "C" int tolower(int c);
extern "C" int toupper(int c);

#endif