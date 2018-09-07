#ifndef HHUOS_WAVPLAY_H
#define HHUOS_WAVPLAY_H

#include <lib/file/bmp/Bmp.h>
#include "Command.h"

class WavPlay : public Command, Receiver {

public:
    /**
     * Default-constructor.
     */
    WavPlay() = delete;

    /**
     * Copy-constructor.
     */
    WavPlay(const WavPlay &copy) = delete;

    /**
     * Constructor.
     *
     * @param shell The shell, that executes this command
     */
    explicit WavPlay(Shell &shell);

    /**
     * Destructor.
     */
    ~WavPlay() override = default;

    /**
     * Overriding function from Command.
     */
    void execute(Util::Array<String> &args) override;

    /**
     * Overriding function from Command.
     */
    const String getHelpText() override;

    /**
     * Overriding function from Receiver.
     */
    void onEvent(const Event &event) override;
};

#endif