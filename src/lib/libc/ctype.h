#ifndef _LIBC_CTYPE
#define _LIBC_CTYPE


#ifdef __cplusplus
extern "C" {
#endif

int isalnum(int c);
int isalpha(int c);
int islower(int c);
int isupper(int c);
int isdigit(int c);
int isxdigit(int c);
int iscntrl(int c);
int isgraph(int c);
int isspace(int c);
int isblank(int c);
int isprint(int c);
int ispunct(int c);
int tolower(int c);
int toupper(int c);

#ifdef __cplusplus
}
#endif

#endif