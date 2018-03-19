/*****************************************************************************
 *                                                                           *
 *                            D I R E C T O R Y                              *
 *                                                                           *
 *---------------------------------------------------------------------------*
 * Beschreibung:    Stellt Funktionen zur Interaktion mit Verzeichnissen aus *
 *                  dem VFS zur Verfügung.                                   *
 *                                                                           *
 * Autor:           Fabian Ruhland, HHU, 13.11.2017                          *
 *****************************************************************************/

#ifndef __Directory_include__
#define __Directory_include__


#include "kernel/services/FileSystem.h"
#include "kernel/Kernel.h"
#include <stdint.h>

extern "C" {
#include "lib/libc/string.h"
}

/**
 * Contains information about a specific file.
 */
struct DirEntry {
    String name;
    String fullPath;
    uint32_t fileType;
    uint64_t length;
};

class Directory {



public:
    ~Directory() {
        delete node;
    }

    static Directory *open(const String &path);

    DirEntry *getInfo();

    DirEntry *nextEntry();

    void setPos(uint32_t pos);

    uint32_t getPos();

private:

    FsNode *node;

    String path;

    uint32_t pos = 0;

    Directory(FsNode *arg_node, const String &arg_path) : node(arg_node) {
        path = FileSystem::parsePath(arg_path);
    };
};

#endif