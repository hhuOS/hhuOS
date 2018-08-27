/*
 * Copyright (C) 2018 Burak Akguel, Christian Gesse, Fabian Ruhland, Filip Krakowski, Michael Schoettner
 * Heinrich-Heine University
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "Rm.h"

Rm::Rm(Shell &shell) : Command(shell) {

};

void Rm::deleteFile(const String &progName, FileStatus &fStat) {
    if(shell.getCurrentWorkingDirectory().getAbsolutePath().beginsWith(fStat.getAbsolutePath())) {
        stderr << progName << ": '" << fStat.getName() << "': Cannot delete the current working directory!" << endl;
        return;
    }

    auto ret = fileSystem->deleteFile(fStat.getAbsolutePath());

    switch(ret) {
        case FileSystem::SUCCESS :
            break;
        case FileSystem::SUBDIRECTORY_CONTAINS_MOUNT_POINT :
            stderr << progName << ": '" << fStat.getName() << "': A device is still mounted to a subdirectory!" << endl;
            break;
        case FileSystem::FILE_NOT_FOUND :
            stderr << progName << ": '" << fStat.getName() << "': File or Directory not found!" << endl;
            break;
        case FileSystem::DELETING_FILE_FAILED :
            stderr << progName << ": '" << fStat.getName() << "': Unable to delete file!" << endl;
            break;
        default:
            break;
    }
}

void Rm::recursiveDelete(const String &progName, Directory &dir) {
    for(const String &childName : dir.getChildren()) {
        String absolutePath = dir.getAbsolutePath() + "/" + childName;

        FileStatus &fStat = *FileStatus::stat(absolutePath);

        if(fStat.getFileType() == FsNode::DIRECTORY_FILE) {
            Directory &currentDir = *Directory::open(absolutePath);

            recursiveDelete(progName, currentDir);

            delete &currentDir;
        } else {
            deleteFile(progName, fStat);
        }

        delete &fStat;
    }

    FileStatus &fStat = *FileStatus::stat(dir.getAbsolutePath());

    deleteFile(progName, fStat);

    delete &fStat;
}

void Rm::execute(Util::Array<String> &args) {
    ArgumentParser parser(getHelpText(), 1);
    parser.addSwitch("recursive", "r");

    if(!parser.parse(args)) {
        stderr << args[0] << ": " << parser.getErrorString() << endl;
        return;
    }

    bool recursive = parser.checkSwitch("recursive");

    fileSystem = Kernel::getService<FileSystem>();

    for(const String &path : parser.getUnnamedArguments()) {
        String absolutePath = calcAbsolutePath(path);

        if (!FileStatus::exists(absolutePath)) {
            stderr << args[0] << ": '" << path << "': File or Directory not found!" << endl;
            continue;
        }

        FileStatus &fStat = *FileStatus::stat(absolutePath);

        if(fStat.getFileType() != FsNode::DIRECTORY_FILE) {
            deleteFile(args[0], fStat);
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

const String Rm::getHelpText() {
    return "Deletes files.\n\n"
           "Usage: rm [OPTION]... [PATH]...\n\n"
           "Options:\n"
           "  -r, --recursive: Recursive delete: Delete all files and subdirectories inside a directory.\n"
           "  -h, --help: Show this help-message.";
}
