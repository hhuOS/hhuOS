#ifndef __UsbEndpoint_include__
#define __UsbEndpoint_include__

#include <stdint.h>

/**
 * @author Filip Krakowski
 */
class UsbEndpoint {

public:

    typedef struct {
        uint8_t     length;                 // Length
        uint8_t     type;                   // Type
        uint8_t     address;                // Endpoint Address
        uint8_t     attributes;             // Attributes
        uint16_t    maxPacketSize;          // Max Packet Size
        uint8_t     interval;               // Polling Interval
    } Descriptor;

    /* Subclass Codes */
    const static uint8_t SUBCLASS_SCSI = 0x06;

    /* Protocol Codes */
    const static uint8_t PROTOCOL_BBB = 0x50;

    enum Direction {
        OUT, IN
    };

    enum TransferType {
        CONTROL, ISOCHRONOUS, BULK, INTERRUPT
    };

    UsbEndpoint(Descriptor *descriptor);

    uint8_t getAddress() const;

    void setAddress(uint8_t address);

    uint16_t getMaxPacketSize() const;

    void setMaxPacketSize(uint16_t maxPacketSize);

    uint8_t getInterval() const;

    void setInterval(uint8_t interval);

    TransferType getTransferType() const;

    void setTransferType(TransferType transferType);

    Direction getDirection() const;

    void print();

private:

    UsbEndpoint::Descriptor descriptor;

    uint8_t address;
    uint8_t interval;
    uint16_t maxPacketSize;

    TransferType transferType;
    Direction direction;

    void parse(Descriptor *descriptor);
};

#endif