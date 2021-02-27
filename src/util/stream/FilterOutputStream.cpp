#include "FilterOutputStream.h"

namespace Util::Stream {

FilterOutputStream::FilterOutputStream(OutputStream &stream) : stream(stream) {}

void FilterOutputStream::write(uint8_t c) {
    stream.write(c);
}

void FilterOutputStream::write(const uint8_t *source, uint32_t offset, uint32_t length) {
    stream.write(source, offset, length);
}

void FilterOutputStream::flush() {
    stream.flush();
}

void FilterOutputStream::close() {
    stream.close();
}

}