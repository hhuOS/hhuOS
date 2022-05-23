#include "InstanceFactory.h"

namespace Util::Reflection {

Util::Data::HashMap<Memory::String, Prototype*> InstanceFactory::prototypeTable;

Prototype* InstanceFactory::createInstance(const Memory::String &type) {
    if (prototypeTable.containsKey(type)) {
        return prototypeTable.get(type)->clone();
    }

    return nullptr;
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