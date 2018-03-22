#include <lib/file/FileStatus.h>
#include <lib/file/File.h>
#include "Touch.h"

Touch::Touch(Shell &shell) : Command(shell) {

};

void Touch::execute(Util::Array<String> &args, OutputStream &outputStream) {
    Util::ArrayList<String> paths;

    for(uint32_t i = 1; i < args.length(); i++) {
        if(!args[i].beginsWith("-") || args[i] == "-") {
            paths.add(args[i]);
        } else if(args[i] == "-h" || args[i] == "--help") {
            outputStream << "Creates new files." << endl << endl;
            outputStream << "Usage: " << args[0] << " [OPTION]... [PATH]..." << endl << endl;
            outputStream << "Options:" << endl;
            outputStream << "  -h, --help: Show this help-message." << endl;
            return;
        } else {
            stderr << args[0] << ": Invalid option '" << args[i] << "'!" << endl;
            return;
        }
    }

    if(paths.size() == 0) {
        stderr << args[0] << ": Missing operand!" << endl;
        return;
    }

    fileSystem = Kernel::getService<FileSystem>();

    for(const String &path : paths) {
        String absolutePath = calcAbsolutePath(path);

        if (!FileStatus::exists(path)) {
            if (fileSystem->createFile(absolutePath) != FileSystem::SUCCESS) {
                stderr << args[0] << ": '" << path << "': Unable to create file!" << endl;
            }
        }
    }
}