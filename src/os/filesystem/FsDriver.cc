#include <lib/util/HashMap.h>
#include "FsDriver.h"

Util::HashMap<String, FsDriver*> FsDriver::prototypeTable;

FsDriver *FsDriver::createInstance(String type) {
    String key = type.toLowerCase();

    if(prototypeTable.containsKey(key)) {
        return prototypeTable.get(type)->clone();
    }

    Cpu::throwException(Cpu::Exception::UNKNOWN_DRIVER);

    return nullptr;
}

void FsDriver::registerDriverType(String type, FsDriver *driver) {
    String key = type.toLowerCase();

    prototypeTable.put(key, driver);
}