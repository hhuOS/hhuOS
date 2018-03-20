#include "File.h"

extern "C" {
#include "lib/libc/string.h"    
}

/**
 * Tries to open the file at a specified path.
 * 
 * @param path The file's path.
 * @param mode The desired mode:
 *             "r":  Reading only.
 *             "w":  Writing only; Creates the file, if it does not exist; Starts writing at the file's beginning.
 *             "a":  Writing only; Creates the file, if it does not exist; Always appends to the file's end.
 *             "r+": Reading and writing; Starts writing at the file's beginning.
 *             "w+": Reading and writing; Truncates the file to zero length, if it already exists;
 *                   Creates the file, if it does not exist; Starts writing at the file's beginning.
 *             "a+": Reading and writing; Creates the file, if it does not exist; Starts reading from the file's beginning;
 *                   Always appends to the file's end.
 * 
 * @return On success, the file;
 *         else nullptr.
 */
File *File::open(const String &path, const String &mode) {

    if (mode.length() == 0) {
        return nullptr;
    }

    if((mode[0] != 'w' && mode[0] != 'r' && mode[0] != 'a') || (mode[1] != '+' && mode[1] != 0)) {
        return nullptr;
    }


    FileSystem *fileSystem = Kernel::getService<FileSystem>();

    FsNode *node = fileSystem->getNode(path);

    if(node == nullptr) {
        switch(mode[0]) {
            case 'r' :
                return nullptr;
            case 'w' :
                if(fileSystem->createFile(path) == -1) return nullptr;
                return new File(fileSystem->getNode(path), path, mode);
            case 'a' :
                if(fileSystem->createFile(path) == -1)
                    return nullptr;
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

/**
 * Returns a DirEntry-struct, that contains information about this file.
 * 
 * @return The DirEntry-struct
 */
DirEntry *File::getInfo() {

    DirEntry *entry = new DirEntry();

    entry->name = node->getName();

    entry->fullPath = path;

    entry->fileType = node->getFileType();

    entry->length = node->getLength();

    return entry;
}

/**
 * Writes a char to the file at the position specified by 'pos'.
 * 
 * @param ch The char.
 * 
 * @return 0, on success.
 */
int32_t File::writeChar(char ch) {
    if(mode[0] == 'r' && mode[1] != '+') return -1;
    uint32_t pos = mode[0] == 'a' ? node->getLength() : this->pos;

    int32_t ret = node->writeData(&ch, pos, 1);
    this->pos++;

    return ret;
}

/**
 * Writes a null-terminated string to the file at the position specified by 'pos'.
 * 
 * @param s The string.
 * 
 * @return 0, on success.
 */
int32_t File::writeString(char *string) {
    if(mode[0] == 'r' && mode[1] != '+') return -1;
    uint32_t pos = mode[0] == 'a' ? node->getLength() : this->pos;

    uint32_t len = strlen(string);
    int32_t ret = node->writeData(string, pos, len);
    if(ret == 0) this->pos += len;

    return ret;
}

/**
 * Writes a specified number of bytes from a char-array to the file at the position specified by 'pos'.
 * 
 * @param data The char-array.
 * @param len The amount of bytes.
 * 
 * @return 0, on success.
 */
int32_t File::writeBytes(char *data, uint32_t len) {
    if(mode[0] == 'r' && mode[1] != '+') return -1;
    uint32_t pos = mode[0] == 'a' ? node->getLength() : this->pos;

    int32_t ret = node->writeData(data, pos, len);
    if(ret >= 0) this->pos += len;

    return ret;
}

/**
 * Reads a char from the file at the position specified by 'pos'.
 *
 * @return The char.
 */
char File::readChar() {
    if(mode[0] != 'r' && mode[1] != '+') return -1;

    char ch;
    if(node->readData(&ch, pos, 1) == nullptr) return -1;
    pos++;

    return ch;
}

/**
 * Reads a string from the file at the position specified by 'pos'.
 * If a '\n' or 'EOF' character is encountered, the function returns the read string up to that point.
 * A '0' is always appended.
 *
 * @param buf An already allocated buffer, to which the string will be written.
 * @param len The buffer's size. At most, len - 1 characters will be read.
 * 
 * @return The string.
 */
char *File::readString(char *buf, uint32_t len) {
    if(mode[0] != 'r' && mode[1] != '+') return nullptr;

    uint32_t i;
    for(i = 0; i < len - 1; i++) {
        buf[i] = readChar();

        if(buf[i] == '\n' || buf[i] == VFS_EOF) {
            buf[i] = 0;
            return buf;
        }
    }

    buf[i + 1] = 0;
    return buf;
}

/**
 * Reads a given amount of bytes from the file at the position specified by 'pos'.
 *
 * @param buf An already allocated buffer, to which the string will be written.
 * @param len The buffer's size. This function will read len bytes from the file.
 * 
 * @return The string.
 */
char *File::readBytes(char *buf, uint32_t len) {
    if(mode[0] != 'r' && mode[1] != '+') return nullptr;

    if(node->readData(buf, pos, len) == nullptr) return nullptr;
    pos += len;

    return buf;
}

/**
 * The variable 'pos' determines which character will be read next.
 * This function return the current value of pos.
 * 
 * @return The current value of 'pos'.
 */
uint32_t File::getPos() {
    return pos;
}

/**
 * The variable 'pos' determines which character will be read next.
 * This function can be used to set 'pos'.
 * 
 * @param offset Number of bytes to offset from 'origin'.
 * @param origin SEEK_SET: The beginning of the file;
 *               SEEK_CUR: The current position;
 *               SEEK_END: The end of the file.
 */
void File::setPos(uint32_t offset, uint32_t origin) {
    switch(origin) {
        case SEEK_SET :
            pos = offset;
            break;
        case SEEK_CUR :
            pos += offset;
            break;
        case SEEK_END :
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

InputStream& File::operator >> (char *string) {
    readBytes(string, node->getLength());

    return *this;
}

InputStream& File::operator >> (OutputStream &outStream) {
    uint32_t len = node->getLength();
    char *buf = new char[len + 1];

    readBytes(buf, len);
    buf[len] = 0;

    outStream << buf;
    outStream.flush();

    delete buf;
    return *this;
}