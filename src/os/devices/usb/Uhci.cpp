#include <kernel/Kernel.h>
#include <cstdint>
#include <kernel/memory/SystemManagement.h>
#include "Uhci.h"

Logger &Uhci::log = Logger::get("UHCI");

Uhci::~Uhci() {
    delete usbCommandPort;
    delete usbStatusPort;
    delete usbInterruptEnablePort;
    delete frameNumberPort;
    delete frameListBaseAddressPort;
    delete startOfFrameModifyPort;
    delete statusControlPort1;
    delete statusControlPort2;

    SystemManagement::getInstance()->freeIO(frameList);
}

void Uhci::setup(const Pci::Device &device) {
    uint32_t baseAddress = Pci::readDoubleWord(device.bus, device.device, device.function, Pci::PCI_HEADER_BAR4) & 0x0000ffe0;

    log.trace("IO base address: 0x%08x", baseAddress);

    timeService = Kernel::getService<TimeService>();

    usbCommandPort = new IOport(static_cast<uint16_t>(baseAddress + IORegisterOffset::USB_COMMAND_REGISTER));
    usbStatusPort = new IOport(static_cast<uint16_t>(baseAddress + IORegisterOffset::USB_STATUS_REGISTER));
    usbInterruptEnablePort = new IOport(static_cast<uint16_t>(baseAddress + IORegisterOffset::USB_INTERRUPT_REGISTER));
    frameNumberPort = new IOport(static_cast<uint16_t>(baseAddress + IORegisterOffset::FRAME_NUMBER_REGISTER));
    frameListBaseAddressPort = new IOport(static_cast<uint16_t>(baseAddress + IORegisterOffset::FRAME_LIST_BASE_ADDRESS_REGISTER));
    startOfFrameModifyPort = new IOport(static_cast<uint16_t>(baseAddress + IORegisterOffset::START_OF_FRAME_MODIFY_REGISTER));
    statusControlPort1 = new IOport(static_cast<uint16_t>(baseAddress + IORegisterOffset::PORT_1_STATUS_CONTROL_REGISTER));
    statusControlPort2 = new IOport(static_cast<uint16_t>(baseAddress + IORegisterOffset::PORT_2_STATUS_CONTROL_REGISTER));

    frameList = static_cast<FrameListPointer *>(SystemManagement::getInstance()->mapIO(1024 * sizeof(FrameListPointer)));

    memset(frameList, 0, 1024 * sizeof(FrameListPointer));

    for(uint32_t i = 0; i < 1024; i++) {
        frameList->terminate = 1;
    }

    Pci::enableBusMaster(device.bus, device.device, device.function);
    Pci::enableIoSpace(device.bus, device.device, device.function);

    stopHostController();

    resetHostController();

    disableInterrupts();

    //Reset command and status registers
    usbCommandPort->outw(0x00);
    statusControlPort1->outw(0x00);
    statusControlPort2->outw(0x00);

    // Set SOF Timing to 1 ms
    startOfFrameModifyPort->outb(0x40);

    // Set maximum packet size to 64 byte
    usbCommandPort->outb(MAX_PACKET);

    // Set frame list address
    frameNumberPort->outb(0x00);
    frameListBaseAddressPort->outdw(reinterpret_cast<uint32_t>(frameList));

    startHostController();

    usbCommandPort->outw(usbCommandPort->inw() | UsbCommand::CONFIGURE_FLAG);

    resetPort(1);
    resetPort(2);

    if(statusControlPort1->inw() & UsbPortControlStatus::CURRENT_CONNECT_STATUS) {
        log.trace("Device detected on port 1");
    }

    if(statusControlPort2->inw() & UsbPortControlStatus::CURRENT_CONNECT_STATUS) {
        log.trace("Device detected on port 2");
    }
}

void Uhci::resetHostController() {
    log.trace("Resetting Host Controller");

    // Send reset command
    usbCommandPort->outw(usbCommandPort->inw() | UsbCommand::HOST_CONTROLLER_RESET);

    // Wait for the controller to reset itself
    while(usbCommandPort->inw() & UsbCommand::HOST_CONTROLLER_RESET);

    log.trace("Successfully reset Host Controller");
}

void Uhci::startHostController() {
    log.trace("Starting Host Controller");

    // Send start command
    usbCommandPort->outw(usbCommandPort->inw() | UsbCommand::RUN_STOP);

    log.trace("Successfully started Host Controller");
}

void Uhci::stopHostController() {
    log.trace("Stopping Host Controller");

    // Send stop command
    usbCommandPort->outw(usbCommandPort->inw() & ~UsbCommand::RUN_STOP);

    // Wait for the controller to finish processing the current transaction and stop
    while(!(usbStatusPort->inw() & UsbStatus::HOST_CONTROLLER_HALTED));

    log.trace("Successfully stopped Host Controller");
}

void Uhci::disableInterrupts() {
    usbInterruptEnablePort->outw(0x00);
}

void Uhci::enableInterrupt(Uhci::UsbInterrupt interrupt) {
    usbInterruptEnablePort->outw(usbInterruptEnablePort->inw() | interrupt);
}

void Uhci::resetPort(uint8_t portNum) {
    if(portNum != 1 && portNum != 2) {
        log.trace("Trying to reset an undefined port (%u)! Aborting...", portNum);

        return;
    }

    log.trace("Resetting port %u", portNum);

    IOport *port = portNum == 1 ? statusControlPort1 : statusControlPort2;

    // Clear change flags
    port->outw(port->inw() | UsbPortControlStatus::CONNECT_STATUS_CHANGE | UsbPortControlStatus::PORT_ENABLED_DISABLED_CHANGE);

    // Reset port
    port->outw(port->inw() | UsbPortControlStatus::PORT_RESET);
    timeService->msleep(100);

    port->outw(port->inw() & ~UsbPortControlStatus::PORT_RESET);
    timeService->msleep(10);

    log.trace("Successfully reset port %u", portNum);
}
