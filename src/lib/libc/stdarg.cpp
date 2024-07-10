/*#include "lib/libc/stdarg.h"

void _va_start(va_list * list, void * firstParam, size_t firstParamSize) {
	*list = firstParam + firstParamSize;
}

void * _va_arg(va_list * list, size_t typeSize) {
	void * ret = *list;
	*list += typeSize;
	return ret;
}*/