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

#include <lib/file/File.h>
#include <lib/file/FileStatus.h>
#include "Tail.h"

Tail::Tail(Shell &shell) : Command(shell) {

};

void Tail::execute(Util::Array<String> &args) {
    bool byteMode = false;
    uint64_t count = 10;
    Util::ArrayList<String> paths;

    for(uint32_t i = 1; i < args.length(); i++) {
        if(!args[i].beginsWith("-") || args[i] == "-") {
            paths.add(args[i]);
        } else if(args[i] == "-h" || args[i] == "--help") {
            stdout << "Writes the last 10 lines of multiple files to the standard output stream." << endl << endl;
            stdout << "Usage: " << args[0] << " [OPTION]... [FILE]..." << endl << endl;
            stdout << "Options:" << endl;
            stdout << "  -c, --bytes [NUMBER]: Print the last NUMBER bytes." << endl;
            stdout << "  -n, --lines [NUMBER]: Print the last NUMBER lines." << endl;
            stdout << "  -h, --help: Show this help-message." << endl;
            return;
        } else if(args[i] == "-c" || args[i] == "--bytes") {
            if(i == args.length() - 1) {
                stderr << args[0] << ": '" << args[i] << "': This option needs an argument!" << endl;
                return;
            } else {
                byteMode = true;
                int32_t tmpCount = strtoint((const char *) args[++i]);

                if(tmpCount < 0) {
                    stderr << args[0] << ": '" << args[i] << "': Number must be positive!" << endl;
                    return;
                }

                count = static_cast<uint64_t>(tmpCount);
            }
        } else if(args[i] == "-n" || args[i] == "--lines") {
            if(i == args.length() - 1) {
                stderr << args[0] << ": '" << args[i] << "': This option needs an argument!" << endl;
                return;
            } else {
                byteMode = false;
                int32_t tmpCount = strtoint((const char *) args[++i]);

                if(tmpCount < 0) {
                    stderr << args[0] << ": '" << args[i] << "': Number must be positive!" << endl;
                    return;
                }

                count = static_cast<uint64_t>(tmpCount);
            }
        } else {
            stderr << args[0] << ": Invalid option '" << args[i] << "'!" << endl;
            return;
        }
    }

    if(count == 0) {
        return;
    }

    bool firstFile = true;

    for(const String &path : paths) {
        String absolutePath = calcAbsolutePath(path);

        if(FileStatus::exists(absolutePath)) {
            FileStatus &fStat = *FileStatus::stat(absolutePath);

            if(fStat.getFileType() == FsNode::DIRECTORY_FILE) {
                stderr << args[0] << ": '" << path << "': Is a directory!" << endl;

                delete &fStat;
                continue;
            }

            delete &fStat;
        } else {
            stderr << args[0] << ": '" << path << "': File or Directory not found!" << endl;
            continue;
        }

        File &file = *File::open(absolutePath, "r");

        if(paths.size() > 1) {
            if(!firstFile) {
                stdout << endl;
            }

            firstFile = false;

            stdout << "==> " << path << " <==" << endl;
        }

        if(byteMode) {
            auto *buf = new char[count];
            memset(buf, 0, count);

            file.setPos(count > file.getLength() ? 0 : file.getLength() - count);
            uint64_t outCount = file.readBytes(buf, count);
            stdout.writeBytes(buf, outCount);

            delete[] buf;
        } else {
            uint32_t lineCount = 0;
            uint64_t pos = file.getLength();

            file.setPos(pos);

            while(pos > 0) {
                char c = file.readChar();

                if(c == '\n') {
                    lineCount++;

                    if(lineCount == count) {
                        pos++;
                        file.setPos(pos);
                        break;
                    }
                }

                pos--;
                file.setPos(pos);
            }

            char *buf;
            file >> buf;

            stdout.writeBytes(buf, file.getLength() - pos);

            delete[] buf;
        }

        stdout.flush();
        delete &file;
    }
}