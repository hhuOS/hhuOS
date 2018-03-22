#ifndef __Clear_include__
#define __Clear_include__

#include <lib/OutputStream.h>
#include <lib/String.h>
#include "Command.h"

class Clear : public Command {

public:
    /**
     * Default-constructor.
     */
    Clear() = delete;

    /**
     * Copy-constructor.
     */
    Clear(const Clear &copy) = delete;

    explicit Clear(Shell &shell);

    /**
     * Destructor.
     */
    ~Clear() override = default;

    void execute(Util::Array<String> &args, OutputStream &outputStream) override;
};

#endif