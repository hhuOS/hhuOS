#ifndef LIBC_STDARG
#define LIBC_STDARG

#include <stddef.h>

typedef void * va_list;

#ifdef __cplusplus
extern "C" {
#endif

void _va_start(va_list * list, void * firstParam, size_t firstParamSize);

void * _va_arg(va_list * list, size_t typeSize);

#ifdef __cplusplus
}
#endif

#define va_start(list, param) _va_start(&list, ((void*)&param), sizeof(param))
#define va_arg(list, T) *((T*)_va_arg(&list, sizeof(T)))
#define va_end(list) list = NULL


#endif