#include <lib/file/FileStatus.h>
#include "Cd.h"

Cd::Cd(Shell &shell) : Command(shell) {

};

void Cd::execute(Util::Array<String> &args, OutputStream &outputStream) {
    Util::ArrayList<String> paths;

    for(uint32_t i = 1; i < args.length(); i++) {
        if(!args[i].beginsWith("-") || args[i] == "-") {
            paths.add(args[i]);
        } else if(args[i] == "-h" || args[i] == "--help") {
            outputStream << "Changes the working directory." << endl << endl;
            outputStream << "Usage: " << args[0] << " [OPTION]... [PATH]" << endl << endl;
            outputStream << "Options:" << endl;
            outputStream << "  -h, --help: Show this help-message." << endl;
            return;
        } else {
            stderr << args[0] << ": Invalid option '" << args[i] << "'!" << endl;
            return;
        }
    }

    if(paths.size() > 1) {
        stderr << args[0] << ": Too many arguments!" << endl;
        return;
    }

    String absolutePath = calcAbsolutePath(paths.get(0));

    if(!FileStatus::exists(absolutePath)) {
        stderr << args[0] << ": '" << paths.get(0) << "': Directory not found!" << endl;
        return;
    }

    FileStatus &fStat = *FileStatus::stat(absolutePath);

    if(fStat.getFileType() != FsNode::DIRECTORY_FILE) {
        stderr << args[0] << ": '" << paths.get(0) << "': Not a directory!" << endl;
    } else {
        shell.setCurrentWorkingDirectory(Directory::open(absolutePath));
    }

    delete &fStat;
}