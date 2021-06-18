//
// Created by hannes on 15.06.21.
//

#ifndef HHUOS_UDP4HEADER_H
#define HHUOS_UDP4HEADER_H


#include <cstdint>
#include <kernel/network/NetworkByteBlock.h>

class UDP4Header {
private:
    uint16_t
            sourcePort = 0,
            destinationPort = 0,
            length = 0,
            checksum = 0;

public:
    UDP4Header(uint16_t sourcePort, uint16_t destinationPort, Kernel::NetworkByteBlock *dataBytes);

    UDP4Header() = default;

    uint8_t copyTo(Kernel::NetworkByteBlock *output) const;

    uint8_t parse(Kernel::NetworkByteBlock *input);

    static size_t getHeaderLength();

    [[nodiscard]] size_t getTotalDatagramLength() const;

    [[nodiscard]] uint16_t getSourcePort() const;

    [[nodiscard]] uint16_t getDestinationPort() const;

};


#endif //HHUOS_UDP4HEADER_H
