#ifndef LIBC_SYS_STAT
#define LIBC_SYS_STAT

#include <sys/types.h>

#ifdef __cplusplus
extern "C" {
#endif

int mkdir(const char *pathname, mode_t mode);

#ifdef __cplusplus
}
#endif

#endif