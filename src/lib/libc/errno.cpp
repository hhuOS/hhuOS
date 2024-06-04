#include "lib/libc/errno.h"

errno_val_t currentErrno;

void setErrno(errno_val_t val) {
	currentErrno = val;
 }
 
 errno_val_t getErrno() {
	 return currentErrno;
 }