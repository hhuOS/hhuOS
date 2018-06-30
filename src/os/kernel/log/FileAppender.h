#ifndef __FileAppender_include__
#define __FileAppender_include__


#include <lib/file/File.h>
#include "Appender.h"

class FileAppender : public Appender {

public:

    FileAppender(File *file);

    FileAppender(const FileAppender &other) = delete;

    FileAppender &operator=(const FileAppender &other) = delete;

    ~FileAppender() = default;

    void append(const String &message) override;

private:

    File *file = nullptr;
};


#endif
