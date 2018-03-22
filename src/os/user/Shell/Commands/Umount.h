#ifndef __Umount_include__
#define __Umount_include__

#include <lib/OutputStream.h>
#include <lib/String.h>
#include "Command.h"

class Umount : public Command {

public:
    /**
     * Default-constructor.
     */
    Umount() = delete;

    /**
     * Copy-constructor.
     */
    Umount(const Umount &copy) = delete;

    explicit Umount(Shell &shell);

    /**
     * Destructor.
     */
    ~Umount() override = default;

    void execute(Util::Array<String> &args, OutputStream &outputStream) override;
};

#endif