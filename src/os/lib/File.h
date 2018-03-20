/*****************************************************************************
 *                                                                           *
 *                                  F I L E                                  *
 *                                                                           *
 *---------------------------------------------------------------------------*
 * Beschreibung:    Stellt Funktionen zur Interaktion mit Dateien aus dem    *
 *                  VFS zur Verfügung.                                       *
 *                                                                           *
 * Autor:           Fabian Ruhland, HHU, 13.11.2017                          *
 *****************************************************************************/

#ifndef __File_include__
#define __File_include__


#include "kernel/services/FileSystem.h"
#include "kernel/Kernel.h"
#include "Directory.h"
#include "InputStream.h"
#include <stdint.h>

#define SEEK_SET    0
#define SEEK_CUR    1
#define SEEK_END    2

class File : public OutputStream, public InputStream {

private:

    FsNode *node;

    String path;

    String mode;

    uint64_t pos = 0;

    File(FsNode *node, const String &arg_path, const String &arg_mode);

public:

    ~File() {
        delete node;
    }

    static File *open(const String &path, const String &mode);

    String getName();

    String getAbsolutePath();

    uint32_t getFileType();

    uint64_t getLength();

    int32_t writeChar(char ch);

    int32_t writeString(char *string);

    int32_t writeBytes(char *data, uint64_t len);

    char readChar();

    char *readString(char *buf, uint64_t len);

    char *readBytes(char *buf, uint64_t len);

    uint64_t getPos();

    void setPos(uint64_t offset, uint32_t origin);


    void flush();


    InputStream& operator >> (char &c);

    InputStream& operator >> (char *&string);

    InputStream& operator >> (OutputStream &outStream);
};

#endif