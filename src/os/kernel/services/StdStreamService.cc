#include "StdStreamService.h"

OutputStream *StdStreamService::getStdout() {
    return stdout;
}

void StdStreamService::setStdout(OutputStream *stream) {
    stdout = stream;
}

OutputStream *StdStreamService::getStderr() {
    return stderr;
}

void StdStreamService::setStderr(OutputStream *stream) {
    stderr = stream;
}