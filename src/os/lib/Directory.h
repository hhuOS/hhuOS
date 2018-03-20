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
#include "lib/util/Array.h"
#include <cstdint>

extern "C" {
#include "lib/libc/string.h"
}

class Directory {

private:

    FsNode *node;

    String path;

    Directory(FsNode *arg_node, const String &arg_path) : node(arg_node) {
        path = FileSystem::parsePath(arg_path);
    };

public:
    ~Directory() {
        delete node;
    }

    static Directory *open(const String &path);

    String getName();

    String getAbsolutePath();

    Util::Array<String> getChildren();
};

#endif