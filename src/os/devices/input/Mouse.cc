//
// Created by burak on 05.03.18.
//

/*****************************************************************************
 *                                                                           *
 *                                M O U S E                                  *
 *                                                                           *
 *---------------------------------------------------------------------------*
 * Beschreibung:    Treiber für den Mauscontroller des PCs.                  *
 *                                                                           *
 * Autor:           Burak Akgül, HHU Düsseldorf, 08.12.2016                  *
 *****************************************************************************/
#include <kernel/interrupts/IntDispatcher.h>
#include <kernel/interrupts/Pic.h>
#include <kernel/Kernel.h>
#include <lib/libc/printf.h>
#include <kernel/Cpu.h>
#include <kernel/events/input/MouseMovedEvent.h>
#include <kernel/services/TimeService.h>
#include "devices/input/Mouse.h"


// warte darauf, dass ein Befehl zum Controller geschickt werden kann
void Mouse::waitControl() {
    // timeOut für das Warten
    int time_out = 100000;
    // Polling solange timeout nicht vorbei
    while(time_out--) {
        // prüfe, ob 2. Bit im Statusregister nicht gesetzt
        // -> Input Buffer leer, es dürfen Befehle geschickt werden
        if((ctrl_port.inb() & 2) == 0)
            return;
    }
}

// warte darauf, dass Daten am Controller zum Lesen bereit liegen
void Mouse::waitData() {
    // timeOut für das Warten
    int time_out = 100000;
    // Polling solange timeout nicht vorbei
    while(time_out--) {
        // prüfe, ob 2. Bit im Statusregister nicht gesetzt
        // -> Input Buffer leer, es dürfen Befehle geschickt werden
        if((ctrl_port.inb() & 1) == 1)
            return;
    }
}

// lese ein Byte vom Controller
unsigned char Mouse::read() {
    // warte auf Daten
    waitData();
    return data_port.inb();
}

// schicke ein Befehlsbyte zum Controller
void Mouse::write(unsigned char byte_write) {
    waitControl(); // Auf ok warten
    ctrl_port.outb(0xD4); // Maus Bescheid geben, dass Befehl geschickt wird (0xD4 -> Kommando an zweites PS2 Device reichen)
    waitControl(); // Auf ok warten
    data_port.outb(byte_write);
}

// aktiviere die Maus am Controller
void Mouse::activate() {
    // Statusbyte
    unsigned char status;

    // Test if mouse is there
    waitControl();
    ctrl_port.outb(0xA9);
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

    // Keyboard deaktivieren
    waitControl();
    ctrl_port.outb(0x20);  				// lese Status Byte des KB Controllers ein
    waitData();
    status = data_port.inb() & 0xFC;	// deaktiviere Maus- und Tastaturinterrupts (erste beide Bits nicht setzen)
    status |= 0x10;						// stelle zusätzlich Keyboard aus (1 heißt deaktiviert)
    waitControl();
    // schreibe Status-Byte zurück
    ctrl_port.outb(0x60);
    waitControl();
    data_port.outb(status);

    // Auxiliary Maus Gerät aktivieren
    waitControl();
    ctrl_port.outb(0xA8);

    // Interrupts aktivieren, sodass die Maus und Keyboard Interrupts auslöst (mit 0x20 aktuellen Status abfragen 0x60 überschreiben)
    waitControl();
    ctrl_port.outb(0x20);				// Status Byte einlesen
    waitData();
    status = data_port.inb() | 3;		// Interrupts für Maus und Keyboard aktivieren (erste beide Bits setzen)
    // Keyboard aktiviHTC Viveeren
    status &= ~0x10;					// stelle Keyboard an
    waitControl();
    ctrl_port.outb(0x60);
    waitControl();
    data_port.outb(status);

    // Default Eigenschaften der Maus verwenden (Optional) [Streaming deaktiviert, 100 Pakete die Sekunde, Auflösung 4px pro mm
    writeCommand(0xF6, "Set Defaults");
    // Setze Auflösung der Maus
    writeCommandAndByte(0xE8, 0x02, "Set Resolution");
    // Setze Anzahl der Pakete die pro Sekunde erzeugt werden sollen
    writeCommandAndByte(0xF3, 80, "Set Sampling Rate");
    // Maus Paket-Streaming aktivieren
    writeCommand(0xF4, "Enable packet streaming");

}

// sendet Befehl an Controller und anschließend die Nutzdaten (mit jeweiligem warten auf ACKs)
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
            write(byte_write); // Controller will Befehl erneut erhalten
        }

        cnt ++;
        if(cnt == 5) {
            MOUSE_TRACE("ERROR >> Couldn't receive ACK from mouse - waited 5 times (Mouse will be disabled)\n");
            available = false;
            cleanup();
            return;
        }
    }
    write(data); // Eigentliche Daten für den Befehl senden
    while((tmp = read()) != 0xFA){ // ack warten
        if( tmp == 0xFE ) {
            write(data);
        }
    }

}

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
            write(byte_write); // Controller will Befehl erneut erhalten
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



// Konstrukor für die maus mit Erstellung der Ports
Mouse::Mouse() : ctrl_port(0x64), data_port(0x60), movedEventBuffer(1024), clickedEventBuffer(1024), releasedEventBuffer(1024), doubleclickEventBuffer(1024) {
    buttons = 0;
    cycle = 1;

    // Maus aktivieren
    activate();
}

// ISR-Routine zum Demaskieren des Interrupts im PIC, Zuweisen des Interrupt-Handlers
void Mouse::plugin() {
    if(available) {
        timeService = ((TimeService *) Kernel::getService(TimeService::SERVICE_NAME));
        eventBus = (EventBus *) Kernel::getService(EventBus::SERVICE_NAME);

        IntDispatcher::assign(IntDispatcher::mouse, *this);
        Pic::getInstance()->allow(Pic::mouse);
    }
}

