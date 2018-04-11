#include <kernel/interrupts/IntDispatcher.h>
#include <kernel/interrupts/Pic.h>
#include <kernel/Kernel.h>
#include <kernel/services/TimeService.h>
#include <lib/libc/printf.h>
#include "kernel/Cpu.h"
#include "devices/Rtc.h"


/**
 * Enable interrupts for the RTC.
 */
void Rtc::plugin() {
    IntDispatcher::getInstance().assign(40, *this);
    Pic::getInstance()->allow(Pic::Interrupt::RTC);

    Cpu::disableInterrupts();

    //'Update interrupts' einschalten: Immer, wenn die RTC ein update macht,
    //wird ein Interrupt ausgelöst.
    registerPort.outb(STATUS_REGISTER_B);
    char oldValue = dataPort.inb();

    registerPort.outb(STATUS_REGISTER_B);
    dataPort.outb(oldValue | 0x10);

    registerPort.outb(STATUS_REGISTER_A);
    oldValue = dataPort.inb();

    registerPort.outb(STATUS_REGISTER_A);
    dataPort.outb((oldValue & 0xF0) | RTC_RATE);

    if(RTC_DEBUG) {
        registerPort.outb(STATUS_REGISTER_A);
        char freq = dataPort.inb();
        printf("RTC Frequency = %x\n", ((freq & 0b01110000) >> 4));
    }

    registerPort.outb(STATUS_REGISTER_C);
    dataPort.inb();

    Cpu::enableInterrupts();
}

/**
 * Reads the current hardware-time and saves it in the struct 'currentDate'.
 */
void Rtc::trigger() {

    registerPort.outb(STATUS_REGISTER_B);
    char bcd = !(dataPort.inb() & 0x04);

    registerPort.outb(STATUS_REGISTER_B);
    char twelveHour = !(dataPort.inb() & 0x02);

    registerPort.outb(STATUS_REGISTER_C);
    dataPort.inb();

    registerPort.outb(SECONDS_REGISTER);
    currentDate.seconds = dataPort.inb();

    registerPort.outb(MINUTES_REGISTER);
    currentDate.minutes = dataPort.inb();

    registerPort.outb(HOURS_REGISTER);
    currentDate.hours = dataPort.inb();

    registerPort.outb(DAY_OF_MONTH_REGISTER);
    currentDate.dayOfMonth = dataPort.inb();

    registerPort.outb(MONTH_REGISTER);
    currentDate.month = dataPort.inb();

    registerPort.outb(YEAR_REGISTER);
    currentDate.year = dataPort.inb();

    registerPort.outb(CENTURY_REGISTER);
    currentDate.year += dataPort.inb() * 100;

    if(bcd) {
        currentDate.seconds = (currentDate.seconds & 0x0F) + ((currentDate.seconds / 16) * 10);
        currentDate.minutes = (currentDate.minutes & 0x0F) + ((currentDate.minutes / 16) * 10);
        currentDate.hours = ((currentDate.hours & 0x0F) + (((currentDate.hours & 0x70) / 16) * 10)) | (currentDate.hours & 0x80);
        currentDate.dayOfMonth = (currentDate.dayOfMonth & 0x0F) + ((currentDate.dayOfMonth / 16) * 10);
        currentDate.month = (currentDate.month & 0x0F) + ((currentDate.month / 16) * 10);
        currentDate.year = (currentDate.year & 0x0F) + ((currentDate.year / 16) * 10);
    }

    if(twelveHour && (currentDate.hours & 0x80)) {
        currentDate.hours = ((currentDate.hours & 0x7F) + 12) % 24;
    }
}

bool Rtc::checkForData() {
    registerPort.outb(STATUS_REGISTER_C);
    return (dataPort.inb() & 0x10) == 0x10;
}

/**
 * Get the last saved date.
 *
 * @return The date.
 */
Rtc::date Rtc::getCurrentDate() {
    return Rtc::currentDate;
}
