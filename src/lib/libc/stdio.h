#ifndef LIBC_STDIO
#define LIBC_STDIO

#include "lib/libc/stdio_def.h"

typedef unsigned long fpos_t;

#ifdef __cplusplus

#include "lib/libc/StdioFileStream.h"

typedef Libc::StdioFileStream FILE;

#else 
	
typedef void FILE;

#endif


extern FILE * stdin;
extern FILE * stdout;
extern FILE * stderr;

#define FOPEN_MAX 100
#define FILENAME_MAX 1024
#define TMP_MAX 9999
#define L_tmpnam 1024


#ifdef __cplusplus
extern "C" {
#endif

void _init_stdio(); //called by runtime

FILE * fopen(const char* filename, const char* mode);
FILE * freopen(const char * filename, const char * mode, FILE * stream);
int fclose(FILE * stream);
int fflush(FILE * stream);
void setbuf(FILE * stream, char * buf);
int setvbuf (FILE * stream, char * buf, int mode, size_t size);

size_t fread(void * buffer, size_t size, size_t count, FILE * stream);
size_t fwrite(const void* buffer, size_t size, size_t count, FILE * stream);

int fgetc(FILE * stream);
int getc(FILE * stream);
char * fgets(char * str, int count, FILE * stream);
int fputc(int ch, FILE * stream);
int putc(int ch, FILE * stream);
int fputs(const char * str, FILE* stream);
int getchar();
char * gets(char * str);
int putchar(int ch);
int puts(const char * str);
int ungetc(int ch, FILE * stream);

long ftell(FILE * stream);
int fgetpos(FILE * stream, fpos_t * pos);
int fseek(FILE * stream, long offset, int origin);
int fsetpos(FILE * stream, const fpos_t * pos);
void rewind(FILE * stream);

void clearerr(FILE * stream);
int feof(FILE * stream);
int ferror(FILE * stream);
void perror(const char * s);

int remove(const char * pathname);
int rename(const char * old_filename, const char * new_filename);
char * tmpnam(char * filename);
FILE * tmpfile();

#ifdef __cplusplus
}
#endif



#endif