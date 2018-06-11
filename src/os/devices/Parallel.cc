#include <kernel/Kernel.h>
#include "Parallel.h"

Parallel::Parallel(Parallel::LptPort port) : port(port), dataPort(port), statusPort(port + 1), controlPort(port + 2) {
    initializePrinter();

    timeService = Kernel::getService<TimeService>();
}

void Parallel::initializePrinter() {
    uint8_t control = controlPort.inb();

    controlPort.outb(control | static_cast<uint8_t>(0x06)); // Initialize Printer and enable automatic linefeed
}

bool Parallel::isBusy() {
    return (statusPort.inb() & static_cast<uint8_t>(0x80)) == 0x00; // NOTE: The busy-flag is active low!
}

bool Parallel::isPaperEmpty() {
    return (statusPort.inb() & static_cast<uint8_t>(0x20)) == 0x20;
}

bool Parallel::checkError() {
    return (statusPort.inb() & static_cast<uint8_t>(0x08)) == 0x08;
}

void Parallel::sendChar(char c) {
    while(isBusy()); // Wait for the printer to be ready

    dataPort.outb(static_cast<uint8_t>(c)); // Send the byte

    // Pulse the strobe bit, so that the printer knows, that there is data to be fetched on the data port.
    uint8_t control = controlPort.inb();
    controlPort.outb(control | static_cast<uint8_t>(0x01));
    timeService->msleep(10);
    controlPort.outb(control);

    control = controlPort.inb();

    while(isBusy()); // Wait for the printer to finish reading the data
}

void Parallel::sendData(char *data, uint32_t len) {
    for(uint32_t i = 0; i < len; i++) {
        sendChar(data[i]);
    }
}

Parallel::LptPort Parallel::getPortNumber() {
    return port;
}
