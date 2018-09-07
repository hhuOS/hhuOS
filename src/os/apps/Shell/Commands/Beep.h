#ifndef HHUOS_BEEP_H
#define HHUOS_BEEP_H

#include <lib/file/beep/BeepFile.h>
#include "Command.h"

class Beep : public Command, Receiver {

private:

    BeepFile *beepFile = nullptr;

    bool isRunning;

public:
    /**
     * Default-constructor.
     */
    Beep() = delete;

    /**
     * Copy-constructor.
     */
    Beep(const Beep &copy) = delete;

    /**
     * Constructor.
     *
     * @param shell The shell, that executes this command
     */
    explicit Beep(Shell &shell);

    /**
     * Destructor.
     */
    ~Beep() override = default;

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
