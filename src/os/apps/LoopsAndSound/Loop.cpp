#include <lib/Random.h>
#include <devices/graphics/text/TextDriver.h>
#include <kernel/services/TimeService.h>
#include <kernel/services/GraphicsService.h>
#include "Loop.h"

#include "kernel/Kernel.h"

Mutex Loop::printLock;

Loop::Loop(uint32_t id) : Thread("Loop") {
    myID = id;
}

void Loop::run () {
    auto *stream = (Kernel::getService<GraphicsService>())->getTextDriver();
    auto *timeService = Kernel::getService<TimeService>();

    for (uint32_t i = 0; isRunning; i++) {
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