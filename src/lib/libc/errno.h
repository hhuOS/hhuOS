#ifndef LIBC_ERRNO
#define LIBC_ERRNO 

 typedef enum {
	 EDOM = 1, //parameter outside function domain
	 ERANGE = 2, //result outside of function range
	 EILSEQ = 3 //illegal byte sequence
 } errno_val_t;
 
 
 
 #ifdef __cplusplus
extern "C" {
#endif
 
void setErrno(errno_val_t val);
errno_val_t getErrno();


#ifdef __cplusplus
}
#endif
 
#define errno getErrno()
 
#endif