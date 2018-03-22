#include <lib/file/File.h>
#include <lib/file/FileStatus.h>
#include "Cat.h"

Cat::Cat(Shell &shell) : Command(shell) {

};

void Cat::execute(Util::Array<String> &args, OutputStream &outputStream) {
    Util::ArrayList<String> paths;

    for(uint32_t i = 1; i < args.length(); i++) {
        if(!args[i].beginsWith("-") || args[i] == "-") {
            paths.add(args[i]);
        } else if(args[i] == "-h" || args[i] == "--help") {
            outputStream << "Concatenates multiple files and writes them to the standard output stream." << endl << endl;
            outputStream << "Usage: " << args[0] << " [OPTION]... [FILE]..." << endl << endl;
            outputStream << "Options:" << endl;
            outputStream << "  -h, --help: Show this help-message." << endl;
            return;
        } else {
            stderr << args[0] << ": Invalid option '" << args[i] << "'!" << endl;
            return;
        }
    }

    for(const String &path : paths) {
        String absolutePath = calcAbsolutePath(path);

        if(FileStatus::exists(absolutePath)) {
            FileStatus &fStat = *FileStatus::stat(absolutePath);

            if(fStat.getFileType() == FsNode::DIRECTORY_FILE) {
                stderr << args[0] << ": '" << path << "': Is a directory!" << endl;

                delete &fStat;
                continue;
            }

            delete &fStat;
        } else {
            stderr << args[0] << ": '" << path << "': File or Directory not found!" << endl;
            continue;
        }

        File &file = *File::open(absolutePath, "r");

        file >> outputStream;

        delete &file;
    }
}