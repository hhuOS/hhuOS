#include <util/memory/Address.h>
#include "ByteArrayOutputStream.h"

namespace Util::Stream {

ByteArrayOutputStream::ByteArrayOutputStream() : ByteArrayOutputStream(DEFAULT_BUFFER_SIZE) {

}

ByteArrayOutputStream::ByteArrayOutputStream(uint32_t size) : buffer(new uint8_t[size]), size(size) {

}

ByteArrayOutputStream::~ByteArrayOutputStream() {
    delete[] buffer;
}

void ByteArrayOutputStream::getContent(uint8_t *target, uint32_t length) const {
    Memory::Address<uint32_t> sourceAddress(buffer);
    Memory::Address<uint32_t> targetAddress(target);

    targetAddress.copyRange(sourceAddress, position > length ? length : position);
}

uint32_t ByteArrayOutputStream::getSize() const {
    return position;
}

void ByteArrayOutputStream::reset() {
    position = 0;
}

void ByteArrayOutputStream::write(uint8_t c) {
    ensureRemainingCapacity(1);
    buffer[position++] = c;
}

void ByteArrayOutputStream::write(const uint8_t *source, uint32_t offset, uint32_t length) {
    if (offset < 0 || length < 0) {
        Device::Cpu::throwException(Device::Cpu::Exception::OUT_OF_BOUNDS, "ByteArrayOutputStream: Negative offset or size!");
    }

    ensureRemainingCapacity(length);
    Memory::Address<uint32_t> sourceAddress(source + offset);
    Memory::Address<uint32_t> targetAddress(buffer + position);
    targetAddress.copyRange(sourceAddress, length);

    position += length;
}

void ByteArrayOutputStream::ensureRemainingCapacity(uint32_t count) {
    if (position + count < size) {
        return;
    }

    uint32_t newSize = size * 2;
    while (newSize < position + count) {
        newSize *= 2;
    }

    auto *newBuffer = new uint8_t[newSize];
    Memory::Address<uint32_t> source(buffer);
    Memory::Address<uint32_t> target(newBuffer);
    target.copyRange(source, position);

    delete[] buffer;
    buffer = newBuffer;
    size = newSize;
}

}