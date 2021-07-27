//
// Created by hannes on 14.05.21.
//

#ifndef HHUOS_IP4DATAPART_H
#define HHUOS_IP4DATAPART_H

#include <kernel/network/NetworkByteBlock.h>

class IP4DataPart {
protected:
    IP4DataPart();

public:
    //see RFC 790 page 6
    enum IP4ProtocolType : uint8_t {
        ICMP4 = 1,
        UDP = 17,
        INVALID = 0
    };

    virtual ~IP4DataPart();

    // forbid copying
    IP4DataPart(IP4DataPart const &) = delete;

    IP4DataPart &operator=(IP4DataPart const &) = delete;

    static IP4ProtocolType parseIntAsIP4ProtocolType(uint8_t value);

    IP4ProtocolType getIP4ProtocolType();

    uint16_t length();

    uint8_t copyTo(Kernel::NetworkByteBlock *output);

private:
    virtual IP4ProtocolType do_getIP4ProtocolType() = 0;

    virtual uint8_t do_copyTo(Kernel::NetworkByteBlock *output) = 0;

    virtual uint16_t do_length() = 0;
};


#endif //HHUOS_IP4DATAPART_H
