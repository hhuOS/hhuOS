#ifndef __Mathexpr_include__
#define ___Mathexpr_include__

#include "lib/stream/OutputStream.h"
#include "lib/string/String.h"
#include "Command.h"

/**
 * Implementation of Command.
 * Calculated the expression 
 *
 * -h, --help: Show help message
 *
 * @author Namit Shah, Martand Javia, Harvish Jariwala
 * @date 2020
 */
class Mathexpr : public Command {

public:
    /**
     * Default-constructor.
     */
    Mathexpr() = delete;

    /**
     * Copy-constructor.
     */
    Mathexpr(const Mathexpr &copy) = delete;

    /**
     * Constructor.
     *
     * @param shell The shell, that executes this command
     */
    explicit Mathexpr(Shell &shell);

    /**
     * Destructor.
     */
    ~Mathexpr() override = default;

    /**
     * Overriding function from Command.
     */
    void execute(Util::Array<String> &args) override;

    double calculateExpression(String expr);

    /**
     * Overriding function from Command.
     */
    const String getHelpText() override;
};

#endif