// Interrupt-Routine
void Mouse::trigger() {
    // prüfe ob maus daten vorliegen
    unsigned int status = ctrl_port.inb();
    if (!(status & 0x20)) {
        return;
    }

    // insgesamt 3 Bytes für ein Datenpaket, pro Interrupt wird ein Byte geschickt
    unsigned int val;	// temp für case 2 und 3
    // handle die 3 Bytes ab
    switch (cycle) {
        // erstes Byte: Flags zu Maustasten, Vorzeichen und Overflow
        case 1:
            flags = (unsigned int)data_port.inb();
            // Prüfe, ob das Always 1 Bit (muss immer im ersten Byte gesetzt sein) vorhanden ist
            // wenn nicht, handelt es sich nicht um das erste Datenpaket der Maus
            if(!(flags & 0x08)) {
                cycle = 1;
            } else {
                cycle++;
            }
            break;
            // zweites Byte: x-Verschiebung der Maus (relativ)
        case 2:
            val = (unsigned int)data_port.inb();
            // Bitmaske, da nur 8 Bits genutzt werden sollen
            val &= (unsigned int)0xFF;
            // prüfen ob sign Bit gesetzt ist
            if(flags & 0x10) {
                // negativen 9 Bit Wert auf 32 Bit Zweierkomplement erweitern (Sign Bit ist in den Flags)
                dx = val | 0xFFFFFF00;
            } else {
                // positives Vorzeichen
                dx = val;
            }
            cycle++;
            break;
            // drittes Byte: y-Verschiebung der Maus (relativ)
        case 3:
            val = (unsigned int)data_port.inb();
            // Bitmaske, da nur 8 Bits genutzt werden sollen
            val &= (unsigned int)0xFF;
            // prüfen ob sign Bit gesetzt ist, "-" Zeichen da Sign Bit sagt, dass Maus nach unten bewegt wurde
            if (flags & 0x20) {
                // negativen 9 Bit Wert auf 32 Bit Zweierkomplement erweitern (Sign Bit ist in den Flags)
                dy = - (val | 0xFFFFFF00);
            } else {
                // positives Vorzeichen
                dy = - (val);
            }

            // falls Overflow in eine Richtung - Paket verwerfen
            if(!(flags & 0x40 || flags & 0x80)){
                // prüfe, ob wirklich Bewegung vorliegt
                if(dx != 0 || dy != 0) {
                    // Rufe den Maus-Handler der laufenden Applikation auf
                    movedEventBuffer.push(MouseMovedEvent(dx, dy));
                    eventBus->publish(movedEventBuffer.pop());
                }
            }


            // Mausklick
            // Schleife, um für jede der drei Tasten zu prüfen
            bool clickEventOccurred= false;
            uint8_t clickMask = 0;

            bool releaseEventOccurred= false;
            uint8_t releaseMask = 0;
            for(int i = 0; i < 3; i++)
            {
                // Prüfe, ob Wechsel des Tastendrucks vorliegt (buttons -> zuletzt gedrückt)
                if((flags & (0x1<<i)) != (buttons & (0x1<<i)))
                {
                    // Prüfe, ob Maustaste gedrückt oder losgelassen wurde
                    if(buttons & (0x1<<i)) {
                        // Released
                        releaseEventOccurred = true;
                        releaseMask |= (0x1<<i);
                    } else {
                        // Clicked
                        uint32_t tmpTstmp = timeService->getSystemTime();
                        clickEventOccurred = true;
                        clickMask|= (0x1<<i);
// TODO: let tolerance be configurable
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
            if(clickEventOccurred) {
                clickedEventBuffer.push(MouseClickedEvent(clickMask));
                eventBus->publish(clickedEventBuffer.pop());
            }

            if(releaseEventOccurred) {
                releasedEventBuffer.push(MouseReleasedEvent(releaseMask));
                eventBus->publish(releasedEventBuffer.pop());
            }

            // merke aktuell gedrückte Buttons
            buttons = flags;
            // setze Zyklus auf Beginn zurück
            cycle = 1;
            break;
    }


}

void Mouse::cleanup() {
    Pic::getInstance()->forbid(Pic::mouse);  //TODO Bugfix - null pointer exception in ahci if forbid is being called
    uint8_t status;
    waitControl();
    ctrl_port.outb(0x20);  				// lese Status Byte des KB Controllers ein
    waitData();
    status = data_port.inb() & 0xFC;	// deaktiviere Maus- und Tastaturinterrupts (erste beide Bits nicht setzen)
    status |= 0x10;						// stelle zusätzlich Keyboard aus (1 heißt deaktiviert)
    waitControl();
    // schreibe Status-Byte zurück
    ctrl_port.outb(0x60);
    waitControl();
    data_port.outb(status);

    // Auxiliary Maus Gerät aktivieren
    waitControl();
    ctrl_port.outb(0xA7);

    // Interrupts aktivieren, sodass die Maus und Keyboard Interrupts auslöst (mit 0x20 aktuellen Status abfragen 0x60 überschreiben)
    waitControl();
    ctrl_port.outb(0x20);				// Status Byte einlesen
    waitData();
    status = data_port.inb() | 1;		// Interrupts für Maus und Keyboard aktivieren (erste beide Bits setzen)
    // Keyboard aktiviHTC Viveeren
    status &= ~0x10;					// stelle Keyboard an
    waitControl();
    ctrl_port.outb(0x60);
    waitControl();
    data_port.outb(status);
}
