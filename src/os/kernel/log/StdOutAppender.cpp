#include <lib/libc/printf.h>
#include "StdOutAppender.h"

StdOutAppender::StdOutAppender() {

}

void StdOutAppender::append(const String &message) {

    *stdout << (char*) message << endl;
}
