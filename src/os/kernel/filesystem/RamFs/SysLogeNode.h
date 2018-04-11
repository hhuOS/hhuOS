#ifndef HHUOS_SYSLOGNODE_H
#define HHUOS_SYSLOGNODE_H


#include "VirtualNode.h"

class SysLogNode : public VirtualNode {

private:
    // nothing private
    String cache;

    void getLogsFromLogger();

public:
    SysLogNode();
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


#endif //HHUOS_SYSLOGNODE_H
