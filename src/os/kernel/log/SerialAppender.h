#ifndef __SerialAppender_include__
#define __SerialAppender_include__

#include <devices/Serial.h>
#include "Appender.h"

class SerialAppender : public Appender {

public:

    explicit SerialAppender(Serial &serial);

    SerialAppender(const SerialAppender &other) = delete;

    SerialAppender &operator=(const SerialAppender &other) = delete;

    ~SerialAppender() = default;

    void append(const String &message) override;

private:

    Serial &serial;
};


#endif
