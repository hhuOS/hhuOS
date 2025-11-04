#include "InstanceFactory.h"

#include "util/collection/HashMap.h"
#include "util/reflection/Prototype.h"

namespace Util::Reflection {

HashMap<String, Prototype*> InstanceFactory::prototypeTable;

bool InstanceFactory::isPrototypeRegistered(const String &className) {
    return prototypeTable.containsKey(className);
}

void InstanceFactory::registerPrototype(Prototype *prototype) {
    String key = prototype->getClassName();
    prototypeTable.put(key, prototype);
}

void InstanceFactory::deregisterPrototype(const String &className) {
    if (prototypeTable.containsKey(className)) {
        delete prototypeTable.get(className);
        prototypeTable.remove(className);
    }
}

}