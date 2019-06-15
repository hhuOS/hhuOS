#include "Result.h"

namespace Standard::System {

Result::Result() : status(OK) {}

uint32_t Result::getStatus() const {
    return status;
}
void Result::setStatus(Status status) {
    this->status = status;
}

}