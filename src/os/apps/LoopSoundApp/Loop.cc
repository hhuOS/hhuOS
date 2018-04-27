/*****************************************************************************
 *                                                                           *
 *                                 L O O P                                   *
 *                                                                           *
 *---------------------------------------------------------------------------*
 * Beschreibung:    Einstieg in eine Anwendung.                              *
 *                                                                           *
 * Autor:           Michael Schoettner, HHU, 26.8.2016                       *
 *****************************************************************************/

#include <lib/Random.h>
#include <devices/graphics/text/TextDriver.h>
#include <kernel/services/TimeService.h>
#include <kernel/services/GraphicsService.h>
#include "apps/LoopSoundApp/Loop.h"

#include "kernel/Kernel.h"


// lock zur Synchronisierung der Threads, damit die Bildschirmausgabe
// nicht durcheinander kommt
Mutex Loop::printLock;


/*****************************************************************************
 * Methode:         Loop::run                                                *
 *---------------------------------------------------------------------------*
 * Beschreibung:    Code des Threads.                                        *
 *****************************************************************************/
void Loop::run () {
    uint32_t i = 0;
    TextDriver *stream = (Kernel::getService<GraphicsService>())->getTextDriver();
    TimeService *timeService = Kernel::getService<TimeService>();

    for (;isRunning;i++) {

        printLock.acquire();

        String string("Loop[");
        string += String::valueOf(myID, 10);
        string += "]: ";
        string += String::valueOf(i, 10);

        stream->setpos (21, static_cast<uint16_t>(10 + 2 * myID));
        stream->puts((char *) string, string.length(), Colors::HHU_GRAY, Colors::BLACK);

        printLock.release();

        timeService->msleep(10);
    }
}
