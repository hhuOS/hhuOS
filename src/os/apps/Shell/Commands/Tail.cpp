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

#include <lib/file/File.h>
#include <lib/file/FileStatus.h>
#include "Tail.h"

Tail::Tail(Shell &shell) : Command(shell) {

};

void Tail::execute(Util::Array<String> &args) {
    bool byteMode = false;
    uint64_t count = 10;

    ArgumentParser parser(getHelpText(), 1);
    parser.addParameter("bytes", "c");
    parser.addParameter("lines", "n");

    if(!parser.parse(args)) {
        stderr << args[0] << ": " << parser.getErrorString() << endl;
        return;
    }

    if(!parser.getNamedArgument("bytes").isEmpty()) {
        byteMode = true;
        count = static_cast<uint64_t>(strtoint((const char*) parser.getNamedArgument("bytes")));
    } else if(!parser.getNamedArgument("lines").isEmpty()) {
        count = static_cast<uint64_t>(strtoint((const char*) parser.getNamedArgument("lines")));
    }

    if(count == 0) {
        return;
    }

    bool firstFile = true;

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

        if(parser.getUnnamedArguments().length() > 1) {
            if(!firstFile) {
                stdout << endl;
            }

            firstFile = false;

            stdout << "==> " << path << " <==" << endl;
        }

        if(byteMode) {
            auto *buf = new char[count];
            memset(buf, 0, count);

            file->setPos(count > file->getLength() ? 0 : file->getLength() - count);
            uint64_t outCount = file->readBytes(buf, count);
            stdout.writeBytes(buf, outCount);

            delete buf;
        } else {
            uint32_t lineCount = 0;
            uint64_t pos = file->getLength();

            file->setPos(pos);

            while(pos > 0) {
                char c = file->readChar();

                if(c == '\n') {
                    lineCount++;

                    if(lineCount == count) {
                        pos++;
                        file->setPos(pos);
                        break;
                    }
                }

                pos--;
                file->setPos(pos);
            }

            char *buf;
            *file >> buf;

            stdout.writeBytes(buf, file->getLength() - pos);

            delete buf;
        }

        stdout.flush();
        delete file;
    }
}

const String Tail::getHelpText() {
    return "Writes the last 10 lines of multiple files to the standard output stream.\n\n"
           "Usage: tail [OPTION]... [FILE]...\n\n"
           "Options:\n"
           "  -c, --bytes [NUMBER]: Print the last NUMBER bytes.\n"
           "  -n, --lines [NUMBER]: Print the last NUMBER lines.\n"
           "  -h, --help: Show this help-message.";
}
