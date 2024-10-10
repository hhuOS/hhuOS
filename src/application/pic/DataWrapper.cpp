//
// Created by Rafael Reip on 04.10.24.
//

#include "DataWrapper.h"
#include "lib/util/io/file/File.h"

DataWrapper::DataWrapper() {
    // screen
    auto lfbFile = Util::Io::File("/device/lfb");
    lfb = new Util::Graphic::LinearFrameBuffer(lfbFile);
    screenX = lfb->getResolutionX(), screenY = lfb->getResolutionY(), pitch = lfb->getPitch();
    workAreaX = screenX - 200, workAreaY = screenY;
    guiX = 200, guiY = screenY;

    // input
    mouseX = -1;
    mouseY = -1;
    oldMouseX = -1;
    oldMouseY = -1;
    leftButtonPressed = false;
    oldLeftButtonPressed = false;

    // rendering
    flags = new RenderFlags();

    // layers
    layers = nullptr;
    layerCount = -1;
    currentLayer = -1;

    // gui
    guiLayers = nullptr;
    currentGuiLayer = nullptr;

    // work vars
    currentTool = NONE;
    moveX = -1;
    moveY = -1;
    rotateDeg = -1;
    scale = -1.0;
    cropLeft = -1;
    cropRight = -1;
    cropTop = -1;
    cropBottom = -1;
    penSize = -1;
    penColor = 0xFFFFFFFF;  // Setting to white as -1 might not be appropriate for color
}
