#ifndef __KernelEntry_include__
#define __KernelEntry_include__


#include <cstdint>
#include <kernel/Bootscreen.h>

class GatesOfHell {

public:

    GatesOfHell() = delete;

    virtual ~GatesOfHell() = delete;

    GatesOfHell(const GatesOfHell &other) = delete;

    GatesOfHell &operator=(const GatesOfHell &other) = delete;

    static int32_t enter();

private:

    static void registerServices();

    static void initializeGraphics();

    static void initializeSerialPorts();

    static void initializePciDrivers();

    static void loadInitrd();

    static Logger &log;

    static LinearFrameBuffer *lfb;

    static TextDriver *text;

    static EventBus *eventBus;

    static Bootscreen *bootscreen;

    static IdleThread *idleThread;

    static uint16_t xres;

    static uint16_t yres;

    static uint8_t bpp;
};


#endif
