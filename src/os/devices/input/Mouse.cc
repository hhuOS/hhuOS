/**
 * A simple ps2 mouse driver
 *
 * @author Burak Akguel, Christian Gesse, Fabian Ruhland, Filip Krakowski, Michael Schoettner
 * @date HHU, 2018
 */
#include <kernel/interrupts/IntDispatcher.h>
#include <kernel/interrupts/Pic.h>
#include <kernel/Kernel.h>
#include <lib/libc/printf.h>
#include <kernel/Cpu.h>
#include <kernel/events/input/MouseMovedEvent.h>
#include <kernel/services/TimeService.h>
#include "devices/input/Mouse.h"


/**
 * Wait until new commands can be sent to controller.
 */
void Mouse::waitControl() {
    int time_out = 100000;
    // Polling until timeout
    while(time_out--) {
        // check if second bit is set -> controller is ready to 'process' commands
        if((ctrl_port.inb() & 2) == 0)
            return;
    }
}

/**
 * Wait until new data is there
 */
void Mouse::waitData() {
    int time_out = 100000;
    // Polling until timeout
    while(time_out--) {
        // check if first bit is set -> data is there
        if((ctrl_port.inb() & 1) == 1)
            return;
    }
}

/**
 * Reads a byte from the data port
 * @return
 */
unsigned char Mouse::read() {
    // wait until data is there
    waitData();
    return data_port.inb();
}

/**
 * writes byte to command port
 * @param byte_write
 */
void Mouse::write(unsigned char byte_write) {
    waitControl();
    ctrl_port.outb(0xD4); // D4 - write next data byte to second ps2 input buffer (in this case mouse)
    waitControl(); // wait for ok
    data_port.outb(byte_write);
}

/**
 * Activate mouse as auxiliary ps2 device
 */
void Mouse::activate() {
    // Statusbyte
    unsigned char status;

    // Test if mouse is there
    waitControl();
    ctrl_port.outb(0xA9); // A9 - tests second ps2 port (works only if two ps2 ports supported)
    waitData();
    uint8_t tmp = data_port.inb();
    if(tmp == 0xFF) {
        MOUSE_TRACE("No mouse detected\n");
        available = false;
        return;
    } else {
        MOUSE_TRACE("Mouse maybe detected\n");
        available = true;
    }

    // deactivate keyboard in ps2 status byte
    waitControl();
    ctrl_port.outb(0x20);  				// 20 - read status byte from controller
    waitData();
    status = data_port.inb() & 0xFC;	// deactivate mouse and keyboard interrupts in controller
    status |= 0x10;						// deactivate keyboard too (1 means disabled)
    waitControl();

    // write modified byte back to controller
    ctrl_port.outb(0x60);
    waitControl();
    data_port.outb(status);

    // activate auxiliary device
    waitControl();
    ctrl_port.outb(0xA8); // A8 - enables second ps2 port

    // activate mouse and keyboard interrupts
    waitControl();
    ctrl_port.outb(0x20);				// read status byte from controller
    waitData();
    status = data_port.inb() | 3;		// activate mouse and keyboard interrupts (first two bits in status byte)
    status &= ~0x10;					// activate keyboard
    waitControl();
    ctrl_port.outb(0x60); // write modified byte back to controller
    waitControl();
    data_port.outb(status);

    // use mouse default settings
    writeCommand(0xF6, "Set Defaults");
    // set resolution
    writeCommandAndByte(0xE8, 0x02, "Set Resolution");
    // set sampling to 80 packets per second
    writeCommandAndByte(0xF3, 80, "Set Sampling Rate");
    // activate mouse packet streaming
    writeCommand(0xF4, "Enable packet streaming");

}

/**
 * sends a command to mouse controller and waits for acknowledgement
 * @param byte_write
 * @param commandString
 */
void Mouse::writeCommandAndByte(unsigned char byte_write, unsigned char data, char* commandString) {
    if(!available) {
        return;
    }

    unsigned char tmp;
    uint8_t cnt = 0;
    write(byte_write); // Befehl senden
    while((tmp = read()) != 0xFA){ // ack warten
        MOUSE_TRACE("ERROR >> Couldn't get ack from %s command\n", commandString);
        if( tmp == 0xFE ) {
            write(byte_write); // send command byte again
        }

        cnt ++;
        if(cnt == 5) {
            MOUSE_TRACE("ERROR >> Couldn't receive ACK from mouse - waited 5 times (Mouse will be disabled)\n");
            available = false;
            cleanup();
            return;
        }
    }
    write(data); // write payload for command to data port
    while((tmp = read()) != 0xFA){ // wait for ack
        if( tmp == 0xFE ) {
            write(data);
        }
    }

}

/**
 * sends a command to mouse controller and waits for acknowledge
 * @param byte_write
 * @param commandString
 */
void Mouse::writeCommand(unsigned char byte_write, char* commandString) {
    if(!available) {
        return;
    }

    unsigned char tmp;
    uint8_t cnt = 0;

    write(byte_write); // Befehl senden
    while((tmp = read()) != 0xFA){ // ack warten
        MOUSE_TRACE("ERROR >> Couldn't get ack from %s command\n", commandString);
        if( tmp == 0xFE ) {
            write(byte_write); // send command byte again
        }

        cnt ++;
        if(cnt == 5) {
            MOUSE_TRACE("ERROR >> Couldn't receive ACK from mouse - waited 5 times (Mouse will be disabled)\n");
            available = false;
            cleanup();
            return;
        }
    }
}

