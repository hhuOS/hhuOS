#include "lib/libc/errno.h"

int currentErrno;

void setErrno(int val) {
	currentErrno = val;
 }
 
 int * getErrno() {
	 return &currentErrno;
 }