#include "FatModule.h"
#include "FatDriver.h"

MODULE_PROVIDER {

    return new FatModule();
};

int32_t FatModule::initialize() {

    FsDriver::registerDriverType("Fat", new FatDriver());

    return 0;
}

int32_t FatModule::finalize() {

    return 0;
}

String FatModule::getName() {

    return "FatDriver";
}

Util::Array<String> FatModule::getDependencies() {

    return Util::Array<String>(0);
}
