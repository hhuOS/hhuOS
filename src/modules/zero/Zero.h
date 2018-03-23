#ifndef __Zero_include__
#define __Zero_include__


#include "kernel/Module.h"

class Zero : public Module {

public:

    Zero() = default;

    int32_t initialize() override;

    int32_t finalize() override;

    String getName() override;

    Util::Array<String> getDependencies() override;

};


#endif
