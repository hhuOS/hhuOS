#include <lib/file/FileStatus.h>
#include "Ls.h"

Ls::Ls(Shell &shell) : Command(shell) {

};

void Ls::execute(Util::Array<String> &args, OutputStream &outputStream) {
    Util::ArrayList<String> paths;

    for(uint32_t i = 1; i < args.length(); i++) {
        if(!args[i].beginsWith("-") || args[i] == "-") {
            paths.add(args[i]);
        } else if(args[i] == "-h" || args[i] == "--help") {
            outputStream << "Lists the content of directories. Default directory is the working directory" << endl << endl;
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
        Directory &dir = shell.getCurrentWorkingDirectory();

        for(const String &name : dir.getChildren()) {
            outputStream << "  " << name;

            FileStatus &childStat = *FileStatus::stat(dir.getAbsolutePath() + "/" + name);
            if (childStat.getFileType() == FsNode::DIRECTORY_FILE) {
                outputStream << "/";
            }

            delete &childStat;
            outputStream << endl;
        }

        return;
    }

    for(const String &path : paths) {
        String absolutePath = calcAbsolutePath(path);

        if (FileStatus::exists(absolutePath)) {
            FileStatus &fStat = *FileStatus::stat(absolutePath);

            if (fStat.getFileType() != FsNode::DIRECTORY_FILE) {
                outputStream << fStat.getName() << endl;

                if(paths.size() > 1) {
                    outputStream << endl;
                }
            } else {
                if(paths.size() > 1) {
                    outputStream << path << ":" << endl;
                }

                Directory &dir = *Directory::open(absolutePath);

                for (const String &name : dir.getChildren()) {
                    outputStream << "  " << name;

                    FileStatus &childStat = *FileStatus::stat(dir.getAbsolutePath() + "/" + name);

                    if (childStat.getFileType() == FsNode::DIRECTORY_FILE) {
                        outputStream << "/";
                    }

                    delete &childStat;
                    outputStream << endl;
                }

                if(paths.size() > 1) {
                    outputStream << endl;
                }

                delete &dir;
            }

            delete &fStat;
        } else {
            stderr << args[0] << ": File or Directory not found!" << endl;
        }
    }
}