#include "InstanceFactory.h"

#include "lib/util/data/Array.h"
#include "lib/util/data/Collection.h"
#include "lib/util/data/HashMap.h"
#include "lib/util/data/Iterator.h"
#include "lib/util/reflection/Prototype.h"

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