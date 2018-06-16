/*****************************************************************************
*                                                                           *
*                                K E Y B O A R D                            *
*                                                                           *
*---------------------------------------------------------------------------*
* Beschreibung:    Treiber für den Tastaturcontroller des PCs.              *
*                                                                           *
* Autor:           Olaf Spinczyk, TU Dortmund                               *
*                  Modifikationen, Michael Schoettner, 21.12.2016           *
*****************************************************************************/
#ifndef __Keyboard_include__
#define __Keyboard_include__

#include <lib/util/RingBuffer.h>
#include <lib/InputStream.h>
#include "kernel/services/EventBus.h"
#include "Key.h"
#include "kernel/IOport.h"
#include "kernel/interrupts/InterruptHandler.h"
#include "devices/graphics/text/CgaText.h"
#include "kernel/events/input/KeyEvent.h"


#include <kernel/services/GraphicsService.h>
#include <lib/Random.h>
#include <devices/IODevice.h>

class Keyboard : public IODevice {

private:

    Keyboard(const Keyboard &copy); // Verhindere Kopieren

    const static uint32_t KB_BUFFER_SIZE = 4;

    uint8_t code;     // Byte von Tastatur
    uint8_t prefix;   // Prefix von Tastatur
    Key gather;             // letzter dekodierter Key
    uint8_t leds;              // Zustand LEDs

    uint32_t keysPressed;
    uint32_t buffer[KB_BUFFER_SIZE];

    // Benutzte ports des Tastaturcontrollers
    const IOport controlPort;    // Status- (R) u. Steuerregister (W)
    const IOport dataPort;    // Ausgabe- (R) u. Eingabepuffer (W)

    // Bits im Statusregister
    enum { outb = 0x01, inpb = 0x02, auxb = 0x20 };

    // Kommandos an die Tastatur
    struct kbd_cmd {
        enum { set_led = 0xed, set_speed = 0xf3 };
    };
    enum { cpu_reset = 0xfe };

    // Namen der LEDs
    struct led {
        enum { caps_lock = 4, num_lock = 2, scroll_lock = 1 };
    };

    // Antworten der Tastatur
    struct kbd_reply {
        enum { ack = 0xfa };
    };

    // Konstanten fuer die Tastaturdekodierung
    enum { break_bit = 0x80, prefix1 = 0xe0, prefix2   = 0xe1 };

    // Klassenvariablen
    static uint8_t normal_tab[];
    static uint8_t shift_tab[];
    static uint8_t alt_tab[];
    static uint8_t asc_num_tab[];
    static uint8_t scan_num_tab[];

    // Interpretiert die Make und Break-Codes der Tastatur.
    bool key_decoded ();

    // Ermittelt anhand von Tabellen den ASCII-Code.
    void get_ascii_code ();

    void addToBuffer (uint32_t scancode);

    void removeFromBuffer (uint32_t scancode);

    EventBus *eventBus;

    Util::RingBuffer<KeyEvent> eventBuffer;

public:

    uint8_t lastKey;   // zuletzt gedrueckte Taste

    // Initialisierung der Tastatur.
    Keyboard ();

    // Tastaturabfrage (vorerst Polling)
    Key keyHit();

    // Letzter dekodierter Tastenanschlag
    Key lastHit ();

    void resetLast();

    // Fuehrt einen Neustart des Rechners durch.
    void reboot();

    // Einstellen der Wiederholungsrate der Tastatur.
    void setRepeatRate(uint32_t speed, uint32_t delay);

    // Setzt oder loescht die angegebene Leuchtdiode.
    void setLed(uint8_t led, bool on);

    // Aktivierung der Unterbrechungen fuer die Tastatur
    void plugin();

    // Unterbrechnungsroutine der Tastatur.
    void trigger() override;

    bool checkForData() override;

    uint32_t getKeysPressed();

    bool isKeyPressed (uint32_t scancode);
};

#endif
