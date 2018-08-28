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
#include "Ls.h"

Ls::Ls(Shell &shell) : Command(shell) {

};

void Ls::execute(Util::Array<String> &args) {
    Util::ArrayList<String> paths;

    ArgumentParser parser(getHelpText(), 1);

    if(!parser.parse(args)) {
        stderr << args[0] << ": " << parser.getErrorString() << endl;
        return;
    }

    for(const String &path : parser.getUnnamedArguments()) {
        paths.add(path);
    }

    if(paths.size() == 0) {
        Directory &dir = shell.getCurrentWorkingDirectory();

        for(const String &name : dir.getChildren()) {
            stdout << "  " << name;

            FileStatus *childStat = FileStatus::stat(dir.getAbsolutePath() + "/" + name);
            if (childStat->getFileType() == FsNode::DIRECTORY_FILE) {
                stdout << "/";
            }

            delete childStat;
            stdout << endl;
        }

        return;
    }

    for(const String &path : paths) {
        String absolutePath = calcAbsolutePath(path);

        if (FileStatus::exists(absolutePath)) {
            FileStatus *fStat = FileStatus::stat(absolutePath);

            if (fStat->getFileType() != FsNode::DIRECTORY_FILE) {
                stdout << fStat->getName() << endl;

                if(paths.size() > 1) {
                    stdout << endl;
                }
            } else {
                if(paths.size() > 1) {
                    stdout << path << ":" << endl;
                }

                Directory *dir = Directory::open(absolutePath);

                for (const String &name : dir->getChildren()) {
                    stdout << "  " << name;

                    FileStatus *childStat = FileStatus::stat(dir->getAbsolutePath() + "/" + name);

                    if (childStat->getFileType() == FsNode::DIRECTORY_FILE) {
                        stdout << "/";
                    }

                    delete childStat;
                    stdout << endl;
                }

                if(paths.size() > 1) {
                    stdout << endl;
                }

                delete dir;
            }

            delete fStat;
        } else {
            stderr << args[0] << " '" << path << "': File or Directory not found!" << endl;
        }
    }
}

const String Ls::getHelpText() {
    return "Lists the content of directories. The default directory is the current working directory.\n\n"
           "Usage: ls [OPTION]... [PATH]...\n\n"
           "Options:\n"
           "  -h, --help: Show this help-message.";
}
