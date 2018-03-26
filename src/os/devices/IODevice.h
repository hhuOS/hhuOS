#ifndef __IODevice_include__
#define __IODevice_include__

#include "kernel/interrupts/InterruptHandler.h"

class IODevice : public InterruptHandler {

public:
    /**
     * Constructor.
     */
    IODevice() = default;

    /**
     * Copy-constructor.
     */
    IODevice(const IODevice &copy) = delete;

    /**
     * Destructor.
     */
    ~IODevice() override = default;

    virtual bool checkForData() = 0;

    void trigger() override = 0;
};

#endif
