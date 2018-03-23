/*
 * Copyright (C) 2018 Fabian Ruhland
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