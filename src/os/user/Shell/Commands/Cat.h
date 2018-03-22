#ifndef __Cat_include__
#define __Cat_include__

#include <lib/OutputStream.h>
#include <lib/String.h>
#include "Command.h"

class Cat : public Command {

public:
    /**
     * Default-constructor.
     */
    Cat() = delete;

    /**
     * Copy-constructor.
     */
    Cat(const Cat &copy) = delete;

    explicit Cat(Shell &shell);

    /**
     * Destructor.
     */
    ~Cat() override = default;

    void execute(Util::Array<String> &args, OutputStream &outputStream) override;
};

#endif