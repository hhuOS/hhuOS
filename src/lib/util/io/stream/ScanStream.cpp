#include "ScanStream.h"

#include "lib/util/base/CharacterTypes.h"
#include "lib/util/math/Math.h"
#include "lib/util/io/stream/InputStream.h"
#include <limits.h>

#define EOF -1

namespace Util::Io {

ScanStream::ScanStream(InputStream &stream) : stream(stream){
}

int16_t ScanStream::read() {
	if (readLimit >= 0 && readChars >= readLimit) {
        return EOF;
    }
	
	int16_t ret = stream.read();
	
	if (ret >= 0) {
        readChars++;
    }

	return ret;
}

int16_t ScanStream::peek() {
	if (readLimit >= 0 && readChars >= readLimit) {
        return EOF;
    }
	
	return stream.peek();
}

bool ScanStream::isReadyToRead() {
	return stream.isReadyToRead();
}

int32_t ScanStream::read(uint8_t *targetBuffer, uint32_t offset, uint32_t length) {
	readChars += length;
	return stream.read(targetBuffer, offset, length);
}

uint32_t ScanStream::getReadBytes() const {
	return readChars;
}

void ScanStream::setReadLimit(int64_t limit) {
	readLimit = limit;
}


//String conversion
int _get_char_value(char c, int base) {
	if (CharacterTypes::isDigit(c)) c -= 48;
	else if (CharacterTypes::isUpper(c)) c -= 55;
	else if (CharacterTypes::isLower(c)) c -= 87;
	else return -1;
	
	if (c >= base) return -1;
	
	return c;
}

long long ScanStream::readLong(int base) {
	int16_t c;
	while (1) {
		c = peek();
		if (c < 0 || !CharacterTypes::isWhitespace(c) || c == '\0') break;
		read();
	}

	//stream is now at first character past whitespace
	c = peek();
	if (c <= 0) return 0;
	
	//detect sign
	int sign = 1;
	if (c == '-') {
		sign = -1;
		read();
	} else if (c == '+') {
		read();
	}
	
	
	//stream is past sign;
	
	//automatic base detection
	c = peek();
	if (base == 0) {
		if (c == '0') {
			base = 8;
			read();
			c = peek();
			if (c == 'x' || c == 'X') {
				base = 16;
				read();
			}
		} else {
			base = 10;
		}
	}
	
	//skip base prefix if existing
	c = peek();
	if (base == 8 && c == '0') read();
	if (base == 16 && c=='0' ) {
		read(); 
		int16_t c = peek();
		if (c == 'x' || c == 'X') {
			read();
		} 
	}
	
	//stream is now at first character of number
	
	//find end of number
	char buf[32];
	int len;
	
	for (len=0;len<32;len++) {
		c = peek();
		
		if (!CharacterTypes::isAlphaNumeric(c) || c<=0) break;
		if (_get_char_value(c, base) == -1) break;
		
		buf[len] = c;
		read();
	}
	
	//interpret number
	long long ret = 0;
	long long mul = 1;
	for (int i=len-1; i >= 0; i--, mul*=base) {
		ret += _get_char_value(buf[i], base) * mul;
	}
	
	return sign*ret;
}

int32_t ScanStream::readInt(int base) {
	long long ret = readLong(base);
	
	if (ret < LONG_MIN) return LONG_MIN;
	if (ret > LONG_MAX) return LONG_MAX;
	
	return (int32_t)ret;
}

uint32_t ScanStream::readUint(int base) {
	long long ret = readLong(base);
	
	if (ret > ULONG_MAX) return ULONG_MAX;
	if (ret < -ULONG_MAX) return ULONG_MAX;
	
	return (uint32_t)(ret < 0 ? ULONG_MAX + ret + 1: ret);
}

double ScanStream::readDouble() {
	int16_t c;
	
	//skip leading whitespace
	while (1) {
		c = peek();
		if (c < 0 || !CharacterTypes::isWhitespace(c) || c == '\0') break;
		read();
	}
	
	c = peek();
	
	if (c == '\0') return 0;
	
	//read sign
	int sign = 1;
	if (c == '-') {
		sign = -1;
		read();
	} else if (c == '+') {
		read();
	}
	
	//read integer part
	c = peek();
	double ret = 0;
	while (CharacterTypes::isDigit(c)) {
		read();
		ret *= 10;
		ret += _get_char_value(c, 10);
		c = peek();
	}
	
	//read decimal part
	if (c== '.') {
		read();
		c = peek();
		double mul = 0.1;
		while (CharacterTypes::isDigit(c)) {
			read();
			ret += mul * _get_char_value(c, 10);
			mul/=10;
			c = peek();
		}
	}
	
	c = peek();
	if (c == 'e' || c == 'E') {
		read();
		ret *= Util::Math::powInt(10, readInt());
	}
	
	return sign*ret;
}

}