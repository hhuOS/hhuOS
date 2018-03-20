#ifndef __VirtualNode_include__
#define __VirtualNode_include__

#include "../FsNode.h"
#include "lib/LinkedList.h"
#include <stdint.h>
#include <lib/util/ArrayList.h>
#include <lib/deprecated/ArrayList.h>

extern "C" {
#include "lib/libc/string.h"
}

/**
 * Every instance of this class is a virtual file/folder.
 * It is instantiated by the RamFsDriver.
 */
class VirtualNode {

public:

    VirtualNode(const String &arg_name, uint8_t arg_fileType) : fileType(arg_fileType) {

        if(arg_name.length() > 255) {
            name = arg_name.substring(0, 255);
        } else {
            name = arg_name;
        }

        
        if(fileType == REGULAR_FILE) {
            data = new char[1];
            data[0] = VFS_EOF;
        }
    }

    virtual ~VirtualNode() {
        for(const auto &elemement : children) {
            delete elemement;
        }

        if(fileType == REGULAR_FILE) {
            delete data;
        }
    }

    String getName();

    uint8_t getFileType();

    Util::ArrayList<VirtualNode*> &getChildren();
    
    virtual uint64_t getLength();

    virtual char *readData(char *buf, uint64_t pos, uint64_t numBytes);

    virtual int64_t writeData(char *buf, uint64_t pos, uint64_t numBytes);

private:
    String name;

    uint8_t fileType;

    uint32_t length;

    Util::ArrayList<VirtualNode*> children;

    char *data;
};

#endif
