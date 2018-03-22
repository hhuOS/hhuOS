#ifndef __Echo_include__
#define __Echo_include__

#include <lib/OutputStream.h>
#include <lib/String.h>
#include "Command.h"

class Echo : public Command {

public:
    /**
     * Default-constructor.
     */
    Echo() = delete;

    /**
     * Copy-constructor.
     */
    Echo(const Echo &copy) = delete;

    explicit Echo(Shell &shell);

    /**
     * Destructor.
     */
    ~Echo() override = default;

    void execute(Util::Array<String> &args, OutputStream &outputStream) override;
};

#endif