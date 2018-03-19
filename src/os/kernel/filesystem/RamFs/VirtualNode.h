#ifndef __VirtualNode_include__
#define __VirtualNode_include__

#include "../FsNode.h"
#include "lib/LinkedList.h"
#include <stdint.h>

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

        if(fileType == DIRECTORY_FILE) {
            children = new LinkedList<VirtualNode>();
        }
    }

    virtual ~VirtualNode() {

        if(fileType == REGULAR_FILE) {
            delete data;
        }

        if(fileType == DIRECTORY_FILE) {

            for(uint32_t i = 0; i < children->length(); i++) {

                VirtualNode *currentNode = children->get(i);

                children->remove(currentNode);

                delete currentNode;
            }

            delete children;
        }
    }

    String getName();

    uint8_t getFileType();

    String getChild(uint32_t pos);
    
    virtual uint64_t getLength();

    virtual char *readData(char *buf, uint64_t pos, uint32_t numBytes);

    virtual int32_t writeData(char *buf, uint64_t pos, uint32_t numBytes);

    LinkedList<VirtualNode> *getChildren();

private:

    String name;

    uint8_t fileType;

    uint32_t length;

    LinkedList<VirtualNode> *children;

    char *data;
};

#endif
