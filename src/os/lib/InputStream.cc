#include "InputStream.h"

InputStream& InputStream::operator >> (unsigned char &c) {
    return *this >> (char&) c;
}