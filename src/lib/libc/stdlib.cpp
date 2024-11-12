#include <stdint.h>
#include "lib/libc/string.h"
#include "lib/libc/stdlib.h"
#include "lib/libc/ctype.h"
#include "lib/libc/limits.h"
#include "lib/libc/errno.h"

#include "lib/interface.h"
#include "lib/util/base/System.h"
#include "lib/util/base/String.h"
#include "lib/util/math/Math.h"
#include "lib/util/collection/ArrayList.h"
#include "lib/runtime/runtime.h"
#include "lib/util/math/Random.h"
#include "lib/util/io/stream/ByteArrayInputStream.h"
#include "lib/util/io/stream/ScanStream.h"
#include "lib/util/base/WideChar.h"

//memory management
void *malloc(size_t size) {
	return allocateMemory(size);
}

void *calloc(size_t num, size_t size) {
    void *buf = allocateMemory(size * num);
    memset(buf, 0, size * num);

	return buf;
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


//program utility
int system(const char* command) {
	Util::ArrayList<Util::String> args = Util::ArrayList<Util::String>();
	args.add("/");
	args.add("-c");
	
	Util::String commString = Util::String(command);
	for (auto arg : commString.split(" ")) {
		args.add(arg);
	}
	
	auto shell = executeBinary(
		Util::Io::File("/bin/shell"), 
		Util::Io::File("/device/terminal"), 
		Util::Io::File("/device/terminal"), 
		Util::Io::File("/device/terminal"), 
		"shell", 
		args.toArray());
	shell.join();
	return 0; //process return type not preserved currently
}

char * getenv([[maybe_unused]] const char* name) {
	return NULL; //unimplemented
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
		int compResult = comp(key, (void*)((uint8_t*)ptr + size*index));
		if (compResult == 0) {
			return (void*)((uint8_t*)ptr + size*index);
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
	uint8_t * pivot = (uint8_t*)ptr + ((count-1)*size);
	uint8_t * i = (uint8_t*)ptr;
	
	uint32_t reti = 0;
	
	for (uint8_t * j = (uint8_t*)ptr; j < (uint8_t*)ptr+(count-1)*size; j+= size) {
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
	qsort((void*)((uint8_t*)ptr + (p+1)*size), count - p - 1, size, comp);
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


//String conversion


long strtol(const char* str, char **str_end, int base) {
	Util::Io::ByteArrayInputStream is((uint8_t*) str, 0);
	is.disableSizeLimit();
	Util::Io::ScanStream ss(is);
	
	long ret = ss.readInt(base);
	if (ret == LONG_MAX || ret == LONG_MIN) setErrno(ERANGE);
	if (str_end) *str_end = (char*)(str + ss.getReadBytes());
	
	return ret;
}

unsigned long strtoul(const char* str, char **str_end, int base) {
	Util::Io::ByteArrayInputStream is((uint8_t*) str, 0);
	is.disableSizeLimit();
	Util::Io::ScanStream ss(is);
	
	unsigned long ret = ss.readUint(base);
	if (ret == ULONG_MAX) setErrno(ERANGE);
	if (str_end) *str_end = (char*)(str + ss.getReadBytes());
	
	return ret;
}


double strtod(const char* str, char **str_end) {
	Util::Io::ByteArrayInputStream is((uint8_t*) str, 0);
	is.disableSizeLimit();
	Util::Io::ScanStream ss(is);
	
	double ret = ss.readDouble();
	if (str_end) *str_end = (char*)(str + ss.getReadBytes());
	
	return ret;
}

double atof (const char* str) {
	return strtod(str, NULL);
}


int atoi (const char *str) {
	return strtol(str, NULL, 0);
}


long atol (const char *str) {
	return strtol(str, NULL, 0);
}


//multibyte utf-8 strings 
int mblen(const char* s, size_t n) {
	return Util::WideChar::utf8Length(s,n);
}

int mbtowc(wchar_t * pwc, const char* s, size_t n) {
	return Util::WideChar::utf8ToWchar(pwc, s, n);
}

int wctomb(char * s, wchar_t wc) {
	return Util::WideChar::wcharToUtf8(s, wc);
}

size_t mbstowcs(wchar_t * dst, const char * s, size_t len) {
	size_t curr;
	for (curr = 0; curr <len; curr++){
		size_t charlen = mbtowc(dst + curr, s, MB_LEN_MAX);
		
		if (charlen <= 0) return curr-1;
		
		s+=charlen;
	}
	return curr;
}

size_t wcstombs(char* dst, const wchar_t * src, size_t len) {
	size_t curr = 0;
	while (curr < len) {
		size_t charlen = wctomb(dst + curr, *src);
		
		if (charlen<=0) return curr-1;
		
		src++;
		curr += charlen;
	}
	
	return curr;
}