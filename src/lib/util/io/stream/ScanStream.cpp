#include <limits.h>

#include "ScanStream.h"

#include "util/base/CharacterTypes.h"
#include "util/base/WideChar.h"
#include "util/io/stream/InputStream.h"
#include "util/math/Math.h"

namespace Util::Io {

ScanStream::ScanStream(InputStream &stream) : FilterInputStream(stream) {}

int16_t ScanStream::read() {
	if (readLimit >= 0 && readBytes >= readLimit) {
        return -1;
    }

	const auto readByte = FilterInputStream::read();
	if (readByte >= 0) {
        readBytes++;
    }

	return readByte;
}

int32_t ScanStream::read(uint8_t *targetBuffer, const size_t offset, const size_t length) {
	const auto read = FilterInputStream::read(targetBuffer, offset, length);
	readBytes += read;

	return read;
}

int16_t ScanStream::peek() {
	if (readLimit >= 0 && readBytes >= readLimit) {
		return -1;
	}

	return FilterInputStream::peek();
}

bool ScanStream::isReadyToRead() {
	if (readLimit >= 0 && readBytes >= readLimit) {
		return false;
	}

	return FilterInputStream::isReadyToRead();
}

uint32_t ScanStream::getReadBytes() const {
	return readBytes;
}

int8_t ScanStream::readSigned8(const uint8_t base) {
	const auto value = readSigned64(base);
	return static_cast<int8_t>(value);
}

uint8_t ScanStream::readUnsigned8(const uint8_t base) {
	const auto value = readSigned64(base);

	if (value < 0) {
		return 0;
	}

	return static_cast<uint8_t>(value);
}

int16_t ScanStream::readSigned16(const uint8_t base) {
	const auto value = readSigned64(base);
	return static_cast<int16_t>(value);
}

uint16_t ScanStream::readUnsigned16(const uint8_t base) {
	const auto value = readSigned64(base);

	if (value < 0) {
		return 0;
	}

	return static_cast<uint16_t>(value);
}

int32_t ScanStream::readSigned32(const uint8_t base) {
	const auto value = readSigned64(base);
	return static_cast<int32_t>(value);
}

uint32_t ScanStream::readUnsigned32(const uint8_t base) {
	const auto value = readSigned64(base);

	if (value < 0) {
		return 0;
	}

	return static_cast<uint32_t>(value);
}

int64_t ScanStream::readSigned64(uint8_t base) {
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
			} else if (c == 'b' || c == 'B') {
				base = 2;
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
	constexpr int8_t BUFFER_SIZE = 64;
	char buf[BUFFER_SIZE];
	int8_t len;

	for (len = 0; len < BUFFER_SIZE; len++) {
		c = peek();

		if (!CharacterTypes::isAlphaNumeric(c) || c <= 0) {
			break;
		}

		if (charToInt(static_cast<char>(c), base) == -1) {
			break;
		}

		buf[len] = static_cast<char>(c);
		read();
	}

	// Interpret number
	int64_t number = 0;
	int64_t mul = 1;

	for (auto i = static_cast<int8_t>(len - 1); i >= 0; i--, mul *= base) {
		number += charToInt(buf[i], base) * mul;
	}

	return sign * number;
}

uint64_t ScanStream::readUnsigned64(const uint8_t base) {
	const auto value = readSigned64(base);
	return static_cast<uint64_t>(value);
}

void ScanStream::setReadLimit(int64_t limit) {
	readLimit = limit;
}

long double ScanStream::readFloatingPointNumber() {
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
	long double number = 0;
	while (CharacterTypes::isDigit(c)) {
		read();
		number *= 10;
		number += charToInt(static_cast<char>(c), 10);
		c = peek();
	}
	
	// Read decimal part
	if (c == '.') {
		read();
		c = peek();
		double mul = 0.1;
		while (CharacterTypes::isDigit(c)) {
			read();
			number += mul * charToInt(static_cast<char>(c), 10);
			mul /= 10;
			c = peek();
		}
	}

	c = peek();
	if (c == 'e' || c == 'E') {
		read();
		number *= Math::pow(10.0, readSigned32());
	}
	
	return sign * number;
}

wchar_t ScanStream::readWideCharacter() {
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

int32_t ScanStream::scan(const char *format, va_list args) {
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
			enum ParameterLength { SHORT, INT, LONG, LONG_DOUBLE };

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
				} else if (formatChar == 'l') {
					parameterLength = LONG;
				} else if (formatChar == 'L') {
					parameterLength = LONG_DOUBLE;
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
					const auto value = readSigned64(10);
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
				case 'i': {
					const auto value = readSigned64();
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
				case 'u': {
					const auto value = readUnsigned64(10);
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
					const auto value = readUnsigned64(8);
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
					const auto value = readUnsigned64(16);
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
				case 'f':
				case 'F':
				case 'e':
				case 'E':
				case 'g':
				case 'G': {
					const auto value = readFloatingPointNumber();
					if (suppressAssign) {
						break;
					}
					switch (parameterLength) {
						case LONG:
							*va_arg(args, double*) = static_cast<double>(value);
							break;
						case LONG_DOUBLE:
							*va_arg(args, long double*) = value;
							break;
						default:
							*va_arg(args, float*) = static_cast<float>(value);
							break;
					}
					break;
				}
				case 'p': {
					const auto value = readUnsigned64(16);
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
								if (s != nullptr) {
									s[j] = readWideCharacter();
								}
							}
							break;
						}
						default: {
							auto *s = suppressAssign ? nullptr : va_arg(args, char*);
							for (int32_t j = 0; j < maxFieldWidth; j++) {
								if (s != nullptr) {
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

								const auto w = readWideCharacter();
								if (s != nullptr) {
									s[j] = w;
								}
								c = peek();
							}

							if (s != nullptr) {
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

								if (s != nullptr) {
									s[j] = static_cast<char>(c);
								}
								read();
								c = peek();
							}

							if (s != nullptr) {
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

					constexpr int32_t SET_SIZE = 1024;
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
							if (s != nullptr) {
								*s++ = static_cast<char>(r);
							}
						} else {
							break;
						}
					}

					if (s != nullptr) {
						*s = '\0';
					}

					break;
				}
				default:
					break;
			}

			setReadLimit(-1);
			scannedItems++;
		}
	}

