#ifndef __NullYielder_include__
#define __NullYielder_include__


#include "Yieldable.h"

class NullYielder : public Yieldable {

public:

    NullYielder() noexcept = default;

    ~NullYielder() = default;

    void yield() override;
};


#endif
