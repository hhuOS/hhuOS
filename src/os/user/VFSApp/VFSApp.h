#ifndef __VFSdemo_include__
#define __VFSdemo_include__


#include <kernel/services/ModuleLoader.h>
#include <kernel/events/input/KeyEvent.h>
#include <kernel/services/GraphicsService.h>
#include "kernel/threads/Thread.h"
#include "kernel/Kernel.h"
#include "kernel/events/Receiver.h"
#include "kernel/services/EventBus.h"
#include "lib/OutputStream.h"
#include "devices/graphics/lfb/LinearFrameBuffer.h"



class VFSApp : public Thread, Receiver {

public:

    VFSApp () : Thread ("VFSApp"), Receiver() {
        graphicsService = (GraphicsService *) Kernel::getService(GraphicsService::SERVICE_NAME);
        moduleLoader = (ModuleLoader*) Kernel::getService(ModuleLoader::SERVICE_NAME);
        eventBus = (EventBus*) Kernel::getService(EventBus::SERVICE_NAME);
    }

    VFSApp (const VFSApp &copy) = delete; // Verhindere Kopieren

    ~VFSApp() {
        eventBus->unsubscribe(*this, KeyEvent::TYPE);
    }
    
    void onEvent(const Event &event) override ;

    // Thread-Startmethode
    void run ();

private:

    void executeCommand();

    GraphicsService *graphicsService = nullptr;

    ModuleLoader *moduleLoader = nullptr;

    EventBus *eventBus = nullptr;
};

#endif
