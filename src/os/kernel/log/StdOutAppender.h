#ifndef __StdOutAppender_include__
#define __StdOutAppender_include__


#include <lib/String.h>
#include "Appender.h"

class StdOutAppender : public Appender {

public:

    explicit StdOutAppender();

    StdOutAppender(const StdOutAppender &other) = delete;

    StdOutAppender &operator=(const StdOutAppender &other) = delete;

    ~StdOutAppender() = default;

    void append(const String &message) override;
};


#endif
