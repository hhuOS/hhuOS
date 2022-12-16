/*
 * Copyright (C) 2018-2022 Heinrich-Heine-Universitaet Duesseldorf,
 * Institute of Computer Science, Department Operating Systems
 * Burak Akguel, Christian Gesse, Fabian Ruhland, Filip Krakowski, Michael Schoettner
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

#include "lib/interface.h"
#include "FileWriter.h"
#include "lib/util/Exception.h"
#include "lib/util/file/File.h"

namespace Util::Stream {

FileWriter::FileWriter(const File::File &file) : fileDescriptor(File::open(file.getCanonicalPath())) {
    if (fileDescriptor < 0) {
        Util::Exception::throwException(Exception::ILLEGAL_STATE, "FileReader: Unable to open file!");
    }
}

FileWriter::FileWriter(const Memory::String &path) : fileDescriptor(File::open(path)) {
    if (fileDescriptor < 0) {
        Util::Exception::throwException(Exception::ILLEGAL_STATE, "FileReader: Unable to open file!");
    }
}

FileWriter::~FileWriter() {
    File::close(fileDescriptor);
}

void FileWriter::write(char c) {
    write(&c, 0, 1);
}

void FileWriter::write(const char *sourceBuffer, uint32_t length) {
    write(sourceBuffer, 0, length);
}

void FileWriter::write(const char *sourceBuffer, uint32_t offset, uint32_t length) {
    pos += writeFile(fileDescriptor, reinterpret_cast<const uint8_t*>(sourceBuffer + offset), pos, length);
}

void FileWriter::write(const Memory::String &string) {
    write(string, 0, string.length());
}

void FileWriter::write(const Memory::String &string, uint32_t offset, uint32_t length) {
    write(static_cast<char*>(string), offset, length);
}

}
