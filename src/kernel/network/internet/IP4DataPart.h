//
// Created by hannes on 14.05.21.
//

#ifndef HHUOS_IP4DATAPART_H
#define HHUOS_IP4DATAPART_H

#include <kernel/network/NetworkTransmittable.h>

class IP4DataPart : public NetworkTransmittable {
protected:
    IP4DataPart();

public:
    //see RFC 790 page 6
    enum IP4ProtocolType : uint8_t {
        ICMP4 = 1,
        UDP = 17,
        INVALID = 0
    };

    ~IP4DataPart() override;

    // forbid copying
    IP4DataPart(IP4DataPart const &) = delete;

    IP4DataPart &operator=(IP4DataPart const &) = delete;


    static IP4ProtocolType parseIntAsIP4ProtocolType(uint8_t value) {
        switch (value) {
            case 1:
                return IP4ProtocolType::ICMP4;
            case 17:
                return IP4ProtocolType::UDP;
            default:
                return IP4ProtocolType::INVALID;
        }
    }

    IP4ProtocolType getIP4ProtocolType();

private:
    virtual IP4ProtocolType do_getIP4ProtocolType() = 0;
};


#endif //HHUOS_IP4DATAPART_H
