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

#include "File.h"

File::File(Util::SmartPointer<FsNode> node, const String &path, const String &mode) : node(node) {
    this->path = Filesystem::parsePath(path);
    this->mode = mode;

    if(mode.beginsWith("a") && node->getFileType() == FsNode::REGULAR_FILE) {
        pos = node->getLength();
    }
};

File *File::open(const String &path, const String &mode) {
    if (mode.length() == 0) {
        return nullptr;
    }

    if((mode[0] != 'w' && mode[0] != 'r' && mode[0] != 'a') || (mode[1] != '+' && mode[1] != 0)) {
        return nullptr;
    }

    auto *fileSystem = Kernel::getService<Filesystem>();

    Util::SmartPointer<FsNode> node = fileSystem->getNode(path);

    if(node == nullptr) {
        switch(mode[0]) {
            case 'r' :
                return nullptr;
            case 'w' :
                if(fileSystem->createFile(path) == Filesystem::SUCCESS) {
                    return new File(fileSystem->getNode(path), path, mode);
                }

                return nullptr;
            case 'a' :
                if(fileSystem->createFile(path) == Filesystem::SUCCESS) {
                    return new File(fileSystem->getNode(path), path, mode);
                }

                return nullptr;
            default:
                return nullptr;
        }
    } else if(node->getFileType() == FsNode::DIRECTORY_FILE) {
        return nullptr;
    }

    if(mode[0] == 'w' && mode[1] == '+') {
        if(fileSystem->createFile(path)) {
            return new File(fileSystem->getNode(path), path, mode);
        }

        return nullptr;
    }

    return new File(node, path, mode);
}

String File::getName() {
    return node->getName();
}

String File::getAbsolutePath() {
    return path;
}

uint32_t File::getFileType() {
    return node->getFileType();
}

uint64_t File::getLength() {
    return node->getLength();
}

uint64_t File::writeChar(char c) {
    if(mode[0] == 'r' && mode[1] != '+') {
        return 0;
    }

    uint64_t pos = mode[0] == 'a' ? node->getLength() : this->pos;

    if(node->writeData(&c, pos, 1) == 1) {
        this->pos++;

        return 1;
    }

    return 0;
}

uint64_t File::writeString(char *string) {
    if(mode[0] == 'r' && mode[1] != '+') {
        return 0;
    }

    uint64_t pos = mode[0] == 'a' ? node->getLength() : this->pos;

    uint32_t len = strlen(string);

    uint64_t ret = node->writeData(string, pos, len);
    this->pos += ret;

    return ret;
}

uint64_t File::writeBytes(char *data, uint64_t len) {
    if(mode[0] == 'r' && mode[1] != '+') {
        return 0;
    }

    uint64_t pos = mode[0] == 'a' ? node->getLength() : this->pos;

    uint64_t ret = node->writeData(data, pos, len);
    this->pos += ret;

    return ret;
}

char File::readChar() {
    if(mode[0] != 'r' && mode[1] != '+') {
        return 0;
    }

    char c;

    if(node->readData(&c, pos, 1) == 1) {
        pos++;

        return c;
    }

    return 0;
}

uint64_t File::readString(char *buf, uint64_t len) {
    if(mode[0] != 'r' && mode[1] != '+') {
        return 0;
    }

    uint32_t i;
    for(i = 0; i < len - 1; i++) {
        buf[i] = readChar();

        if(buf[i] == 0 || buf[i] == '\n' || buf[i] == FsNode::END_OF_FILE) {
            buf[i] = 0;

            return i;
        }
    }

    buf[i + 1] = 0;
    return i;
}

uint64_t File::readBytes(char *buf, uint64_t len) {
    if(mode[0] != 'r' && mode[1] != '+') {
        return 0;
    }

    uint64_t ret = node->readData(buf, pos, len);
    pos += ret;

    return ret;
}

uint64_t File::getPos() {
    return pos;
}

void File::setPos(uint64_t offset, uint32_t origin) {
    switch(origin) {
        case START :
            pos = offset;
            break;
        case CURRENT :
            pos += offset;
            break;
        case END :
            pos = node->getLength() + offset;
            break;
        default :
            break;
    }
}

void File::flush() {
    writeBytes(StringBuffer::buffer, StringBuffer::pos);
    StringBuffer::pos = 0;
}

InputStream& File::operator >> (char &c) {
    c = readChar();
    
    return *this;
}

InputStream& File::operator >> (char *&string) {
    uint64_t len = node->getLength();
    string = new char[len + 1];

    memset(string, 0, len + 1);

    readBytes(string, len);

    return *this;
}

InputStream& File::operator >> (OutputStream &outStream) {
    uint64_t len = node->getLength();
    auto *buf = new char[len + 1];

    memset(buf, 0, len + 1);

    readBytes(buf, len);

    outStream << buf;
    outStream.flush();

    delete buf;
    return *this;
}