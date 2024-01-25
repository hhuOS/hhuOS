#ifndef UsbInterruptHandler__include
#define UsbInterruptHandler__include

#include "../../../kernel/process/ThreadState.h"
#include "../../../kernel/interrupt/InterruptHandler.h"
#include "UsbController.h"

namespace Device::Usb {

class UsbInterruptHandler: public Kernel::InterruptHandler{

public:
    
    UsbInterruptHandler(void (*t)(void* c), uint8_t irq, void* c);

    UsbInterruptHandler(const UsbInterruptHandler& other) = delete;

    UsbInterruptHandler &operator=(const UsbInterruptHandler& other) = delete;

    void plugin() override;

    void trigger(const Kernel::InterruptFrame& frame) override;

private:
    void (*handler_function)(void* c);
    uint8_t irq;
    void* controller;
};

};

#endif