#ifndef HHUOS_FLOPPYMOTORCONTROLTHREAD_H
#define HHUOS_FLOPPYMOTORCONTROLTHREAD_H

#include <kernel/threads/Thread.h>
#include <devices/block/storage/FloppyDevice.h>

/**
 * Runs in background and controls the state of a floppy drive's motor.
 */
class FloppyMotorControlThread : public Thread {

    friend class FloppyController;

private:

    FloppyDevice &device;

    uint32_t timeout;

    TimeService *timeService = nullptr;

public:

    /**
     * Constructor.
     *
     * @param device The device
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
