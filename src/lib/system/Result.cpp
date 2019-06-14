#include "Result.h"

namespace Standard::System {

Result::Result() : status(0), value(0) {}

uint32_t Result::getStatus() const {
    return status;
}

uint32_t Result::getValue() const {
    return value;
}

void Result::setStatus(uint32_t status) {
    this->status = status;
}

void Result::setValue(uint32_t value) {
    this->value = value;
}

}