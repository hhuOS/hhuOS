#ifndef __StderrNode_include__
#define __StderrNode_include__

#include "VirtualNode.h"
#include <stdint.h>
#include <kernel/services/StdStreamService.h>

class StderrNode : public VirtualNode {

private:
    StdStreamService *stdStreamService = nullptr;

public:

    StderrNode();

    ~StderrNode() override = default;

    StderrNode(const StderrNode &copy) = delete;

    uint64_t getLength() override;

    char *readData(char *buf, uint64_t pos, uint64_t numBytes) override;

    int64_t writeData(char *buf, uint64_t pos, uint64_t numBytes) override;
};

#endif