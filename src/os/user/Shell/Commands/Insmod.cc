#include <lib/file/FileStatus.h>
#include <lib/file/File.h>
#include <kernel/services/ModuleLoader.h>
#include "Insmod.h"

Insmod::Insmod(Shell &shell) : Command(shell) {

};

void Insmod::execute(Util::Array<String> &args, OutputStream &outputStream) {
    Util::ArrayList<String> paths;

    for(uint32_t i = 1; i < args.length(); i++) {
        if(!args[i].beginsWith("-") || args[i] == "-") {
            paths.add(args[i]);
        } else if(args[i] == "-h" || args[i] == "--help") {
            outputStream << "Loads kernel modules from disk." << endl << endl;
            outputStream << "Usage: " << args[0] << " [OPTION]... [FILE]..." << endl << endl;
            outputStream << "Options:" << endl;
            outputStream << "  -h, --help: Show this help-message." << endl;
            return;
        } else {
            stderr << args[0] << ": Invalid option '" << args[i] << "'!" << endl;
            return;
        }
    }

    auto *moduleLoader = Kernel::getService<ModuleLoader>();

    for(const String &path : paths) {
        String absolutePath = calcAbsolutePath(path);

        if(!FileStatus::exists(absolutePath)) {
            stderr << args[0] << ": '" << path << "': File or Directory not found!" << endl;
            continue;
        }

        FileStatus &fStat = *FileStatus::stat(absolutePath);

        if(fStat.getFileType() == FsNode::DIRECTORY_FILE) {
            stderr << args[0] << ": '" << path << "': Is a directory!" << endl;

            delete &fStat;
            continue;
        }

        File *module = File::open(path, "r");

        ModuleLoader::Status status = moduleLoader->load(module);

        if (status == ModuleLoader::Status::OK) {
            outputStream << args[0] << ": '" << path <<  "': Inserted module!" << endl;
        } else {
            stderr << args[0] << ": '" << path <<  "': Unable to insert module!" << endl;
        }

        delete module;
        delete &fStat;
    }
}