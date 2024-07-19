#ifndef LIBC_STDIO_DEF
#define LIBC_STDIO_DEF 

#include <stddef.h>
//Contains all definitions necessary for the communication between stdio and StdioFileStream

#define EOF -1

#define BUFSIZ 1024

enum FileMode {
	READ,
	WRITE,
	APPEND,
	READ_EXTEND,
	WRITE_EXTEND,
	APPEND_EXTEND
};	


#define _IOFBF 0
#define _IOLBF 1
#define _IONBF 2

#define SEEK_SET 0
#define SEEK_CUR 1
#define SEEK_END 2
	


#endif