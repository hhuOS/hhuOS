#include "lib/libc/stdio.h"

#include "lib/libc/ctype.h"
#include "lib/libc/stdlib.h"
#include "lib/libc/math.h"
#include "lib/util/io/stream/OutputStream.h"
#include "lib/util/io/stream/PrintStream.h"
#include "lib/util/base/System.h"
#include "lib/util/io/stream/ByteArrayOutputStream.h"

#include <stdarg.h>
#include <stddef.h>



int _stream_vprintf(Util::Io::OutputStream &os, const char* format, va_list args) {
	Util::Io::PrintStream out(os);
	return out.vprintf(format, args);
	}
	


int vfprintf(FILE * stream, const char * format, va_list vlist) {
	return _stream_vprintf(*stream, format, vlist);
}

int vprintf(const char * format, va_list vlist) {
	return vfprintf(stdout, format, vlist);
}

int vsprintf(char * buffer, const char * format, va_list vlist) {
	Util::Io::ByteArrayOutputStream os((uint8_t*)buffer, -1);
	int ret = _stream_vprintf(os, format, vlist);
	os.write('\0');
	return ret;
}

int vsnprintf(char * buffer, size_t bufsz, const char * format, va_list vlist) {
	Util::Io::ByteArrayOutputStream os((uint8_t*)buffer, bufsz);
	os.setEnforceSizeLimit(true);
	int ret = _stream_vprintf(os, format, vlist);
	os.write('\0');
	return ret;
}

int printf(const char * format, ...) {
	va_list list;
	va_start(list, format);
	int ret = vprintf(format, list);
	va_end(list);
	return ret;
}

int fprintf(FILE * stream, const char * format, ...) {
	va_list list;
	va_start(list, format);
	int ret = vfprintf(stream, format, list);
	va_end(list);
	return ret;
}

int sprintf(char * buffer, const char * format, ...) {
	va_list list;
	va_start(list, format);
	int ret = vsprintf(buffer, format, list);
	va_end(list);
	return ret;
}

int snprintf(char * buffer, size_t bufsz, const char * format, ...) {
	va_list list;
	va_start(list, format);
	int ret = vsnprintf(buffer, bufsz, format, list);
	va_end(list);
	return ret;
}
	
