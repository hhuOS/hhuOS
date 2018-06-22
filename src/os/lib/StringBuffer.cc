#include "lib/StringBuffer.h"

StringBuffer::StringBuffer() : pos(0) {

}

void StringBuffer::put (char c) {
    buffer[pos] = c;
    pos++;

    if (pos >= sizeof(buffer)) {
        flush();
    }
}

