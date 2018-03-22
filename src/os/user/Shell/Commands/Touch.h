#ifndef __Touch_include__
#define __Touch_include__

#include <lib/OutputStream.h>
#include <lib/String.h>
#include "Command.h"

class Touch : public Command {

private:
    FileSystem *fileSystem = nullptr;

public:
    /**
     * Default-constructor.
     */
    Touch() = delete;

    /**
     * Copy-constructor.
     */
    Touch(const Touch &copy) = delete;

    explicit Touch(Shell &shell);

    /**
     * Destructor.
     */
    ~Touch() override = default;

    void execute(Util::Array<String> &args, OutputStream &outputStream) override;
};

#endif