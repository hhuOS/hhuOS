#ifndef HHUOS_STATICHEAPMODULE_H
#define HHUOS_STATICHEAPMODULE_H

#include <kernel/Module.h>

class StaticHeapModule : public Module {

public:

    StaticHeapModule() = default;

    int32_t initialize() override;

    int32_t finalize() override;

    String getName() override;

    Util::Array<String> getDependencies() override;
};

#endif
