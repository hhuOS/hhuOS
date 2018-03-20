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

    bool readData(char *buf, uint64_t pos, uint64_t numBytes) override;

    bool writeData(char *buf, uint64_t pos, uint64_t numBytes) override;
};

#endif