#include "File.h"

File::File(FsNode *node, const String &path, const String &mode) : node(node) {
    this->path = FileSystem::parsePath(path);
    this->mode = mode;

    if(mode.beginsWith("a") && node->getFileType() == REGULAR_FILE) {
        pos = node->getLength();
    }
};

File::~File() {
    delete node;
}

File *File::open(const String &path, const String &mode) {

    if (mode.length() == 0) {
        return nullptr;
    }

    if((mode[0] != 'w' && mode[0] != 'r' && mode[0] != 'a') || (mode[1] != '+' && mode[1] != 0)) {
        return nullptr;
    }


    auto *fileSystem = Kernel::getService<FileSystem>();

    FsNode *node = fileSystem->getNode(path);

    if(node == nullptr) {
        switch(mode[0]) {
            case 'r' :
                return nullptr;
            case 'w' :
                if(fileSystem->createFile(path) == -1) {
                    return nullptr;
                }

                return new File(fileSystem->getNode(path), path, mode);
            case 'a' :
                if(fileSystem->createFile(path) == -1) {
                    return nullptr;
                }

                return new File(fileSystem->getNode(path), path, mode);
            default:
                return nullptr;
        }
    } else if(node->getFileType() == DIRECTORY_FILE) {
        return nullptr;
    }

    if(mode[0] == 'w' && mode[1] == '+') {
        if(fileSystem->createFile(path) == -1) {
            delete node;
            return nullptr;
        }
        
        return new File(fileSystem->getNode(path), path, mode);
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

uint32_t File::writeChar(char ch) {
    if(mode[0] == 'r' && mode[1] != '+') {
        return READ_ONLY_MODE;
    }

    uint64_t pos = mode[0] == 'a' ? node->getLength() : this->pos;

    if(node->writeData(&ch, pos, 1) == 0) {
        this->pos++;

        return SUCCESS;
    }

    return WRITE_ERROR;
}

uint32_t File::writeString(char *string) {
    if(mode[0] == 'r' && mode[1] != '+') {
        return READ_ONLY_MODE;
    }

    uint64_t pos = mode[0] == 'a' ? node->getLength() : this->pos;

    uint32_t len = strlen(string);

    if(node->writeData(string, pos, len) == 0) {
        this->pos += len;

        return SUCCESS;
    }

    return WRITE_ERROR;
}

uint32_t File::writeBytes(char *data, uint64_t len) {
    if(mode[0] == 'r' && mode[1] != '+') {
        return READ_ONLY_MODE;
    }

    uint64_t pos = mode[0] == 'a' ? node->getLength() : this->pos;

    if(node->writeData(data, pos, len) == 0) {
        this->pos += len;

        return SUCCESS;
    }

    return WRITE_ERROR;
}

char File::readChar() {
    if(mode[0] != 'r' && mode[1] != '+') {
        return 0;
    }

    char c;

    if(node->readData(&c, pos, 1)) {
        pos++;

        return c;
    }

    return 0;
}

uint32_t File::readString(char *buf, uint64_t len) {
    if(mode[0] != 'r' && mode[1] != '+') {
        return WRITE_ONLY_MODE;
    }

    uint32_t i;
    for(i = 0; i < len - 1; i++) {
        buf[i] = readChar();

        if(buf[i] == 0 || buf[i] == '\n' || buf[i] == VFS_EOF) {
            buf[i] = 0;
            return SUCCESS;
        }
    }

    buf[i + 1] = 0;
    return SUCCESS;
}

uint32_t File::readBytes(char *buf, uint64_t len) {
    if(mode[0] != 'r' && mode[1] != '+') {
        return WRITE_ONLY_MODE;
    }

    if(node->readData(buf, pos, len)) {
        pos += len;

        return SUCCESS;
    }

    return READ_ERROR;
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