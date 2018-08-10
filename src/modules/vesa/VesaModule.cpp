#include <kernel/services/GraphicsService.h>
#include "VesaModule.h"
#include "VesaGraphics.h"
#include "VesaText.h"

MODULE_PROVIDER {

    return new VesaModule();
};

int32_t VesaModule::initialize() {

    auto *graphicsService = Kernel::getService<GraphicsService>();
    
    auto *lfb = new VesaGraphics();
    auto *text = new VesaText();
    
    if(lfb->isAvailable()) {
        graphicsService->setLinearFrameBuffer(lfb);
        graphicsService->setTextDriver(text);
    }

    return 0;
}

int32_t VesaModule::finalize() {

    return 0;
}

String VesaModule::getName() {

    return "VesaDriver";
}

Util::Array<String> VesaModule::getDependencies() {

    return Util::Array<String>(0);
}
