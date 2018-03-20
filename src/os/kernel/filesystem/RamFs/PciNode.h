#ifndef __PciNode_include__
#define __PciNode_include__

#include "VirtualNode.h"
#include <stdint.h>
#include <lib/String.h>

class PciNode : public VirtualNode {

private:

    String cache;

    void cacheDeviceList();

public:

    PciNode();

    ~PciNode() = default;

    PciNode(const PciNode &copy) = delete;

    uint64_t getLength() override;

    bool readData(char *buf, uint64_t pos, uint64_t numBytes) override;

    bool writeData(char *buf, uint64_t pos, uint64_t numBytes) override;
};

#endif