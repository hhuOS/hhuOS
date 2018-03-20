#ifndef __PIT_include__
#define __PIT_include__

#include "kernel/interrupts/InterruptHandler.h"
#include "kernel/IOport.h"
#include "kernel/Kernel.h"
#include "kernel/services/GraphicsService.h"
#include "kernel/services/TimeService.h"
#include "devices/graphics/text/TextDriver.h"

class Pit : public InterruptHandler {

public:

    Pit(const Pit &copy) = delete;

    Pit &operator=(const Pit &copy) = delete;

    ~Pit() override = default;

    /**
     * Returns an instance of the PIT.
     *
     * @return An instance of the PIT
     */
    static Pit *getInstance();

    /**
     * Returns the interval at which the PIT fires it's interrupts.
     *
     * @return The PIT's interval in microseconds
     */
    uint32_t getInterval();

    /**
     * Sets the interval at which the PIT fires it's interrupts.
     *
     * @param us The PIT's interval in microseconds
     */
    void setInterval(uint32_t us);

    void plugin ();
    
    void trigger () override;

private:

    explicit Pit (uint32_t us);

    static Pit *instance;

    TimeService *timeService;

    GraphicsService *graphicsService;

    uint32_t timerInterval;

    static const uint32_t TIME_BASE = 838;

    static const uint32_t DEFAULT_INTERVAL = 10000;
};

#endif
