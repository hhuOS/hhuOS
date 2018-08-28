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

#include <lib/file/FileStatus.h>
#include <lib/file/File.h>
#include <kernel/services/ModuleLoader.h>
#include "Insmod.h"

Insmod::Insmod(Shell &shell) : Command(shell) {

};

void Insmod::execute(Util::Array<String> &args) {
    ArgumentParser parser(getHelpText(), 1);

    if(!parser.parse(args)) {
        stderr << args[0] << ": " << parser.getErrorString() << endl;
        return;
    }

    auto *moduleLoader = Kernel::getService<ModuleLoader>();

    for(const String &path : parser.getUnnamedArguments()) {
        String absolutePath = calcAbsolutePath(path);

        if(!FileStatus::exists(absolutePath)) {
            stderr << args[0] << ": '" << path << "': File or Directory not found!" << endl;
            continue;
        }

        FileStatus *fStat = FileStatus::stat(absolutePath);

        if(fStat->getFileType() == FsNode::DIRECTORY_FILE) {
            stderr << args[0] << ": '" << path << "': Is a directory!" << endl;

            delete fStat;
            continue;
        }

        File *module = File::open(absolutePath, "r");

        ModuleLoader::Status status = moduleLoader->load(module);

        if (status == ModuleLoader::Status::OK) {
            stdout << args[0] << ": '" << path <<  "': Inserted module!" << endl;
        } else {
            stderr << args[0] << ": '" << path <<  "': Unable to insert module!" << endl;
        }

        delete module;
        delete fStat;
    }
}

const String Insmod::getHelpText() {
    return "Loads kernel modules from disk.\n\n"
           "Usage: insmod [OPTION]... [FILE]...\n\n"
           "Options:\n"
           "  -h, --help: Show this help-message.";
}
