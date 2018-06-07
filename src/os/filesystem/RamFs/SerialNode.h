#ifndef HHUOS_SERIALNODE_H
#define HHUOS_SERIALNODE_H

#include <kernel/services/SerialService.h>
#include <kernel/Kernel.h>
#include "VirtualNode.h"

/**
 * Implementation of VirtualNode, that writes to and reads from a COM-Port.
 *
 * @author Fabian Ruhland
 * @date 2018
 */
class SerialNode : public VirtualNode {

private:

    Serial *serial = nullptr;

    static String generateName(Serial::ComPort port);

public:
    /**
     * Constructor.
     */
    explicit SerialNode(Serial *serial);

    /**
     * Copy-constructor.
     */
    SerialNode(const SerialNode &copy) = delete;

    /**
     * Destructor.
     */
    ~SerialNode() override = default;

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
};

#endif
