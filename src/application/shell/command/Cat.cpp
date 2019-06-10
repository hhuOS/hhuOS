/*
 * Copyright (C) 2018 Burak Akguel, Christian Gesse, Fabian Ruhland, Filip Krakowski, Michael Schoettner
 * Heinrich-Heine University
 *
 * This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public
 * License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any
 * later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied
 * warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>
 */

#include "lib/file/File.h"
#include "lib/file/FileStatus.h"
#include "Cat.h"

Cat::Cat(Shell &shell) : Command(shell) {

};

void Cat::execute(Util::Array<String> &args) {
    Util::ArgumentParser parser(getHelpText(), 1);

    if(!parser.parse(args)) {
        stderr << args[0] << ": " << parser.getErrorString() << endl;
        return;
    }

    for(const String &path : parser.getUnnamedArguments()) {
        String absolutePath = calcAbsolutePath(path);

        if(FileStatus::exists(absolutePath)) {
            FileStatus *fStat = FileStatus::stat(absolutePath);

            if(fStat->getFileType() == FsNode::DIRECTORY_FILE) {
                stderr << args[0] << ": '" << path << "': Is a directory!" << endl;

                delete fStat;
                continue;
            }

            delete fStat;
        } else {
            stderr << args[0] << ": '" << path << "': File or Directory not found!" << endl;
            continue;
        }

        File *file = File::open(absolutePath, "r");

        char *buf;
        *file >> buf;

        stdout.writeBytes(buf, file->getLength());
        stdout.flush();

        delete buf;
        delete file;
    }
}

const String Cat::getHelpText() {
    return "Concatenates multiple files and writes them to the standard output stream.\n\n"
            "Usage: cat [OPTION]... [FILE]...\n\n"
            "Options:\n"
            "  -h, --help: Show this help-message.";
}
