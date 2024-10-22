#ifndef LIBC_STDIO
#define LIBC_STDIO

#include <stdarg.h>
#include <stddef.h>

typedef unsigned long fpos_t;

#ifdef __cplusplus

#include "lib/util/io/stream/FileStream.h"
#include "lib/util/io/stream/OutputStream.h"
#include "lib/util/io/stream/InputStream.h"

typedef Util::Io::FileStream FILE;

int _stream_vprintf(Util::Io::OutputStream &os, const char* format, va_list vlist);
int _stream_vscanf(Util::Io::InputStream &is, const char* format, va_list vlist);

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

#define BUFSIZ 1024

#ifndef EOF
#define EOF -1
#endif


#define _IOFBF 0
#define _IOLBF 1
#define _IONBF 2

#define SEEK_SET 0
#define SEEK_CUR 1
#define SEEK_END 2
	



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


int vprintf(const char * format, va_list vlist);
int vfprintf(FILE * stream, const char * format, va_list vlist);
int vsprintf(char * buffer, const char * format, va_list vlist);
int vsnprintf(char * buffer, size_t bufsz, const char * format, va_list vlist);

int printf(const char * format, ...);
int fprintf(FILE * stream, const char * format, ...);
int sprintf(char * buffer, const char * format, ...);
int snprintf(char * buffer, size_t bufsz, const char * format, ...);


int vscanf(const char* format, va_list args);
int vfscanf(FILE * stream, const char * format, va_list args);
int vsscanf(const char * s, const char * format, va_list args);

int scanf(const char* format, ...);
int fscanf(FILE * stream, const char * format, ...);
int sscanf(const char * s, const char * format, ...);


#ifdef __cplusplus
}
#endif



#endif