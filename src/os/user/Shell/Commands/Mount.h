#ifndef __Mount_include__
#define __Mount_include__

#include <lib/OutputStream.h>
#include <lib/String.h>
#include "Command.h"

class Mount : public Command {

public:
    /**
     * Default-constructor.
     */
    Mount() = delete;

    /**
     * Copy-constructor.
     */
    Mount(const Mount &copy) = delete;

    explicit Mount(Shell &shell);

    /**
     * Destructor.
     */
    ~Mount() override = default;

    void execute(Util::Array<String> &args, OutputStream &outputStream) override;
};

#endif