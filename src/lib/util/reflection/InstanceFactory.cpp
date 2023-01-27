#include "InstanceFactory.h"

#include "lib/util/collection/Array.h"
#include "lib/util/collection/Collection.h"
#include "lib/util/collection/HashMap.h"
#include "lib/util/collection/Iterator.h"
#include "lib/util/reflection/Prototype.h"

namespace Util::Reflection {

Util::HashMap<String, Prototype*> InstanceFactory::prototypeTable;

Prototype* InstanceFactory::createInstance(const String &type) {
    if (prototypeTable.containsKey(type)) {
        return prototypeTable.get(type)->clone();
    }

    return nullptr;
}

void InstanceFactory::registerPrototype(Prototype *prototype) {
    String key = prototype->getClassName();
    prototypeTable.put(key, prototype);
}

void InstanceFactory::deregisterPrototype(const String &type) {
    if (prototypeTable.containsKey(type)) {
        delete prototypeTable.get(type);
        prototypeTable.remove(type);
    }
}

}