#ifndef UsbInterruptHandler__include
#define UsbInterruptHandler__include

#include "../../../kernel/interrupt/InterruptHandler.h"

extern "C"{
#include "UsbController.h"
}

namespace Device::Usb {

class UsbInterruptHandler: public Kernel::InterruptHandler{

public:
    
    UsbInterruptHandler(uint8_t irq, UsbController* c);

    UsbInterruptHandler(const UsbInterruptHandler& other) = delete;

    UsbInterruptHandler &operator=(const UsbInterruptHandler& other) = delete;

    void plugin() override;

    void trigger(const Kernel::InterruptFrame& frame, Kernel::InterruptVector slot) override;

private:
    uint8_t irq;
    UsbController* controller;
};

};

#endif