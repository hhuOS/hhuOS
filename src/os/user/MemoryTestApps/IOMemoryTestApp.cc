/** A simple test app for the IO memory manager 
 * 
 * Burak Akguel, Christian Gesse, HHU 2017 **/

#include <kernel/Kernel.h>
#include <devices/graphics/text/TextDriver.h>
#include <kernel/services/GraphicsService.h>

#include "kernel/threads/Scheduler.h"
#include "user/MemoryTestApps/IOMemoryTestApp.h"

#include "../../kernel/memory/SystemManagement.h"
#include "kernel/memory/manager/IOMemoryManager.h"
#include "lib/deprecated/Queue.h"
#include "lib/Byte.h"
#include "lib/Random.h"


void IOMemoryTestApp::run () {

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


    stream << endl << "  Please press <RETURN>" << endl;
    //TODO: Keypress

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

    // selbst terminieren
    Scheduler::getInstance()->exit();
}
