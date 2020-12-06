#ifndef __Pwd_include__
#define __Pwd_include__

#include "lib/stream/OutputStream.h"
#include "lib/string/String.h"
#include "Command.h"

/**
 * Implementation of Command.
 * Displays the whole path of the current working directory.
 *
 * -h, --help: Show help message
 *
 * @author Namit Shah, Martand Javia & Harvish Jariwala
 * @date 2020
 */
class Pwd : public Command {

public:
    /**
     * Default-constructor.
     */
    Pwd() = delete;

    /**
     * Copy-constructor.
     */
    Pwd(const Pwd &copy) = delete;

    /**
     * Constructor.
     *
     * @param shell The shell, that executes this command
     */
    explicit Pwd(Shell &shell);

    /**
     * Destructor.
     */
    ~Pwd() override = default;

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