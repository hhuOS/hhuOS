/*****************************************************************************
 *                                                                           *
 *                                   P I T                                   *
 *                                                                           *
 *---------------------------------------------------------------------------*
 * Beschreibung:    Programmable Interval Timer.                             *
 *                                                                           *
 * Autor:           Michael Schoettner, 25.8.2016                            *
 *****************************************************************************/

#include <kernel/Kernel.h>
#include <kernel/interrupts/IntDispatcher.h>
#include <kernel/interrupts/Pic.h>
#include "kernel/threads/Scheduler.h"
#include "devices/Pit.h"


IOport control(0x43);
IOport data0(0x40);

Pit* Pit::pit = nullptr;

/*****************************************************************************
 * Methode:         PIT::interval                                            *
 *---------------------------------------------------------------------------*
 * Beschreibung:    Zeitinervall programmieren.                              *
 *****************************************************************************/
void Pit::interval (int us) {
    int divisor = (us * 1000) / time_base;

    control.outb(0x36);
    data0.outb(divisor & 0xff);
    data0.outb(divisor >> 8);
}


/*****************************************************************************
 * Methode:         PIT::plugin                                              *
 *---------------------------------------------------------------------------*
 * Beschreibung:    Unterbrechungen fuer den Zeitgeber erlauben. Ab sofort   *
 *                  wird bei Ablauf des definierten Zeitintervalls die       *
 *                  Methode 'trigger' aufgerufen.                            *
 *****************************************************************************/
void Pit::plugin () {

    // TODO(krakowski):
    //  Do this inside the constructor once all Global Objects
    //  are encapsulated in separate services
    timeService = (TimeService*) Kernel::getService(TimeService::SERVICE_NAME);
    graphicsService = (GraphicsService *) Kernel::getService(GraphicsService::SERVICE_NAME);

    IntDispatcher::assign(32, *this);
    Pic::getInstance()->allow(0);
}


/*****************************************************************************
 * Methode:         PIT::trigger                                             *
 *---------------------------------------------------------------------------*
 * Beschreibung:    ISR fuer den Zeitgeber. Wird aufgerufen, wenn der        *
 *                  Zeitgeber eine Unterbrechung ausloest. Anzeige der Uhr   *
 *                  aktualisieren und Thread wechseln durch Setzen der       *
 *                  Variable 'threadSwitch', wird in 'int_disp' behandelt.   *
 *****************************************************************************/
void Pit::trigger () {

    // alle 10ms, Systemzeit weitersetzen
     timeService->tick();

    // Bei jedem Tick einen Threadwechsel ausloesen.
    // Aber nur wenn der Scheduler bereits fertig intialisiert wurde
    // und ein weiterer Thread rechnen moechte
    if (Scheduler::getInstance()->isInitialized()) {
        Scheduler::getInstance()->yield();
    }
}

/**
 * Enables/Disables the cursor in VESA-Mode.
 *
 * @param cursor The desired cursor-state (Enabled/Disabled).
 */
void Pit::setCursor(bool cursor) {
    this->cursor = cursor;
}

/**
 * Returns the cursor-state (Enabled/Disabled).
 *
 * @return true, if the cursor is enabled;
 *         false, if the cursor is disabled.
 */
bool Pit::isCursorEnabled() {
    return this->cursor;
}
