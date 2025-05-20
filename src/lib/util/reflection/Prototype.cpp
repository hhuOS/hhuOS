#include "Prototype.h"

#include "lib/util/base/Panic.h"

Util::Reflection::Prototype *Util::Reflection::Prototype::clone() const {
    Panic::fire(Panic::UNSUPPORTED_OPERATION, "Prototype class does not implement clone()!");
}
