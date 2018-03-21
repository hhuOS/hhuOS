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
 * A virtual file or directory used by RamFsDriver.
 */
class VirtualNode {

private:
    String name;

    uint8_t fileType;

    uint64_t length;

    Util::ArrayList<VirtualNode*> children;

    char *data = nullptr;

public:

    VirtualNode(const String &name, uint8_t fileType);

    VirtualNode(const VirtualNode &copy) = delete;

    virtual ~VirtualNode();

    String getName();

    uint8_t getFileType();

    Util::ArrayList<VirtualNode*> &getChildren();
    
    virtual uint64_t getLength();

    virtual uint64_t readData(char *buf, uint64_t pos, uint64_t numBytes);

    virtual uint64_t writeData(char *buf, uint64_t pos, uint64_t numBytes);
};

#endif
