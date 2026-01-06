/*
 * Copyright (C) 2017-2026 Heinrich Heine University Düsseldorf,
 * Institute of Computer Science, Department Operating Systems
 * Main developers: Christian Gesse <christian.gesse@hhu.de>, Fabian Ruhland <ruhland@hhu.de>
 * Original development team: Burak Akguel, Christian Gesse, Fabian Ruhland, Filip Krakowski, Michael Schöttner
 * This project has been supported by several students.
 * A full list of integrated student theses can be found here: https://github.com/hhuOS/hhuOS/wiki/Student-theses
 *
 * This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public
 * License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any
 * later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied
 * warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>
 *
 * The stanard C library is based on a master's thesis, written by Tobias Fabian Oehme.
 * The original source code can be found here: https://github.com/ToboterXP/hhuOS/tree/thesis
 */

#include <stdint.h>

#include "string.h"

#include "stdlib.h"
#include "limits.h"
#include "errno.h"
#include "interface.h"
#include "runtime/runtime.h"
#include "util/base/System.h"
#include "util/base/String.h"
#include "util/collection/ArrayList.h"
#include "util/math/Random.h"
#include "util/io/stream/ByteArrayInputStream.h"
#include "util/io/stream/ScanStream.h"
#include "util/base/WideChar.h"

// Memory management
void* malloc(const size_t size) {
	return allocateMemory(size);
}

void* calloc(const size_t num, const size_t size) {
    void *buf = allocateMemory(size * num);
    memset(buf, 0, size * num);

	return buf;
}

void* realloc(void *ptr, const size_t new_size) {
	return reallocateMemory(ptr, new_size);
}

void free(void *ptr) {
	freeMemory(ptr);
}


// Process management
void abort() {
	LibcRuntime::abort();
}

void exit(const int exit_code) {
	LibcRuntime::exitWithCleanup(exit_code);
}

int atexit(void (*func)()) {
	LibcRuntime::addExitCleanupFunc(func);
	return 0;
}

// Program utility
int system(const char *command) {
	Util::ArrayList<Util::String> args{};
	args.add("/");
	args.add("-c");

	for (const auto &arg : Util::String(command).split(" ")) {
		args.add(arg);
	}

	const auto shell = executeBinary(
		Util::Io::File("/bin/shell"), 
		Util::Io::File("/device/terminal"), 
		Util::Io::File("/device/terminal"), 
		Util::Io::File("/device/terminal"), 
		"shell", 
		args.toArray());

	shell.join();
	return 0; // Process return type not preserved currently
}

char* getenv(const char*) {
	return nullptr; // Unimplemented
}

// Random
Util::Math::Random rng;

int rand() {
	return static_cast<int>(rng.getRandomNumber() * RAND_MAX);
}

void srand(const unsigned int seed) {
	rng = Util::Math::Random(seed);
}

// Algorithms
void* bsearch(const void *key, const void *ptr, const size_t count, const size_t size,
	int (*comp)(const void*, const void*))
{
	size_t index = count / 2;
	size_t leftMargin = count / 2;
	size_t rightMargin = count - leftMargin - 1;
	
	while (true) {
		const int compResult = comp(key, static_cast<const void*>(static_cast<const uint8_t*>(ptr) + size * index));

		if (compResult == 0) {
			return const_cast<uint8_t*>(static_cast<const uint8_t*>(ptr) + size * index);
		}

		if (compResult < 0) {
			if (leftMargin == 0) {
				return nullptr;
			}

			index -= (leftMargin + 1) / 2;
			rightMargin = (leftMargin + 1) / 2 - 1;
			leftMargin -= (leftMargin + 1) / 2;
		} else {
			if (rightMargin == 0) {
				return nullptr;
			}

			index += (rightMargin + 1) / 2;
			leftMargin = (rightMargin + 1) / 2 - 1;
			rightMargin -= (rightMargin + 1) / 2;
		}
	}
}

void qsort_swap(void *a, void *b, const size_t size) {
	auto *pa = static_cast<uint8_t*>(a);
	auto *pb = static_cast<uint8_t*>(b);

	for (size_t i = 0; i < size; i++, pa++, pb++) {
		const auto t = *pa;
		*pa = *pb;
		*pb = t;
	}
}

