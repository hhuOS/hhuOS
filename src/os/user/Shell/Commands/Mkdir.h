#ifndef __Mkdir_include__
#define __Mkdir_include__

#include <lib/OutputStream.h>
#include <lib/String.h>
#include "Command.h"

class Mkdir : public Command {

private:
    FileSystem * fileSystem = nullptr;

public:
    /**
     * Default-constructor.
     */
    Mkdir() = delete;

    /**
     * Copy-constructor.
     */
    Mkdir(const Mkdir &copy) = delete;

    explicit Mkdir(Shell &shell);

    /**
     * Destructor.
     */
    ~Mkdir() override = default;

    void execute(Util::Array<String> &args, OutputStream &outputStream) override;
};

#endif