#include "Mount.h"

Mount::Mount(Shell &shell) : Command(shell) {

};

void Mount::execute(Util::Array<String> &args, OutputStream &outputStream) {
    String devicePath;
    String targetPath;
    String type;

    for(uint32_t i = 1; i < args.length(); i++) {
        if(!args[i].beginsWith("-") || args[i] == "-") {
            if(devicePath.isEmpty()) {
                devicePath = args[i];
            } else if(targetPath.isEmpty()) {
                targetPath = args[i];
            } else {
                stderr << args[0] << ": Too many arguments!" << endl;
                return;
            }
        } else if(args[i] == "-t" || args[i] == "--type") {
            if(i == args.length() - 1) {
                stderr << args[0] << ": '" << args[i] << "': This option needs an argument!" << endl;
                return;
            } else {
                type = args[++i];
            }
        } else if(args[i] == "-h" || args[i] == "--help") {
            outputStream << "Mounts a device to a target path." << endl << endl;
            outputStream << "Usage: " << args[0] << " [DEVICE] [TARGET] [OPTIONS]..." << endl << endl;
            outputStream << "Options:" << endl;
            outputStream << "  -t, --type: Filesystem type (REQUIRED!)." << endl;
            outputStream << "  -h, --help: Show this help-message." << endl;
            return;
        } else {
            stderr << args[0] << ": Invalid option '" << args[i] << "'!" << endl;
            return;
        }
    }

    if(type == FileSystem::TYPE_RAM) {
        targetPath = devicePath;
    }

    if(devicePath.isEmpty()) {
        stderr << args[0] << ": No device given!" << endl;
        return;
    }

    if(targetPath.isEmpty()) {
        stderr << args[0] << ": No target given!" << endl;
        return;
    }

    if(type.isEmpty()) {
        stderr << args[0] << ": No filesystem type given!" << endl;
        return;
    }

    String absoluteDevicePath = calcAbsolutePath(devicePath);
    String absoluteTargetPath = calcAbsolutePath(targetPath);

    auto *fileSystem = Kernel::getService<FileSystem>();
    auto ret = fileSystem->mount(absoluteDevicePath, absoluteTargetPath, type);

    switch(ret) {
        case FileSystem::SUCCESS :
            break;
        case FileSystem::DEVICE_NOT_FOUND :
            stderr << args[0] << ": '" << devicePath << "': Device not found!" << endl;
            return;
        case FileSystem::FILE_NOT_FOUND :
            stderr << args[0] << ": '" << targetPath << "': Directory not found!" << endl;
            return;
        case FileSystem::MOUNT_TARGET_ALREADY_USED :
            stderr << args[0] << ": '" << targetPath << "': A device is already mounted to that path!" << endl;
            return;
        case FileSystem::INVALID_DRIVER :
            stderr << args[0] << ": '" << type << "': Unknown filesystem type!" << endl;
            return;
        case FileSystem::MOUNTING_FAILED :
            stderr << args[0] << ": Unable to mount '" << devicePath << "' to '" << targetPath << "'!" << endl;
            return;
        default:
            break;
    }
}