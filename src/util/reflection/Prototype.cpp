#include "Prototype.h"

Util::Reflection::Prototype *Util::Reflection::Prototype::clone() const {
    Device::Cpu::throwException(Device::Cpu::Exception::UNSUPPORTED_OPERATION, "Prototype class does not implement clone()!");
}
