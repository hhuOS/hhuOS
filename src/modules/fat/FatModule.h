#ifndef __FatModule_include__
#define __FatModule_include__

#include <kernel/Module.h>

class FatModule : public Module {

public:

    FatModule() = default;

    int32_t initialize() override;

    int32_t finalize() override;

    String getName() override;

    Util::Array<String> getDependencies() override;
};


#endif
 
