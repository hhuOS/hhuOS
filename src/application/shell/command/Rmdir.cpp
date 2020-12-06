#include "Rmdir.h"

Rmdir::Rmdir(Shell &shell) : Command(shell) {

};

/* We will use this function to actually delete the folder and remove it from the file system */
void Rmdir::deleteFile(const String &progName, FileStatus &fStat) {
    if(shell.getCurrentWorkingDirectory().getAbsolutePath().beginsWith(fStat.getAbsolutePath())) {
        stderr << progName << ": '" << fStat.getName() << "': Cannot delete the current working directory!" << endl;
        return;
    }
    auto ret = fileSystem->deleteFile(fStat.getAbsolutePath());
    /* Handling all options of the file system that are possible */
    switch(ret) {
        case Filesystem::SUCCESS :
            break;
        case Filesystem::SUBDIRECTORY_CONTAINS_MOUNT_POINT :
            stderr << progName << ": '" << fStat.getName() << "': A device is still mounted to a subdirectory!" << endl;
            break;
        case Filesystem::FILE_NOT_FOUND :
            stderr << progName << ": '" << fStat.getName() << "': File or Directory not found!" << endl;
            break;
        case Filesystem::DELETING_FILE_FAILED :
            stderr << progName << ": '" << fStat.getName() << "': Unable to delete file!" << endl;
            break;
        default:
            break;
    }
}

/* This function opens the file status pointer and then calls the deleteFile function */
void Rmdir::deleteDirectory(const String &progName, Directory &dir) {
    FileStatus *fStat = FileStatus::stat(dir.getAbsolutePath());

    /* 
    Check if there are some files or directories in the directory which user want to delete .
    If there is then print the appropriate message to notify the user else delete the direcotry.
    */
    if(dir.getChildren().length()==0){
        deleteFile(progName,*fStat);
        delete fStat;
    } else {
        stderr << progName << ": '" << dir.getName() << "': The directory is not empty" << endl;
    }
}

/* The execute command is implemented here */
void Rmdir::execute(Util::Array<String> &args) {
    
    /* We are first going to parser the arguments */
    Util::ArgumentParser parser(getHelpText(), 1);
    if(!parser.parse(args)) {
        stderr << args[0] << ": " << parser.getErrorString() << endl;
        return;
    }
    fileSystem = Kernel::System::getService<Filesystem>();
    for(const String &path : parser.getUnnamedArguments()) {
        String absolutePath = calcAbsolutePath(path);

        /* If file status does not exist, we show the appropriate message*/
        if (!FileStatus::exists(absolutePath)) {
            stderr << args[0] << ": '" << path << "': File or Directory not found!" << endl;
            continue;
        }

        /* Finally we delete the directory below */
        FileStatus *fStat = FileStatus::stat(absolutePath);
        if(fStat->getFileType() != FsNode::DIRECTORY_FILE) {
            stderr << args[0] << ": '" << path << "': Invalid command" << endl;
            continue;
        } else {
            Directory *dir  = Directory::open(absolutePath);
            deleteDirectory(args[0], *dir);
            delete dir;
        }
        delete fStat;
    }
}

/* Function for the help flags (-h or --help) */
const String Rmdir::getHelpText() {
    return "Deletes empty directory.\n\n"
           "Usage: rmdir [PATH]...\n"
           "Options:\n"
           "  -h, --help: Show this help-message.";
}