	return scannedItems;
}

ScanStream& ScanStream::operator>>(uint8_t &number) {
	number = readUnsigned8();
	return *this;
}

ScanStream& ScanStream::operator>>(int8_t &number) {
	number = readSigned8();
	return *this;
}

ScanStream& ScanStream::operator>>(uint16_t &number) {
	number = readUnsigned16();
	return *this;
}

ScanStream& ScanStream::operator>>(int16_t &number) {
	number = readSigned16();
	return *this;
}

ScanStream& ScanStream::operator>>(uint32_t &number) {
	number = readUnsigned32();
	return *this;
}

ScanStream& ScanStream::operator>>(int32_t &number) {
	number = readSigned32();
	return *this;
}

ScanStream& ScanStream::operator>>(uint64_t &number) {
	number = readUnsigned64();
	return *this;
}

ScanStream& ScanStream::operator>>(int64_t &number) {
	number = readSigned64();
	return *this;
}

ScanStream& ScanStream::operator>>(float &number) {
	number = static_cast<float>(readFloatingPointNumber());
	return *this;
}

ScanStream& ScanStream::operator>>(double &number) {
	number = static_cast<double>(readFloatingPointNumber());
	return *this;
}

ScanStream& ScanStream::operator>>(long double &number) {
	number = readFloatingPointNumber();
	return *this;
}

ScanStream& ScanStream::operator>>(wchar_t &character) {
	character = readWideCharacter();
	return *this;
}

int8_t ScanStream::charToInt(char c, const uint8_t base) {
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
