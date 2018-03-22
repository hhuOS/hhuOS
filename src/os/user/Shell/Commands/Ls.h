#ifndef __Ls_include__
#define __Ls_include__

#include <lib/OutputStream.h>
#include <lib/String.h>
#include "Command.h"

class Ls : public Command {

public:
    /**
     * Default-constructor.
     */
    Ls() = delete;

    /**
     * Copy-constructor.
     */
    Ls(const Ls &copy) = delete;

    explicit Ls(Shell &shell);

    /**
     * Destructor.
     */
    ~Ls() override = default;

    void execute(Util::Array<String> &args, OutputStream &outputStream) override;
};

#endif