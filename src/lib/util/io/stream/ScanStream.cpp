#include <limits.h>

#include "ScanStream.h"

#include "base/WideChar.h"
#include "lib/util/base/CharacterTypes.h"
#include "lib/util/math/Math.h"
#include "lib/util/io/stream/InputStream.h"

namespace Util::Io {

ScanStream::ScanStream(InputStream &stream) : FilterInputStream(stream){
}

int16_t ScanStream::read() {
	if (readLimit >= 0 && readBytes >= readLimit) {
        return -1;
    }

	int16_t ret = FilterInputStream::read();

	if (ret >= 0) {
        readBytes++;
    }

	return ret;
}

int16_t ScanStream::peek() {
	if (readLimit >= 0 && readBytes >= readLimit) {
        return -1;
    }

	return FilterInputStream::peek();
}

bool ScanStream::isReadyToRead() {
	return FilterInputStream::isReadyToRead();
}

int32_t ScanStream::read(uint8_t *targetBuffer, uint32_t offset, uint32_t length) {
	readBytes += length;
	return FilterInputStream::read(targetBuffer, offset, length);
}

uint32_t ScanStream::getReadBytes() const {
	return readBytes;
}

void ScanStream::setReadLimit(int64_t limit) {
	readLimit = limit;
}

long long ScanStream::readLong(int32_t base) {
	int16_t c;

	// Skip leading whitespace
	while (true) {
		c = peek();
		if (!CharacterTypes::isWhitespace(c) || c <= 0) {
			break;
		}

		read();
	}

	// Stream is now at first character past whitespace
	c = peek();
	if (c <= 0) {
		return 0;
	}
	
	// Detect sign
	int8_t sign = 1;
	if (c == '-') {
		sign = -1;
		read();
	} else if (c == '+') {
		read();
	}
	
	// Automatic base detection
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

	// Skip base prefix if existing
	c = peek();
	if (base == 8 && c == '0') {
		read();
	}

	if (base == 16 && c == '0' ) {
		read();
		c = peek();
		if (c == 'x' || c == 'X') {
			read();
		}
	}
	
	// Find end of number
	constexpr int8_t BUFFER_SIZE = 32;
	char buf[BUFFER_SIZE];
	int8_t len;
	
	for (len = 0; len < BUFFER_SIZE; len++) {
		c = peek();

		if (!CharacterTypes::isAlphaNumeric(c) || c <= 0) {
			break;
		}

		if (charToInt(c, base) == -1) {
			break;
		}
		
		buf[len] = static_cast<char>(c);
		read();
	}
	
	// Interpret number
	long long ret = 0;
	long long mul = 1;

	for (int8_t i = len - 1; i >= 0; i--, mul *= base) {
		ret += charToInt(buf[i], base) * mul;
	}

	return sign * ret;
}

int ScanStream::readInt(int32_t base) {
	const auto value = readLong(base);
	
	if (value < LONG_MIN) {
		return LONG_MIN;
	}
	if (value > LONG_MAX) {
		return LONG_MAX;
	}
	
	return static_cast<int>(value);
}

unsigned int ScanStream::readUnsignedint(int32_t base) {
	const auto value = readLong(base);
	
	if (value > ULONG_MAX) {
		return ULONG_MAX;
	}
	if (value < -ULONG_MAX) {
		return ULONG_MAX;
	}
	
	return static_cast<uint32_t>(value < 0 ? ULONG_MAX + value + 1 : value);
}

double ScanStream::readDouble() {
	int16_t c;

	// Skip leading whitespace
	while (true) {
		c = peek();
		if (!CharacterTypes::isWhitespace(c) || c <= 0) {
			break;
		}

		read();
	}

	// Stream is now at first character past whitespace
	c = peek();
	if (c <= 0) {
		return 0;
	}

	// Detect sign
	int8_t sign = 1;
	if (c == '-') {
		sign = -1;
		read();
	} else if (c == '+') {
		read();
	}
	
	// Read integer part
	c = peek();
	double ret = 0;
	while (CharacterTypes::isDigit(c)) {
		read();
		ret *= 10;
		ret += charToInt(c, 10);
		c = peek();
	}
	
	// Read decimal part
	if (c == '.') {
		read();
		c = peek();
		double mul = 0.1;
		while (CharacterTypes::isDigit(c)) {
			read();
			ret += mul * charToInt(c, 10);
			mul /= 10;
			c = peek();
		}
	}

	c = peek();
	if (c == 'e' || c == 'E') {
		read();
		ret *= Math::pow(10.0, readInt());
	}
	
	return sign * ret;
}

wchar_t ScanStream::readWideChar() {
	uint8_t wc[4] = {0, 0, 0, 0};

	wc[0] = static_cast<uint8_t>(read());
	for (uint32_t i = 1; i < 4; i++) {
		if (WideChar::utf8Length(reinterpret_cast<const char*>(wc), 4) != -1) {
			break;
		}

		wc[i] = read();
	}

	wchar_t ret;
	WideChar::utf8ToWchar(&ret, reinterpret_cast<const char*>(wc), 4);
	return ret;
}

int32_t ScanStream::scan(const char *format, ...) {
	va_list args;
	va_start(args, format);

	const auto ret = scan(format, args);

	va_end(args);
	return ret;
}

int32_t ScanStream::scan(const char *format, [[maybe_unused]] va_list args) {
	int32_t scannedItems = 0;
	auto streamChar = peek();

	for (uint32_t i = 0; format[i] != '\0' && streamChar >= 0; i++) {
		auto formatChar = format[i];
		streamChar = peek();

		if (formatChar != '%') {
			if (CharacterTypes::isWhitespace(formatChar)) {
				// A single whitespace in the format string consumes all whitespace characters in the stream
				while (CharacterTypes::isWhitespace(streamChar)) {
					read();
					streamChar = peek();
				}
			} else {
				// Non-whitespace characters must match exactly
				if (formatChar != streamChar) {
					return scannedItems;
				}

				read();
			}
		} else {
			enum ParameterLength { SHORT, INT, LONG };

			bool suppressAssign = false;
			int32_t maxFieldWidth = -1;
			ParameterLength parameterLength = INT;

			while (true) {
				formatChar = format[++i];

				if (formatChar == '%') {
					break;
				}
				if (formatChar == 'h') {
					parameterLength = SHORT;
				} else if (formatChar == 'l' || formatChar == 'L') {
					parameterLength = LONG;
				} else if (CharacterTypes::isDigit(formatChar)) {
					maxFieldWidth = String::parseNumber<int32_t>(format + i);
					while (CharacterTypes::isDigit(format[i])) {
						i++;
					}
					i--;
				} else if (formatChar == '*') {
					suppressAssign = true;
				} else if (formatChar == '\0') {
					return scannedItems;
				} else {
					break;
				}
			}

			if (maxFieldWidth >= 0) {
				setReadLimit(getReadBytes() + maxFieldWidth);
			} else {
				setReadLimit(-1);
			}

			streamChar = peek();
			switch (formatChar) {
				case '%':
					if (streamChar == '%') {
						return scannedItems;
					}

					read();
					break;
				case 'd': {
					const auto value = readInt(10);
					if (suppressAssign) {
						break;
					}
					switch (parameterLength) {
						case SHORT:
							*va_arg(args, short int*) = static_cast<short int>(value);
							break;
						case LONG:
							*va_arg(args, long int*) = value;
							break;
						default:
							*va_arg(args, int*) = static_cast<int>(value);
							break;
					}
					break;
				}
				case 'i': {
					const auto value = readInt();
					if (suppressAssign) {
						break;
					}
					switch (parameterLength) {
						case SHORT:
							*va_arg(args, short int*) = static_cast<short int>(value);
							break;
						case LONG:
							*va_arg(args, long int*) = value;
							break;
						default:
							*va_arg(args, int*) = static_cast<int>(value);
							break;
					}
					break;
				}
				case 'u': {
					const auto value = readUnsignedint(10);
					if (suppressAssign) {
						break;
					}
					switch (parameterLength) {
						case SHORT:
							*va_arg(args, short int*) = static_cast<short int>(value);
							break;
						case LONG:
							*va_arg(args, long int*) = static_cast<long int>(value);
							break;
						default:
							*va_arg(args, int*) = static_cast<int>(value);
							break;
					}
					break;
				}
				case 'o': {
					const auto value = readUnsignedint(8);
					if (suppressAssign) {
						break;
					}
					switch (parameterLength) {
						case SHORT:
							*va_arg(args, short int*) = static_cast<short int>(value);
							break;
						case LONG:
							*va_arg(args, long int*) = static_cast<long int>(value);
							break;
						default:
							*va_arg(args, int*) = static_cast<int>(value);
							break;
					}
					break;
				}
				case 'x':
				case 'X': {
					const auto value = readUnsignedint(16);
					if (suppressAssign) {
						break;
					}
					switch (parameterLength) {
						case SHORT:
							*va_arg(args, short int*) = static_cast<short int>(value);
							break;
						case LONG:
							*va_arg(args, long int*) = static_cast<long int>(value);
							break;
						default:
							*va_arg(args, int*) = static_cast<int>(value);
							break;
					}
					break;
				}
				case 'n': {
					if (suppressAssign) {
						break;
					}
					switch (parameterLength) {
						case SHORT:
							*va_arg(args, short int*) = static_cast<short int>(getReadBytes());
							break;
						case LONG:
							*va_arg(args, long int*) = static_cast<long int>(getReadBytes());
							break;
						default:
							*va_arg(args, int*) = static_cast<int>(getReadBytes());
							break;
					}
					break;
				}
				case 'e':
				case 'E':
				case 'g':
				case 'G': {
					const auto value = readDouble();
					if (suppressAssign) {
						break;
					}
					switch (parameterLength) {
						case SHORT:
							*va_arg(args, float*) = static_cast<float>(value);
							break;
						case LONG:
							*va_arg(args, long double*) = static_cast<long double>(value);
							break;
						default:
							*va_arg(args, double*) = static_cast<double>(value);
							break;
					}
					break;
				}
				case 'p': {
					const auto value = readUnsignedint(16);
					if (!suppressAssign) {
						*va_arg(args, void**) = reinterpret_cast<void*>(value);
					}
					break;
				}
				case 'c': {
					if (maxFieldWidth < 0) {
						maxFieldWidth = 1;
					}
					switch (parameterLength) {
						case LONG: {
							auto *s = suppressAssign ? nullptr : va_arg(args, wchar_t*);
							for (int32_t j = 0; j < maxFieldWidth; j++) {
								if (!suppressAssign) {
									s[j] = readWideChar();
								}
							}
							break;
						}
						default: {
							auto *s = suppressAssign ? nullptr : va_arg(args, char*);
							for (int32_t j = 0; j < maxFieldWidth; j++) {
								if (!suppressAssign) {
									s[j] = static_cast<char>(read());
								}
							}
							break;
						}
					}
					break;
				}
				case 's': {
					switch (parameterLength) {
						case LONG: {
							auto *s = suppressAssign ? nullptr : va_arg(args, wchar_t*);
							auto c = peek();
							int32_t j;
							for (j = 0; c > 0 && !CharacterTypes::isWhitespace(c); j++) {
								if (maxFieldWidth >= 0 && j >= maxFieldWidth) {
									break;
								}

								const auto w = readWideChar();
								if (!suppressAssign) {
									s[j] = w;
								}
								c = peek();
							}

							if (!suppressAssign) {
								s[j] = '\0';
							}
							break;
						}
						default: {
							auto *s = suppressAssign ? nullptr : va_arg(args, char*);
							auto c = peek();
							int32_t j;
							for (j = 0; c > 0 && !CharacterTypes::isWhitespace(c); j++) {
								if (maxFieldWidth >= 0 && j >= maxFieldWidth) {
									break;
								}

								if (!suppressAssign) {
									s[j] = static_cast<char>(c);
								}
								read();
								c = peek();
							}

							if (!suppressAssign) {
								s[j] = '\0';
							}
							break;
						}
					}
					break;
				}
				case '[': {
					if (parameterLength == LONG) {
					Panic::fire(Panic::UNSUPPORTED_OPERATION, "ScanStream: Wide chars not supported in scanset!");
					}

					const int32_t SET_SIZE = 1024;
					char set[SET_SIZE]{};
					bool invert = false;

					int32_t j = 0;
					i++;

					// Parse invert character
					if (format[i] == '^') {
						invert = true;
						i++;
						if (format[i] == ']') {
							set[j] = ']';
							j++;
							i++;
						}
					} else if (format[i] == ']') {
						set[0] = ']';
						i++;
					}

					// Parse character set
					while (format[i] != ']' && j < SET_SIZE) {
						const auto c = format[i];
						if (c == '-') {
							for (auto c2 = format[i - 1]; c2 <= format[i + 1]; c2++) {
								set[j++] = c2;
								if (j >= SET_SIZE) {
									break;
								}
							}
						} else {
							set[j++] = c;
						}

						i++;
					}

					// Match stream output
					char *s = suppressAssign ? nullptr : va_arg(args, char*);
					while (true) {
						const auto c = peek();
						if (c <= 0) {
							break;
						}

						bool matched = false;
						for (int32_t k = 0; k < j; k++) {
							if ((invert && set[k] != c) || (!invert && set[k] == c)) {
								matched = true;
								break;
							}
						}

						if (matched) {
							const auto r = read();
							if (!suppressAssign) {
								*s++ = static_cast<char>(r);
							}
						} else {
							break;
						}
					}

					if (!suppressAssign) {
						*s = '\0';
					}

					break;
				}
			}

			setReadLimit(-1);
			scannedItems++;
		}
	}

	return scannedItems;
}

int32_t ScanStream::charToInt(int16_t c, uint32_t base) {
	if (CharacterTypes::isDigit(c)) {
		c -= 48;
	} else if (CharacterTypes::isUpper(c)) {
		c -= 55;
	} else if (CharacterTypes::isLower(c)) {
		c -= 87;
	} else {
		return -1;
	}

	if (c >= static_cast<int32_t>(base)) {
		return -1;
	}

	return c;
}

}
