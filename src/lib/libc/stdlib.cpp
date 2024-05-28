#include <cstdint>
#include "lib/libc/stdlib.h"

#include "lib/interface.h"
#include "lib/util/base/System.h"
#include "lib/runtime/runtime.h"
#include "lib/util/math/Random.h"

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


//random 

Util::Math::Random rng;
int rand() {
	return rng.nextRandomNumber() * RAND_MAX;
}

void srand( unsigned int seed ) {
	rng = Util::Math::Random(seed);
}

//algorithms
void* bsearch(const void* key, const void* ptr, size_t count, size_t size, int (*comp)(const void*, const void*)) {
	size_t index = count/2;
	size_t leftMargin = count/2;
	size_t rightMargin = count - leftMargin - 1;
	
	while (1) {
		int compResult = comp(key, ptr + size*index);
		if (compResult == 0) {
			return (void*)(ptr + size*index);
		} else if (compResult < 0) {
			if (leftMargin == 0) return NULL;
			index -= (leftMargin+1)/2;
			rightMargin = (leftMargin+1)/2 - 1;
			leftMargin -= (leftMargin+1)/2;
		} else if (compResult > 0) {
			if (rightMargin == 0) return NULL;
			index += (rightMargin+1)/2;
			leftMargin = (rightMargin+1)/2-1;
			rightMargin -= (rightMargin+1)/2;
		}
	}
		
}


void _qsort_swap(void * a, void* b, size_t size) {
	uint8_t * pa = (uint8_t*)a;
	uint8_t * pb = (uint8_t*)b;
	for (size_t i = 0; i<size; i++, pa++, pb++) {
		uint8_t t = *pa;
		*pa = *pb;
		*pb = t;
	}
}

uint32_t _qsort_partition(void* ptr, size_t count, size_t size, int (*comp)(const void*, const void*)) {
	void * pivot = ptr + ((count-1)*size);
	void * i = ptr;
	
	uint32_t reti = 0;
	
	for (void * j = ptr; j < ptr+(count-1)*size; j+= size) {
		if (comp(j, pivot) <= 0) {
			_qsort_swap(i, j, size);
			i += size;
			reti++;
		}
	}
	
	_qsort_swap(i, pivot, size);
	return reti;
}

void qsort(void* ptr, size_t count, size_t size, int (*comp)(const void*, const void*)) {
	if (count<=1) return;
	
	uint32_t p = _qsort_partition(ptr, count, size, comp);
	
	qsort(ptr, p, size, comp);
	qsort(ptr + (p+1)*size, count - p - 1, size, comp);
}


//maths 
int abs(int n) {
	return n < 0 ? -n : n;
}

int labs(long n) {
	return n < 0 ? -n : n;
}

div_t div( int x, int y) {
	div_t ret;
	ret.quot = x/y;
	ret.rem = x%y;
	return ret;
}

ldiv_t ldiv ( long x, long y) {
	ldiv_t ret;
	ret.quot = x/y;
	ret.rem = x%y;
	return ret;
}