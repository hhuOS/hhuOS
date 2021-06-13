//
// Created by hannes on 13.06.21.
//

#ifndef HHUOS_SENDTEXT_H
#define HHUOS_SENDTEXT_H


#include "Command.h"

class SendText : public Command {
private:
    uint16_t serverPort = 16000;

/**
 * Implementation of Command.
 *
 * This application provides UDP/IP protocol stack testing by sending and receiving text over it
 */

public:

    /**
     * Default-constructor.
     */
    SendText() = delete;

    /**
     * Copy-constructor.
     */
    SendText(const SendText &copy) = delete;

    /**
     * Constructor.
     *
     * @param shell The shell, that executes this command
     */
    explicit SendText(Shell &shell);

    /**
     * Overriding function from Command.
     */
    void execute(Util::Array<String> &args) override;

    /**
     * Overriding function from Command.
     */
    const String getHelpText() override;
};


#endif //HHUOS_SENDTEXT_H
