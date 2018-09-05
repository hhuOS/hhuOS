#ifndef __Sound_include__
#define __Sound_include__

#include "kernel/threads/Thread.h"
#include "devices/sound/PcSpeaker.h"

/**
 * A simple thread, which plays a melody in an infinite loop.
 *
 * @author Burak Akguel, Christian Gesse, Fabian Ruhland, Filip Krakowski, Michael Schoettner
 * @date 2016, 2018
 */
class Sound : public Thread {

private:

    PcSpeaker *speaker = nullptr;

    TimeService * timeService = nullptr;

    bool isRunning = true;

public:

    /**
     * Constructor.
     */
    explicit Sound();

    /**
     * Copy-constructor.
     */
    Sound(const Sound &copy) = delete;

    /**
     * Destructor.
     */
    ~Sound() override;

    /**
     * Overriding function from Thread.
     */
    void run() override;
};

#endif
