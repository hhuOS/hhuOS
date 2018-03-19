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

    uint32_t pos = 0;

    File(FsNode *arg_node, const String &arg_path, const String &arg_mode) : node(arg_node) {

        path = FileSystem::parsePath(arg_path);

        mode = arg_mode;

        if(mode.beginsWith("a") && node->getFileType() == REGULAR_FILE) {
            pos = node->getLength();
        }
    };

public:

    ~File() {
        delete node;
    }

    static File *open(const String &path, const String &mode);

    DirEntry *getInfo();

    int32_t writeChar(char ch);

    int32_t writeString(char *string);

    int32_t writeBytes(char *data, uint32_t len);

    char readChar();

    char *readString(char *buf, uint32_t len);

    char *readBytes(char *buf, uint32_t len);

    uint32_t getPos();

    void setPos(uint32_t offset, uint32_t origin);


    void flush();


    InputStream& operator >> (char &c);

    InputStream& operator >> (char *string);

    InputStream& operator >> (OutputStream &outStream);
};

#endif