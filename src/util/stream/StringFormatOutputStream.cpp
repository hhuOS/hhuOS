#include "StringFormatOutputStream.h"

namespace Util::Stream {

StringFormatOutputStream::StringFormatOutputStream(OutputStream &stream) : FilterOutputStream(stream) {}

void StringFormatOutputStream::write(uint8_t c) {
    FilterOutputStream::write(c);
    if (c == '\n') {
        flush();
    }
}

StringFormatOutputStream& StringFormatOutputStream::operator<<(char c) {
    write(c);
    return *this;
}

StringFormatOutputStream& StringFormatOutputStream::operator<<(const char *string) {
    for (uint32_t i = 0; string[i] != 0; i++) {
        write(string[i]);
    }

    return *this;
}

StringFormatOutputStream& StringFormatOutputStream::operator<<(int16_t value) {
    return *this << (int32_t) value;
}

StringFormatOutputStream& StringFormatOutputStream::operator<<(uint16_t value) {
    return *this << (uint32_t) value;
}

StringFormatOutputStream& StringFormatOutputStream::operator<<(int32_t value) {
    if (value < 0) {
        write('-');
        value = -value;
    }

    return *this << (uint32_t) value;
}

StringFormatOutputStream& StringFormatOutputStream::operator<<(uint32_t value) {
    uint32_t div;
    char digit;
    uint8_t currentBase = base;

    if (currentBase == 8)
        write('0');
    else if (currentBase == 16) {
        write('0');
        write('x');
    }

    for (div = 1; value / div >= currentBase; div *= currentBase);

    for (; div > 0; div /= currentBase) {
        digit = static_cast<char>(value / div);

        if (digit < 10) {
            write('0' + digit);
        }
        else {
            write(static_cast<char>('A' + digit - 10));
        }

        value %= div;
    }

    return *this;
}

StringFormatOutputStream& StringFormatOutputStream::operator<<(void *ptr) {
    return *this << reinterpret_cast<uint32_t>(ptr);
}

StringFormatOutputStream& StringFormatOutputStream::operator<<(StringFormatOutputStream& (*f) (StringFormatOutputStream& )) {
    return f(*this);
}

StringFormatOutputStream& StringFormatOutputStream::endl(StringFormatOutputStream& stream) {
    stream << '\n';
    stream.flush();
    return stream;
}

StringFormatOutputStream& StringFormatOutputStream::bin(StringFormatOutputStream& stream) {
    stream.base = 2;
    return stream;
}

StringFormatOutputStream& StringFormatOutputStream::oct(StringFormatOutputStream& stream) {
    stream.base = 8;
    return stream;
}

StringFormatOutputStream& StringFormatOutputStream::dec(StringFormatOutputStream& stream) {
    stream.base = 10;
    return stream;
}

StringFormatOutputStream& StringFormatOutputStream::hex(StringFormatOutputStream& stream) {
    stream.base = 16;
    return stream;
}

}