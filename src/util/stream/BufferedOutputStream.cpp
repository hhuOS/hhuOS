#include <util/memory/Address.h>
#include <device/cpu/Cpu.h>
#include "BufferedOutputStream.h"

namespace Util::Stream {

BufferedOutputStream::BufferedOutputStream(OutputStream &stream) : BufferedOutputStream(stream, DEFAULT_BUFFER_SIZE) {}

BufferedOutputStream::BufferedOutputStream(OutputStream &stream, uint32_t size): FilterOutputStream(stream), buffer(new uint8_t[size]), size(size) {}

BufferedOutputStream::~BufferedOutputStream() {
    delete[] buffer;
}

void BufferedOutputStream::write(uint8_t c) {
    if (position == size) {
        flush();
    }

    buffer[position++] = c;
}

void BufferedOutputStream::write(const uint8_t *source, uint32_t offset, uint32_t length) {
    if (offset < 0 || length < 0) {
        Device::Cpu::throwException(Device::Cpu::Exception::OUT_OF_BOUNDS, "BufferedOutputStream: Negative offset or size!");
    }

    if (length < (size - position)) {
        Memory::Address<uint32_t> sourceAddress(source + offset);
        Memory::Address<uint32_t> targetAddress(buffer + position);

        targetAddress.copyRange(sourceAddress, length);
        position += length;
    } else {
        flush();
        FilterOutputStream::write(source, offset, length);
    }
}

void BufferedOutputStream::flush() {
    FilterOutputStream::write(buffer, 0, position);
    position = 0;
    FilterOutputStream::flush();
}

void BufferedOutputStream::close() {
    FilterOutputStream::close();
}
}