uint32_t qsort_partition(void *ptr, const size_t count, const size_t size, int (*comp)(const void*, const void*)) {
	auto *pivot = static_cast<uint8_t*>(ptr) + (count - 1) * size;
	auto *i = static_cast<uint8_t*>(ptr);
	
	uint32_t reti = 0;
	
	for (auto *j = static_cast<uint8_t*>(ptr); j < static_cast<uint8_t*>(ptr) + (count - 1) * size; j += size) {
		if (comp(j, pivot) <= 0) {
			qsort_swap(i, j, size);
			i += size;
			reti++;
		}
	}
	
	qsort_swap(i, pivot, size);
	return reti;
}

void qsort(void *ptr, const size_t count, const size_t size, int (*comp)(const void*, const void*)) {
	if (count <= 1) {
		return;
	}

	const auto p = qsort_partition(ptr, count, size, comp);
	
	qsort(ptr, p, size, comp);
	qsort(static_cast<uint8_t*>(ptr) + (p + 1) * size, count - p - 1, size, comp);
}

// Maths
int abs(const int n) {
	return n < 0 ? -n : n;
}

int labs(const long n) {
	return n < 0 ? -n : n;
}

div_t div(const int x, const int y) {
	return div_t {
		x / y,
		x % y
	};
}

ldiv_t ldiv(const long x, const long y) {
	return ldiv_t {
		x / y,
		x % y
	};
}


// String conversion
long strtol(const char *str, char **str_end, const int base) {
	Util::Io::ByteArrayInputStream is(reinterpret_cast<const uint8_t*>(str));
	Util::Io::ScanStream ss(is);

	const auto ret = static_cast<long>(ss.readSigned64(base));
	if (ret == LONG_MAX || ret == LONG_MIN) {
		setErrno(ERANGE);
	}

	if (str_end) {
		*str_end = const_cast<char*>(str + ss.getReadBytes());
	}
	
	return ret;
}

unsigned long strtoul(const char *str, char **str_end, const int base) {
	Util::Io::ByteArrayInputStream is(reinterpret_cast<const uint8_t*>(str));
	Util::Io::ScanStream ss(is);
	
	const auto ret = static_cast<unsigned long>(ss.readUnsigned64(base));
	if (ret == ULONG_MAX) {
		setErrno(ERANGE);
	}

	if (str_end) {
		*str_end = const_cast<char*>(str + ss.getReadBytes());
	}
	
	return ret;
}

double strtod(const char *str, char **str_end) {
	Util::Io::ByteArrayInputStream is(reinterpret_cast<const uint8_t *>(str));
	Util::Io::ScanStream ss(is);

	const auto ret = static_cast<double>(ss.readFloatingPointNumber());

	if (str_end) {
		*str_end = const_cast<char*>(str + ss.getReadBytes());
	}
	
	return ret;
}

double atof(const char *str) {
	return strtod(str, nullptr);
}


int atoi(const char *str) {
	return strtol(str, nullptr, 0);
}

long atol(const char *str) {
	return strtol(str, nullptr, 0);
}

// Multibyte UTF-8 strings
int mblen(const char *s, const size_t n) {
	return Util::WideChar::utf8Length(s, n);
}

int mbtowc(wchar_t *pwc, const char *s, const size_t n) {
	return Util::WideChar::utf8ToWchar(pwc, s, n);
}

int wctomb(char *s, const wchar_t wc) {
	return Util::WideChar::wcharToUtf8(s, wc);
}

size_t mbstowcs(wchar_t *dst, const char *s, const size_t len) {
	size_t curr;
	for (curr = 0; curr < len; curr++){
		int charlen = mbtowc(dst + curr, s, MB_LEN_MAX);
		
		if (charlen <= 0) {
			*(dst + curr) = '\0';
			return curr;
		}
		
		s += charlen;
	}

	return curr;
}

size_t wcstombs(char *dst, const wchar_t *src, const size_t len) {
	size_t curr = 0;
	while (curr < len) {
		int charlen = wctomb(dst + curr, *src);
		
		if (charlen <= 0 || *src == L'\0') {
			*(dst + curr) = '\0';
			return curr;
		}
		
		src++;
		curr += charlen;
	}
	
	return curr;
}