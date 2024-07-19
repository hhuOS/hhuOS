#ifndef LIBC_ERRNO
#define LIBC_ERRNO 

	
#define EDOM  1 //parameter outside function domain
#define ERANGE  2 //result outside of function range
#define EILSEQ  3 //illegal byte sequence
 
 
 
 
 
 #ifdef __cplusplus
extern "C" {
#endif
 
void setErrno(int val);
int * getErrno();


#ifdef __cplusplus
}
#endif
 
#define errno *getErrno()
 
#endif