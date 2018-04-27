//
// Created by burak on 09.04.18.
//

#ifndef HHUOS_MEMORYNODE_H
#define HHUOS_MEMORYNODE_H


#include <filesystem/RamFs/VirtualNode.h>

class MemoryNode : public VirtualNode {

protected:
    String cache;
public:
    /**
     * Constructor
     */
    MemoryNode(String name, FsNode::FILE_TYPE type);

    /**
     * Copy-constructor.
     */
    MemoryNode(const MemoryNode &copy) = delete;

    /**
     * Destructor.
     */
    ~MemoryNode() override = default;

    /**
     * Overriding function from VirtualNode.
     */
    uint64_t getLength() override;

    /**
     * Overriding function from VirtualNode.
     */
    uint64_t readData(char *buf, uint64_t pos, uint64_t numBytes) override;

    /**
     * Overriding function from VirtualNode.
     */
    uint64_t writeData(char *buf, uint64_t pos, uint64_t numBytes) override;

    virtual void getValues() = 0;


};


#endif //HHUOS_MEMORYNODE_H
