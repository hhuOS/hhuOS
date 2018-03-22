#ifndef __Rm_include__
#define __Rm_include__

#include <lib/OutputStream.h>
#include <lib/String.h>
#include <lib/file/FileStatus.h>
#include "Command.h"

class Rm : public Command {

private:
    FileSystem *fileSystem = nullptr;

    void deleteFile(const String &progName, FileStatus &fStat);

    void recursiveDelete(const String &progName, Directory &dir);

public:
    /**
     * Default-constructor.
     */
    Rm() = delete;

    /**
     * Copy-constructor.
     */
    Rm(const Rm &copy) = delete;

    explicit Rm(Shell &shell);

    /**
     * Destructor.
     */
    ~Rm() override = default;

    void execute(Util::Array<String> &args, OutputStream &outputStream) override;
};

#endif