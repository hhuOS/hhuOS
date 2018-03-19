#ifndef __InputStream_include__
#define __InputStream_include__

#include "OutputStream.h"

class InputStream {

private:
    InputStream(const InputStream &copy);

public:

    InputStream() = default;

    ~InputStream() = default;


    InputStream& operator >> (unsigned char &c);

    virtual InputStream& operator >> (char &c) = 0;

    virtual InputStream& operator >> (char *string) = 0;

    virtual InputStream& operator >> (OutputStream &outStream) = 0;
};

#endif