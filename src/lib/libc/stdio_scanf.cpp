#include <ctype.h>


#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "errno.h"
#include "lib/util/io/stream/ScanStream.h"
#include "lib/util/io/stream/ByteArrayInputStream.h"



int _stream_vscanf(Util::Io::InputStream &is, const char* format, va_list vlist) {
	auto ss = Util::Io::ScanStream(is);
	return ss.vscanf(format, vlist);
}


int vscanf(const char* format, va_list args) {
	return vfscanf(stdin, format, args);
}

int vfscanf(FILE * stream, const char * format, va_list args) {
	return _stream_vscanf(*stream, format, args);
}

int vsscanf(const char * s, const char * format, va_list args) {
	Util::Io::ByteArrayInputStream bs((uint8_t*)s, 0);
	bs.disableSizeLimit();
	bs.makeNullTerminated();
	return _stream_vscanf(bs, format, args);
}

int scanf(const char* format, ...) {
	va_list args;
	va_start(args, format);
	int ret = vscanf(format, args);
	va_end(args);
	return ret;
}

int fscanf(FILE * stream, const char * format, ...) {
	va_list args;
	va_start(args, format);
	int ret = vfscanf(stream, format, args);
	va_end(args);
	return ret;
}

int sscanf(const char * s, const char * format, ...) {
	va_list args;
	va_start(args, format);
	int ret = vsscanf(s, format, args);
	va_end(args);
	return ret;
}