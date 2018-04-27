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
#include "Cd.h"

Cd::Cd(Shell &shell) : Command(shell) {

};

void Cd::execute(Util::Array<String> &args) {
    String path;

    for(uint32_t i = 1; i < args.length(); i++) {
        if(!args[i].beginsWith("-") || args[i] == "-") {
            if(path.isEmpty()) {
                path = args[i];
            } else {
                stderr << args[0] << ": Too many arguments!" << endl;
                return;
            }
        } else if(args[i] == "-h" || args[i] == "--help") {
            stdout << "Changes the working directory." << endl << endl;
            stdout << "Usage: " << args[0] << " [OPTION]... [PATH]" << endl << endl;
            stdout << "Options:" << endl;
            stdout << "  -h, --help: Show this help-message." << endl;
            return;
        } else {
            stderr << args[0] << ": Invalid option '" << args[i] << "'!" << endl;
            return;
        }
    }

    String absolutePath = calcAbsolutePath(path);

    if(!FileStatus::exists(absolutePath)) {
        stderr << args[0] << ": '" << path << "': Directory not found!" << endl;
        return;
    }

    FileStatus &fStat = *FileStatus::stat(absolutePath);

    if(fStat.getFileType() != FsNode::DIRECTORY_FILE) {
        stderr << args[0] << ": '" << path << "': Not a directory!" << endl;
    } else {
        shell.setCurrentWorkingDirectory(Directory::open(absolutePath));
    }

    delete &fStat;
}