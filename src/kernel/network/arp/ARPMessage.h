//
// Created by hannes on 02.06.21.
//

#ifndef HHUOS_ARPMESSAGE_H
#define HHUOS_ARPMESSAGE_H

#include <kernel/network/ethernet/EthernetDataPart.h>

class ARPMessage final : public EthernetDataPart {
private:
    typedef struct arpHeader {
        //See RFC 826 page 3 for details
        uint16_t hardwareType = 0;
        uint16_t protocolType = 0;
        uint8_t hardwareAddressLength = 0;
        uint8_t protocolAddressLength = 0;
        uint16_t opCode = 0; //1 is REQUEST, 2 is REPLY, 0 is undefined here
    } header_t;

    typedef struct arpBody {
        uint8_t *senderHardwareAddress = nullptr;
        uint8_t *senderProtocolAddress = nullptr;

        uint8_t *targetHardwareAddress = nullptr;
        uint8_t *targetProtocolAddress = nullptr;
    } body_t;

    header_t header;
    body_t body;

    uint8_t do_copyTo(Kernel::NetworkByteBlock *output) final;

    uint16_t do_getLengthInBytes() final;

    EtherType do_getEtherType() final;

    String do_asString(String spacing) final;

public:

    enum class OpCode {
        REQUEST = 1,
        REPLY = 2,
        INVALID
    };

    //Sending constructor
    explicit ARPMessage(uint16_t hardwareType, uint16_t protocolType, uint8_t hardwareAddressLength,
                        uint8_t protocolAddressLength, OpCode opCode);

    //Incoming constructor
    ARPMessage() = default;

    static uint16_t getOpCodeAsInt(ARPMessage::OpCode opCode);

    ~ARPMessage() override;

    [[nodiscard]] OpCode getOpCode() const;

    static ARPMessage::OpCode parseOpCodeFromInteger(uint16_t value);

    void setSenderHardwareAddress(uint8_t *senderHardwareAddress) const;

    void setSenderProtocolAddress(uint8_t *senderProtocolAddress) const;

    void setTargetHardwareAddress(uint8_t *targetHardwareAddress) const;

    void setTargetProtocolAddress(uint8_t *targetProtocolAddress) const;

    [[nodiscard]] uint8_t *getTargetHardwareAddress() const;

    [[nodiscard]] uint8_t *getSenderProtocolAddress() const;

    [[nodiscard]] uint8_t *getSenderHardwareAddress() const;

    [[nodiscard]] uint8_t *getTargetProtocolAddress() const;

    [[nodiscard]] uint16_t getProtocolType() const;

    [[nodiscard]] uint16_t getBodyLengthInBytes() const;

    uint8_t parse(Kernel::NetworkByteBlock *input);

    ARPMessage *buildReply(uint8_t *ourAddressAsBytes) const;
};


#endif //HHUOS_ARPMESSAGE_H
