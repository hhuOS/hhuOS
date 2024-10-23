//
// Created by Rafael Reip on 04.10.24.
//

#include "DataWrapper.h"

DataWrapper::DataWrapper() {
    // screen
    auto lfbFile = Util::Io::File("/device/lfb");
    lfb = new Util::Graphic::LinearFrameBuffer(lfbFile);
    screenX = lfb->getResolutionX(), screenY = lfb->getResolutionY(), pitch = lfb->getPitch(), screenAll =
            screenX * screenY;
    workAreaX = screenX - 200, workAreaY = screenY, workAreaAll = workAreaX * workAreaY;
    guiX = 200, guiY = screenY, guiAll = guiX * guiY;
    Util::Graphic::Ansi::prepareGraphicalApplication(true);

    // input
    auto mouseFile = Util::Io::File("/device/mouse");
    mouseInputStream = new Util::Io::FileInputStream(mouseFile);
    mouseInputStream->setAccessMode(Util::Io::File::NON_BLOCKING);
    Util::Io::File::setAccessMode(Util::Io::STANDARD_INPUT, Util::Io::File::NON_BLOCKING);
    keyDecoder = new Util::Io::KeyDecoder(new Util::Io::DeLayout());
    xMovement = 0, yMovement = 0;
    mouseX = 0, mouseY = 0, oldMouseX = 0, oldMouseY = 0;
    leftButtonPressed = false, oldLeftButtonPressed = false;
    mouseClicks = new Util::ArrayBlockingQueue<Util::Pair<int, int>>(500); // should be more than enough :D
    clickStartedOnGui = true;
    lastInteractedButton = -1;
    currentInput = new Util::String();
    captureInput = false;
    lastScancode = 0;

    // rendering
    flags = new RenderFlags();
    mHandler = new MessageHandler(workAreaX, workAreaY);

    // layers
    layers = nullptr;

    // gui
    guiLayers = new Util::HashMap<Util::String, GuiLayer *>();
    currentGuiLayer = nullptr;
    currentGuiLayerBottom = nullptr;
    textButton = nullptr;

    // overlay
    debugString = nullptr;

    // work vars
    running = true;
    currentTool = Tool::NOTHING;
    moveX = -1;
    moveY = -1;
    rotateDeg = -1;
    scale = -1.0;
    toolCorner = ToolCorner::BOTTOM_RIGHT;
    cropLeft = -1;
    cropRight = -1;
    cropTop = -1;
    cropBottom = -1;
    penSize = -1;
    colorA = 128, colorR = 0, colorG = 255, colorB = 0;
    combineFirst = 0;
    combineSecond = 1;
}
