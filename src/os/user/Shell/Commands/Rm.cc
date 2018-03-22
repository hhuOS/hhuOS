#include <lib/file/FileStatus.h>
#include "Rm.h"

Rm::Rm(Shell &shell) : Command(shell) {

};

void Rm::recursiveDelete(const String &progName, Directory &dir) {
    for(const String &childName : dir.getChildren()) {
        String absolutePath = dir.getAbsolutePath() + "/" + childName;

        FileStatus &fStat = *FileStatus::stat(absolutePath);

        if(fStat.getFileType() == FsNode::DIRECTORY_FILE) {
            Directory &currentDir = *Directory::open(absolutePath);

            recursiveDelete(progName, currentDir);

            delete &currentDir;
        } else {
            if (fileSystem->deleteFile(absolutePath) != FileSystem::SUCCESS) {
                stderr << progName << ": '" << childName << "': Unable to delete file!" << endl;
            }
        }

        delete &fStat;
    }

    if (fileSystem->deleteFile(dir.getAbsolutePath()) != FileSystem::SUCCESS) {
        stderr << progName << ": '" << dir.getName() << "': Unable to delete directory!" << endl;
    }
}

void Rm::execute(Util::Array<String> &args, OutputStream &outputStream) {
    bool recursive = false;
    Util::ArrayList<String> paths;

    for(uint32_t i = 1; i < args.length(); i++) {
        if(!args[i].beginsWith("-") || args[i] == "-") {
            paths.add(args[i]);
        }  else if(args[i] == "-h" || args[i] == "--help") {
            outputStream << "Deletes files." << endl << endl;
            outputStream << "Usage: " << args[0] << " [OPTION]... [PATH]..." << endl << endl;
            outputStream << "Options:" << endl;
            outputStream << "  -r, --recursive: Recursive delete: Delete all files and subdirectories inside a directory." << endl;
            outputStream << "  -h, --help: Show this help-message." << endl;
            return;
        } else if(args[i] == "-r" || args[i] == "--recursive") {
            recursive = true;
        } else {
            stderr << args[0] << ": Invalid option '" << args[i] << "'!" << endl;
            return;
        }
    }

    fileSystem = Kernel::getService<FileSystem>();

    for(const String &path : paths) {
        String absolutePath = calcAbsolutePath(path);

        if (!FileStatus::exists(absolutePath)) {
            stderr << args[0] << ": '" << path << "': File or Directory not found!" << endl;
            continue;
        }

        FileStatus &fStat = *FileStatus::stat(absolutePath);

        if(fStat.getFileType() != FsNode::DIRECTORY_FILE) {
            if (fileSystem->deleteFile(absolutePath) != FileSystem::SUCCESS) {
                stderr << args[0] << ": '" << path << "': Unable to delete file!" << endl;
            }
        } else {
            if(recursive) {
                Directory &dir = *Directory::open(absolutePath);

                recursiveDelete(args[0], dir);

                delete &dir;
            } else {
                stderr << args[0] << ": '" << path << "': Is a directory!" << endl;
            }
        }

        delete &fStat;
    }
}