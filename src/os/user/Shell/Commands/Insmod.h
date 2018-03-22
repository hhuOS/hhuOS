#ifndef __Insmod_include__
#define __Insmod_include__

#include <lib/OutputStream.h>
#include <lib/String.h>
#include "Command.h"

class Insmod : public Command {

public:
    /**
     * Default-constructor.
     */
    Insmod() = delete;

    /**
     * Copy-constructor.
     */
    Insmod(const Insmod &copy) = delete;

    explicit Insmod(Shell &shell);

    /**
     * Destructor.
     */
    ~Insmod() override = default;

    void execute(Util::Array<String> &args, OutputStream &outputStream) override;
};

#endif