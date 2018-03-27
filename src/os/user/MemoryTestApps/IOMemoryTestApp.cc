/** A simple test app for the IO memory manager 
 * 
 * Burak Akguel, Christian Gesse, HHU 2017 **/

#include <kernel/Kernel.h>
#include <devices/graphics/text/TextDriver.h>
#include <kernel/services/GraphicsService.h>
#include <user/Application.h>

#include "kernel/threads/Scheduler.h"
#include "user/MemoryTestApps/IOMemoryTestApp.h"

#include "../../kernel/memory/SystemManagement.h"


void IOMemoryTestApp::run () {
    TimeService *timeService = Kernel::getService<TimeService>();
    Keyboard *kb = Kernel::getService<InputService>()->getKeyboard();
    TextDriver &stream = *(Kernel::getService<GraphicsService>())->getTextDriver();
    SystemManagement *memoryManagement = SystemManagement::getInstance();

    stream.clear ();
    stream << "Dynamically allocate 2 objects" << endl;
    stream << "==============================" << endl << endl;

    SystemManagement::getInstance()->dumpFreeIOMemBlocks();
    stream << endl;

    stream << "  Allocate objects" << endl;
    stream << "  ================" << endl;
    IOMemInfo o = memoryManagement->mapIO(2*4096);
    
    SystemManagement::getInstance()->dumpFreeIOMemBlocks();

    IOMemInfo o2 = memoryManagement->mapIO(4*4096);

    memoryManagement->dumpFreeIOMemBlocks();

    timeService->msleep(1000);
    stream << endl << "  Please press <RETURN>" << endl;
    while (!kb->isKeyPressed(KeyEvent::RETURN));

    //
    // zwei Objekte loeschen
    //
    stream.clear ();
    stream << "Freeing 2 objects" << endl;
    stream << "================" << endl << endl;


    stream << "  Freeing at " << o.virtStartAddress << endl;
    memoryManagement->freeIO(o);
    stream << "  Freeing at " << o2.virtStartAddress << endl;
    memoryManagement->freeIO(o2);

    stream << endl;

    memoryManagement->dumpFreeIOMemBlocks();

    
    stream << endl;
    stream << "*** END OF DEMO ***" << endl;

    timeService->msleep(1000);
    stream << endl << "  Please press <RETURN>" << endl;
    while (!kb->isKeyPressed(KeyEvent::RETURN));

    while (!kb->isKeyPressed(KeyEvent::RETURN));

    Application::getInstance()->resume();
    Scheduler::getInstance()->exit();
}