/**
 * Constructor
 */
Mouse::Mouse() : ctrl_port(0x64), data_port(0x60), movedEventBuffer(1024), clickedEventBuffer(1024), releasedEventBuffer(1024), doubleclickEventBuffer(1024) {
    buttons = 0;
    cycle = 1;

    activate();
}

/**
 * Allows Mouse interrupts in PIC and assigns Mouse class as interrupt handler for Mouse interrupts
 */
void Mouse::plugin() {
    if(available) {
        timeService = Kernel::getService<TimeService>();
        eventBus = Kernel::getService<EventBus>();

        IntDispatcher::getInstance().assign(IntDispatcher::mouse, *this);
        Pic::getInstance()->allow(Pic::Interrupt::MOUSE);
    }
}

/**
 * Interrupt handler
 *
 * The mouse sends for each 'event' 3 bytes
 *  -> the first byte has information about clicked buttons and movement type (signed or unsigned)
 *  -> second byte x Movement
 *  -> third byte y Movement
 */
void Mouse::trigger() {
    // check if mouse data is there
    unsigned int status = ctrl_port.inb();
    if (!(status & 0x20)) {
        return;
    }

    unsigned int val;
    // certain treatments for each cycle
    switch (cycle) {
        case 1:
            flags = (unsigned int)data_port.inb();
            // Check if the Always 1 Bit is Set - hacky way to check if it is really the first cycle
            if(!(flags & 0x08)) {
                cycle = 1;
            } else {
                cycle++;
            }
            break;

        case 2:
            val = (unsigned int)data_port.inb();
            // just get the correct bits
            val &= (unsigned int)0xFF;
            // check if signed
            if(flags & 0x10) {
                // extend unsigned 8 bit value to unsigned 32 bit in twos complement
                dx = val | 0xFFFFFF00;
            } else {
                dx = val;
            }
            cycle++;
            break;

        case 3:
            val = (unsigned int)data_port.inb();
            // just get the correct bits
            val &= (unsigned int)0xFF;
            // check if signed
            if (flags & 0x20) {
                // extend unsigned 8 bit value to unsigned 32 bit in twos complement
                dy = - (val | 0xFFFFFF00);
            } else {
                dy = - (val);
            }

            // if there was a x or y Overflow -> discard this 'event'
            if(!(flags & 0x40 || flags & 0x80)){
                // check if mouse was moved
                if(dx != 0 || dy != 0) {
                    // put MouseMovedEvent to queue and publish event
                    movedEventBuffer.push(MouseMovedEvent(dx, dy));
                    eventBus->publish(movedEventBuffer.pop());
                }
            }

            bool clickEventOccurred= false;
            uint8_t clickMask = 0;

            // check if any button was clicked/released
            bool releaseEventOccurred= false;
            uint8_t releaseMask = 0;
            for(int i = 0; i < 3; i++)
            {
                // check for state change
                if((flags & (0x1<<i)) != (buttons & (0x1<<i)))
                {
                    // Check if mouse was clicked or released
                    if(buttons & (0x1<<i)) {
                        // Released
                        releaseEventOccurred = true;
                        releaseMask |= (0x1<<i);
                    } else {
                        // Clicked
                        uint32_t tmpTstmp = timeService->getSystemTime();
                        clickEventOccurred = true;
                        clickMask|= (0x1<<i);

                        if((tmpTstmp - lastClickTimestamp) < 3000) { // three second tolerance for double click
                            doubleclickEventBuffer.push(MouseDoubleClickEvent());
                            eventBus->publish(doubleclickEventBuffer.pop());
                            lastClickTimestamp = 0;
                        } else {
                            lastClickTimestamp = tmpTstmp;
                        }
                    }
                }
            }

            // if new buttons were clicked -> publish new MouseClickedEvent
            if(clickEventOccurred) {
                clickedEventBuffer.push(MouseClickedEvent(clickMask));
                eventBus->publish(clickedEventBuffer.pop());
            }

            // if new buttons were released -> publish new MouseReleasedEvent
            if(releaseEventOccurred) {
                releasedEventBuffer.push(MouseReleasedEvent(releaseMask));
                eventBus->publish(releasedEventBuffer.pop());
            }

            // remember clicked buttons
            buttons = flags;
            // reset cycle
            cycle = 1;
            break;
    }


}

/**
 * No mouse was detected or mouse is defect -> cleanup
 */
void Mouse::cleanup() {
    // forbid mouse interrupts
    Pic::getInstance()->forbid(Pic::Interrupt::MOUSE);
    uint8_t status;

    waitControl();
    ctrl_port.outb(0x20);  				// reads controller status byte
    waitData();
    status = data_port.inb() & 0xFC;	// deactivate mouse and keyboard interrupts
    status |= 0x10;						// disable keyboard

    waitControl();
    ctrl_port.outb(0x60);               // write modified status byte back to controller
    waitControl();
    data_port.outb(status);

    // deactivate auxiliary device
    waitControl();
    ctrl_port.outb(0xA7);

    // activate keyboard interrupts and enable keyboard
    waitControl();
    ctrl_port.outb(0x20);				// read controller status byte
    waitData();
    status = data_port.inb() | 1;		// activate keyboard interrupts
    // Keyboard aktiviHTC Viveeren
    status &= ~0x10;					// enable keyboard

    waitControl();
    ctrl_port.outb(0x60);
    waitControl();
    data_port.outb(status);
}
