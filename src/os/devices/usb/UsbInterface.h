#ifndef __UsbInterface_include__
#define __UsbInterface_include__

#include "devices/usb/UsbEndpoint.h"

#include <stdint.h>
#include <lib/LinkedList.h>

/**
 * @author Filip Krakowski
 */
class UsbInterface {

public:

    typedef struct {
        uint8_t     length;                 // Length
        uint8_t     type;                   // Type
        uint8_t     number;                 // Interface Number
        uint8_t     altSetting;             // Alternate Setting
        uint8_t     numEndpoints;           // Number of Endpoints
        uint8_t     classCode;              // Interface Class
        uint8_t     subClassCode;           // Interface Subclass
        uint8_t     protocolCode;           // Interface Subclass
        uint8_t     nameIndex;              // Interface String Index
    } Descriptor;

    UsbInterface(UsbInterface::Descriptor *descriptor);

    uint8_t getId() const;

    void setId(uint8_t id);

    uint8_t getNumEndpoints() const;

    void setNumEndpoints(uint8_t numEndpoints);

    uint8_t getInterfaceClass() const;

    void setInterfaceClass(uint8_t interfaceClass);

    uint8_t getInterfaceSubClass() const;

    void setInterfaceSubClass(uint8_t interfaceSubClass);

    uint8_t getInterfaceProtocol() const;

    void setInterfaceProtocol(uint8_t interfaceProtocol);

    char *getName() const;

    void setName(char *name);

    UsbEndpoint *getEndpoint(uint8_t index) const;

    void addEndpoint(UsbEndpoint *endpoints);

    void print();

private:

    UsbInterface::Descriptor descriptor;

    uint8_t id;
    uint8_t numEndpoints;

    uint8_t interfaceClass;
    uint8_t interfaceSubClass;
    uint8_t interfaceProtocol;

    char *name;

    LinkedList<UsbEndpoint> endpoints;

    void parse(Descriptor *descriptor);
};


#endif
