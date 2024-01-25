#include "UsbInterruptHandler.h"
#include "../../../kernel/interrupt/InterruptVector.h"
#include "../../interrupt/InterruptRequest.h"
#include "../../../kernel/process/ThreadState.h"
#include "../../../kernel/service/InterruptService.h"
#include "../../../kernel/system/System.h"

namespace Device::Usb{

UsbInterruptHandler::UsbInterruptHandler(void (*t)(void* c), uint8_t irq, void* c) : handler_function(t), irq(irq), controller(c) {
    
}

void UsbInterruptHandler::plugin(){
    Kernel::InterruptService& interrupt_service = Kernel::System::getService<Kernel::InterruptService>();
    interrupt_service.allowHardwareInterrupt(static_cast<Device::InterruptRequest>(irq));
    interrupt_service.assignInterrupt(static_cast<Kernel::InterruptVector>(irq + 32), *this);
}

void UsbInterruptHandler::trigger(const Kernel::InterruptFrame& frame){
    handler_function(controller);
}

}