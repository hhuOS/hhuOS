#include <kernel/interrupts/Pic.h>
#include <kernel/interrupts/IntDispatcher.h>
#include <lib/libc/printf.h>
#include <kernel/Kernel.h>
#include "Serial.h"

Serial::Serial(Serial::BaudRate speed) : speed(speed), eventBuffer(1024),
                                          dataRegister(0x3f8), interruptRegister(0x3f9),
                                          fifoControlRegister(0x3fa), lineControlRegister(0x3fb),
                                          modemControlRegister(0x3fc), lineStatusRegister(0x3fd),
                                          modemStatusRegister(0x3fe), scratchRegister(0x3ff)
{
    interruptRegister.outb(0x00);        // Disable all interrupts
    lineControlRegister.outb(0x80);      // Enable to DLAB, so that the divisor can be set

    dataRegister.outb(static_cast<uint8_t>(static_cast<uint16_t>(speed) & 0x0f));       // Divisor low byte
    interruptRegister.outb(static_cast<uint8_t>(static_cast<uint16_t>(speed) >> 8));    // Divisor high byte

    lineControlRegister.outb(0x03);      // 8 bits per char, no parity, one stop bit
    fifoControlRegister.outb(0xc7);      // Enable FIFO-buffers, Clear FIFO-buffers, Trigger interrupt after 14 bytes
    modemControlRegister.outb(0x0b);     // Enable data lines

    eventBus = Kernel::getService<EventBus>();
}

void Serial::sendData(char *data, uint32_t len) {
    for(uint32_t i = 0; i < len; i++) {
        while ((lineStatusRegister.inb() & 0x20) == 0);

        if(data[i] == '\n') {
            dataRegister.outb(13);
        }

        dataRegister.outb(static_cast<uint8_t>(data[i]));
    }
}

void Serial::readData(char *data, uint32_t len) {
    for(uint32_t i = 0; i < len; i++) {
        while ((lineStatusRegister.inb() & 0x1) == 0);

        data[i] = dataRegister.inb();
    }
}

void Serial::plugin() {
    IntDispatcher::getInstance().assign(36, *this);
    Pic::getInstance()->allow(Pic::Interrupt::COM1);

    interruptRegister.outb(0x01);
}

void Serial::trigger() {
    bool hasData;

    do {
        hasData = (lineStatusRegister.inb() & 0x01) == 0x01;

        if(hasData) {
            char c = dataRegister.inb();

            eventBuffer.push(SerialEvent(c));

            SerialEvent &event = eventBuffer.pop();

            eventBus->publish(event);
        }
    } while (hasData);
}

bool Serial::checkForData() {
    return (lineStatusRegister.inb() & 0x01) == 0x01;
}

void Serial::setSpeed(Serial::BaudRate speed) {
    uint8_t interruptBackup = interruptRegister.inb();
    uint8_t lineControlBackup = lineStatusRegister.inb();

    interruptRegister.outb(0x00);                   // Disable all interrupts
    lineControlRegister.outb(0x80);                 // Enable to DLAB, so that the divisor can be set

    dataRegister.outb(static_cast<uint8_t>(static_cast<uint16_t>(speed) & 0x0f));       // Divisor low byte
    interruptRegister.outb(static_cast<uint8_t>(static_cast<uint16_t>(speed) >> 8));    // Divisor high byte

    interruptRegister.outb(interruptBackup);       // Restore interrupt register
    lineControlRegister.outb(lineControlBackup);   // Restore line control register

    this->speed = speed;
}
