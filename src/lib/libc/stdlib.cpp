#include "lib/libc/stdlib.h"

#include "lib/interface.h"
#include "lib/util/base/System.h"
#include "lib/runtime/runtime.h"

//memory management
void *malloc(size_t size) {
	return allocateMemory(size);
}

void *calloc(size_t num, size_t size) {
	return allocateMemory(size * num);
}

void *realloc(void *ptr, size_t new_size) {
	return reallocateMemory(ptr, new_size);
}

void free(void *ptr) {
	freeMemory(ptr);
}


//process management
void abort() {
	LibcRuntime::abort();
}

void exit(int exit_code) {
	LibcRuntime::exitWithCleanup(exit_code);
}

int atexit(void (*func)(void)) {
	LibcRuntime::addExitCleanupFunc(func);
	return 0;
}
