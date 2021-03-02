#include "InstanceFactory.h"

namespace Util::Reflection {

Util::Data::HashMap<Memory::String, Prototype*> InstanceFactory::prototypeTable;

Prototype* InstanceFactory::createInstance(const Memory::String &type) {
    if (prototypeTable.containsKey(type)) {
        return prototypeTable.get(type)->clone();
    }

    Device::Cpu::throwException(Device::Cpu::Exception::CLASS_NOT_FOUND, "Prototype class not found!");
}

void InstanceFactory::registerPrototype(Prototype *prototype) {
    Memory::String key = prototype->getClassName();
    prototypeTable.put(key, prototype);
}

void InstanceFactory::deregisterPrototype(const Memory::String &type) {
    if (prototypeTable.containsKey(type)) {
        delete prototypeTable.get(type);
        prototypeTable.remove(type);
    }
}

}