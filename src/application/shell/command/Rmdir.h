#ifndef __Rmdir_include__
#define __Rmdir_include__

#include "lib/stream/OutputStream.h"
#include "lib/string/String.h"
#include "lib/file/FileStatus.h"
#include "Command.h"

/** 
 * Implementation of Command.
 * Deletes empty directory.
 *
 * -h, --help: Show help message
 *
 * @author Namit Shah, Martand Javia, Harvish Jariwala
 * @date 2020
 */
class Rmdir : public Command {

private:
    Filesystem *fileSystem = nullptr;
    void deleteFile(const String &progName, FileStatus &fStat);
    void deleteDirectory(const String &progName, Directory &dir);

public:
    /**
     * Default-constructor.
     */
    Rmdir() = delete;
    /**
     * Copy-constructor.
     */
    Rmdir(const Rmdir &copy) = delete;
    /**
     * Constructor.
     *
     * @param shell The shell, that executes this command
     */
    explicit Rmdir(Shell &shell);
    /**
     * Destructor.
     */
    ~Rmdir() override = default;
    /**
     * Overriding function from Command.
     */
    void execute(Util::Array<String> &args) override;
    /**
     * Overriding function from Command.
     */
    const String getHelpText() override;
};
#endif