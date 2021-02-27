/*
 * Copyright (C) 2021 Heinrich-Heine-Universitaet Duesseldorf,
 * Institute of Computer Science, Department Operating Systems
 *
 * This program is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation, either version 3 of the License,
 * or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPstreamE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>
 */

#include <device/cpu/Cpu.h>
#include "OutputStream.h"

namespace Util::Stream  {

void OutputStream::write(const uint8_t *source, uint32_t offset, uint32_t length) {
    if (offset < 0 || length < 0) {
        Device::Cpu::throwException(Device::Cpu::Exception::OUT_OF_BOUNDS, "OutputStream: Negative offset or length!");
    }

    for (uint32_t i = 0; i < length; i++) {
        write(source[offset + i]);
    }
}

void OutputStream::flush() {

}

void OutputStream::close() {

}

OutputStream &OutputStream::operator<<(char c) {
    write(c);
    return *this;
}

OutputStream &OutputStream::operator<<(const char *string) {
    for (uint32_t i = 0; string[i] != 0; i++) {
        write(string[i]);
    }

    return *this;
}

OutputStream& OutputStream::operator << (int16_t value) {
    return *this << (int32_t) value;
}

OutputStream& OutputStream::operator << (uint16_t value) {
    return *this << (uint32_t) value;
}

OutputStream& OutputStream::operator << (int32_t value) {
    if (value < 0) {
        write('-');
        value = -value;
    }

    return *this << (uint32_t) value;
}

OutputStream& OutputStream::operator << (uint32_t value) {
    uint32_t div;
    char digit;
    uint8_t currentBase = base.get();

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

OutputStream& OutputStream::operator << (void *ptr) {
    return *this << reinterpret_cast<uint32_t>(ptr);
}

OutputStream& OutputStream::operator << (OutputStream& (*f) (OutputStream&)) {
    return f(*this);
}

OutputStream& OutputStream::endl(OutputStream& stream) {
    stream << '\n';
    stream.flush ();
    return stream;
}

OutputStream& OutputStream::bin(OutputStream& stream) {
    stream.base.set(2);
    return stream;
}

OutputStream& OutputStream::oct(OutputStream& stream) {
    stream.base.set(8);
    return stream;
}

OutputStream& OutputStream::dec(OutputStream& stream) {
    stream.base.set(10);
    return stream;
}

OutputStream& OutputStream::hex(OutputStream& stream) {
    stream.base.set(16);
    return stream;
}

}