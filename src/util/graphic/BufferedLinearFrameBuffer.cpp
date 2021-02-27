#include <util/memory/Address.h>
#include "BufferedLinearFrameBuffer.h"

namespace Util::Graphic {

BufferedLinearFrameBuffer::BufferedLinearFrameBuffer(void *address, uint16_t resolutionX, uint16_t resolutionY, uint8_t bitsPerPixel,
                                                     uint16_t pitch) :
        LinearFrameBuffer(address, resolutionX, resolutionY, bitsPerPixel, pitch), softwareBuffer(new uint8_t[pitch * resolutionY]) {
    Memory::Address<uint32_t>(softwareBuffer).setRange(0, pitch * resolutionY);
}

BufferedLinearFrameBuffer::~BufferedLinearFrameBuffer() {
    delete[] softwareBuffer;
}

uint8_t *BufferedLinearFrameBuffer::getBuffer() const {
    return softwareBuffer;
}

void BufferedLinearFrameBuffer::flush() const {
    auto source = Util::Memory::Address<uint32_t>(softwareBuffer);
    auto destination = Util::Memory::Address<uint32_t>(LinearFrameBuffer::getBuffer());

    destination.copyRange(source, getPitch() * getResolutionY());
}

}