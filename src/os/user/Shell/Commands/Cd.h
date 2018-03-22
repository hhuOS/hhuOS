#ifndef __Cd_include__
#define __Cd_include__

#include <lib/OutputStream.h>
#include <lib/String.h>
#include "Command.h"

class Cd : public Command {

public:
    /**
     * Default-constructor.
     */
    Cd() = delete;

    /**
     * Copy-constructor.
     */
    Cd(const Cd &copy) = delete;

    explicit Cd(Shell &shell);

    /**
     * Destructor.
     */
    ~Cd() override = default;

    void execute(Util::Array<String> &args, OutputStream &outputStream) override;
};

#endif