#include "FilterInputStream.h"

namespace Util::Stream {

FilterInputStream::FilterInputStream(InputStream &stream) : stream(stream) {}

int16_t FilterInputStream::read() {
    return stream.read();
}

uint32_t FilterInputStream::read(uint8_t *target, uint32_t offset, uint32_t length) {
    return stream.read(target, offset, length);
}

void FilterInputStream::close() {
    stream.close();
}

}