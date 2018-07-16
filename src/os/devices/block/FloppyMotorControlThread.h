#ifndef HHUOS_FLOPPYMOTORCONTROLTHREAD_H
#define HHUOS_FLOPPYMOTORCONTROLTHREAD_H

#include <kernel/threads/Thread.h>
#include <devices/block/storage/FloppyDevice.h>

class FloppyMotorControlThread : public Thread {

    friend class FloppyController;

private:

    FloppyDevice &device;

    uint32_t timeout;

    TimeService *timeService = nullptr;

public:

    /**
     * Constructor.
     */
    explicit FloppyMotorControlThread(FloppyDevice &device);

    /**
     * Copy-constructor.
     */
    FloppyMotorControlThread(const FloppyMotorControlThread &copy) = delete;

    /**
     * Destructor.
     */
    ~FloppyMotorControlThread() override = default;

    /**
     * Overriding function from Thread.
     */
    void run() override;

};

#endif
