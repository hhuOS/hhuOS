#ifndef __Random_include__
#define __Random_include__


#include "kernel/Module.h"

class Random : public Module {

public:

    Random() = default;

    int32_t initialize() override;

    int32_t finalize() override;

    String getName() override;

    Util::Array<String> getDependencies() override;

};


#endif
