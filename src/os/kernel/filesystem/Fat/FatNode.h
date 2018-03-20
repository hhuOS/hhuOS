#ifndef __FatNode_include__
#define __FatNode_include__

#include "../FsNode.h"
#include "FatFsLib/FatFs.h"
#include <stdint.h>

extern "C" {
#include "lib/libc/string.h"
}

/**
 * An implemenation of FsNode for the Fat-library,
 */
class FatNode : public FsNode {
private:
    union FatObject {
        FIL file;
        DIR dir;
    };

    FatFs *fatInstance;
    FatObject fatObject;
    FILINFO info;
    String path;

    FatNode(FatFs *arg_fatInstance) : fatInstance(arg_fatInstance) {}

public:
    ~FatNode() {
        if(info.fattrib & AM_DIR)
            fatInstance->f_closedir(&fatObject.dir);
        
        if(!(info.fattrib & AM_DIR))
            fatInstance->f_close(&fatObject.file);
    }

    static FatNode *open(const String &path, FatFs *fatInstance);
    String getName();
    uint8_t getFileType();
    uint64_t getLength();
    Util::Array<String> getChildren();
    bool readData(char *buf, uint64_t pos, uint64_t numBytes);
    bool writeData(char *buf, uint64_t pos, uint64_t numBytes);
};

#endif