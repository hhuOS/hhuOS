#include "FileAppender.h"

FileAppender::FileAppender(File *file) : file(file) {

}

void FileAppender::append(const String &message) {

    *file << message << endl;
}
