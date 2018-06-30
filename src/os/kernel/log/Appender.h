#ifndef __Appender_include__
#define __Appender_include__

#include <lib/String.h>

class Appender {

public:

    Appender() = default;

    virtual ~Appender() = default;

    virtual void append(const String &message) = 0;
};

#endif
