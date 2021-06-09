//
// Created by hannes on 17.05.21.
//

#ifndef HHUOS_ICMP4DESTINATIONUNREACHABLE_H
#define HHUOS_ICMP4DESTINATIONUNREACHABLE_H


#include <kernel/network/internet/icmp/ICMP4Message.h>
#include <kernel/network/NetworkByteBlock.h>
#include <kernel/network/internet/IP4Datagram.h>

class ICMP4DestinationUnreachable final : public ICMP4Message {
private:
    typedef struct destinationUnreachableHeader {
        uint8_t type = 3; // RFC792 page 4
        uint8_t code = 0;
        uint16_t checksum = 0;
        uint32_t unused = 0; //32 Bits are unused per definition in RFC792 page 4
    } header_t;
    header_t header;
    NetworkByteBlock *internalBytes = nullptr;
public:
    ICMP4DestinationUnreachable(uint8_t errorCode, IP4Datagram *datagram);

    explicit ICMP4DestinationUnreachable(size_t ip4HeaderSize);

    virtual ~ICMP4DestinationUnreachable();

    uint8_t copyTo(NetworkByteBlock *byteBlock) override;

    size_t getLengthInBytes() override;

    ICMP4MessageType getICMP4MessageType() override;

    uint8_t parseHeader(NetworkByteBlock *input) override;

    uint8_t parseDataBytes(NetworkByteBlock *input);

    void collectDatagramAttributes(Util::ArrayList<String> *strings);
};


#endif //HHUOS_ICMP4DESTINATIONUNREACHABLE_H
