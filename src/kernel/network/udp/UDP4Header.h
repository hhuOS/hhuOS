//
// Created by hannes on 15.06.21.
//

#ifndef HHUOS_UDP4HEADER_H
#define HHUOS_UDP4HEADER_H


#include <cstdint>
#include <kernel/network/udp/sockets/UDP4Port.h>
#include <kernel/network/NetworkByteBlock.h>

class UDP4Header {
private:
    typedef struct udp4Header {
        uint16_t sourcePort = 0;
        uint16_t destinationPort = 0;
        uint16_t length = 0;
        uint16_t checksum = 0;
    } header_t;

    header_t header;
    UDP4Port *sourcePort = nullptr;
    UDP4Port *destinationPort = nullptr;

public:
    UDP4Header(UDP4Port *sourcePort, UDP4Port *destinationPort, Kernel::NetworkByteBlock *dataBytes);

    UDP4Header() = default;

    uint8_t copyTo(Kernel::NetworkByteBlock *output) const;

    uint8_t parse(Kernel::NetworkByteBlock *input);

    size_t getHeaderSize();

    size_t getDatagramLength() const;

    UDP4Port *getSourcePort() const;

    [[nodiscard]] UDP4Port *getDestinationPort() const;

    virtual ~UDP4Header();
};


#endif //HHUOS_UDP4HEADER_H
