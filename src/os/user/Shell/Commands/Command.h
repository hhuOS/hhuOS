#ifndef __Command_include__
#define __Command_include__

#include <cstdint>
#include <user/Shell/Shell.h>

class Command {

protected:
    Shell &shell;

    OutputStream &stderr;

    String calcAbsolutePath(const String &relativePath);

public:
    /**
     * Default-constructor.
     */
    Command() = delete;

    /**
     * Copy-constructor.
     */
    Command(const Command &copy) = delete;

    explicit Command(Shell &shell);

    /**
     * Destructor.
     */
    virtual ~Command() = default;

    virtual void execute(Util::Array<String> &args, OutputStream &outputStream) = 0;
};

#endif