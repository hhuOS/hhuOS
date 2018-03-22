#include "Umount.h"

Umount::Umount(Shell &shell) : Command(shell) {

};

void Umount::execute(Util::Array<String> &args, OutputStream &outputStream) {
    String targetPath;

    for(uint32_t i = 1; i < args.length(); i++) {
        if(!args[i].beginsWith("-") || args[i] == "-") {
            if(targetPath.isEmpty()) {
                targetPath = args[i];
            } else {
                stderr << args[0] << ": Too many arguments!" << endl;
                return;
            }
        } else if(args[i] == "-h" || args[i] == "--help") {
            outputStream << "Unmounts a device from a target path." << endl << endl;
            outputStream << "Usage: " << args[0] << " [PATH]" << endl << endl;
            outputStream << "Options:" << endl;
            outputStream << "  -h, --help: Show this help-message." << endl;
            return;
        } else {
            stderr << args[0] << ": Invalid option '" << args[i] << "'!" << endl;
            return;
        }
    }

    if(targetPath.isEmpty()) {
        stderr << args[0] << ": No target given!" << endl;
        return;
    }

    targetPath = calcAbsolutePath(targetPath);

    auto *fileSystem = Kernel::getService<FileSystem>();
    auto ret = fileSystem->unmount(targetPath);

    switch(ret) {
        case FileSystem::SUCCESS :
            break;
        case FileSystem::FILE_NOT_FOUND :
            stderr << args[0] << ": '" << targetPath << "': Directory not found!" << endl;
            return;
        case FileSystem::NOTHING_MOUNTED_AT_PATH :
            stderr << args[0] << ": '" << targetPath << "': No device mounted at target path!" << endl;
            return;
        default:
            break;
    }
}