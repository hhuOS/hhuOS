#ifndef __StdoutNode_include__
#define __StdoutNode_include__

#include "VirtualNode.h"
#include <stdint.h>
#include <kernel/services/StdStreamService.h>

class StdoutNode : public VirtualNode {

private:
    StdStreamService *stdStreamService = nullptr;

public:
    StdoutNode();

    ~StdoutNode() override = default;

    StdoutNode(const StdoutNode &copy) = delete;

    uint64_t getLength() override;

    char *readData(char *buf, uint64_t pos, uint32_t numBytes) override;

    int32_t writeData(char *buf, uint64_t pos, uint32_t numBytes) override;
};

#endif