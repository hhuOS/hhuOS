#include <lib/file/File.h>
#include "Command.h"

Command::Command(Shell &shell) : shell(shell), stderr(*File::open("/dev/stderr", "w")) {

}

String Command::calcAbsolutePath(const String &relativePath) {
    String absolutePath = shell.getCurrentWorkingDirectory().getAbsolutePath();

    if (!absolutePath.endsWith(FileSystem::SEPARATOR)) {
        absolutePath += FileSystem::SEPARATOR;
    }

    absolutePath += relativePath;

    return absolutePath;
}
