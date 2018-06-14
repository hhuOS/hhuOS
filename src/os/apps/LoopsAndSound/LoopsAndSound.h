#ifndef HHUOS_LOOPSANDSOUND_H
#define HHUOS_LOOPSANDSOUND_H

#include <kernel/threads/Thread.h>
#include <kernel/events/Receiver.h>
#include <kernel/services/EventBus.h>

/**
 * A simple application, that demonstrates the multi-threading capabilities of hhuOS by showing two counters, while
 * simultaneously playing melodies on the speaker.
 */
class LoopsAndSound : public Thread, Receiver {

private:

    EventBus *eventBus = nullptr;

    bool isRunning = true;

public:

    /**
     * Constructor.
     */
    LoopsAndSound();

    /**
     * Copy-constructor.
     */
    LoopsAndSound(const LoopsAndSound &copy) = delete;

    /**
     * Destructor.
     */
    ~LoopsAndSound() override;

    /**
     * Overriding function from Receiver.
     */
    void onEvent(const Event &event) override;

    /**
     * Overriding function from Thread.
     */
    void run() override;
};

#endif
