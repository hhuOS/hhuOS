#ifndef __PciNode_include__
#define __PciNode_include__

#include "VirtualNode.h"
#include <stdint.h>
#include <lib/String.h>

/**
 * Implementation of VirtualNode, that reads the the IDs of all PCI-devices from the PCI-class.
 *
 * @author Filip Krakowski
 * @date 2018
 */
class PciNode : public VirtualNode {

private:
    String cache;

    void cacheDeviceList();

public:
    /**
     * Constructor
     */
    PciNode();

    /**
     * Copy-constructor.
     */
    PciNode(const PciNode &copy) = delete;

    /**
     * Destructor.
     */
    ~PciNode() override = default;

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