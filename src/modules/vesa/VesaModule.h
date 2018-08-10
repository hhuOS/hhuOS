#ifndef __VesaModule_include__
#define __VesaModule_include__

#include <kernel/Module.h>

class VesaModule : public Module {

public:

    VesaModule() = default;

    int32_t initialize() override;

    int32_t finalize() override;

    String getName() override;

    Util::Array<String> getDependencies() override;
};


#endif
 
