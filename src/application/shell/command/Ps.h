#ifndef __Ps_include__
#define __Ps_include__
#include "lib/stream/OutputStream.h"
#include "lib/string/String.h"
#include "Command.h"
/**
 * Implementation of Command.
 * Displays all the threads of the ready queue
 *
 * -h, --help: Show help message
 *
 * @author Namit Shah, Martand Javia & Harvish Jariwala
 * @date 2020
 */
class Ps : public Command {

public:
    /**
     * Default-constructor.
     */
    Ps() = delete;
    /**
     * Copy-constructor.
     */
    Ps(const Ps &copy) = delete;
    /**
     * Constructor.
     *
     * @param shell The shell, that executes this command
     */
    explicit Ps(Shell &shell);
    /**
     * Destructor.
     */
    ~Ps() override = default;
    /**
     * Overriding function from Command.
     */
    void execute(Util::Array<String> &args) override;
    /**
     * Overriding function from Command.
     */
    const String getHelpText() override;
};

#endif