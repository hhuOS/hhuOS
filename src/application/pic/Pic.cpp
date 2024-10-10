#include "Pic.h"

#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION

#include "lib/util/graphic/stb_image.h"
#include "lib/util/graphic/stb_image_write.h"


#define print(i) Util::System::out << i << Util::Io::PrintStream::endl << Util::Io::PrintStream::flush

using namespace Util;
using namespace Graphic;;

int32_t main(int32_t argc, char *argv[]) {

    auto argumentParser = Util::ArgumentParser();
    argumentParser.setHelpText("Test\n");
    argumentParser.addArgument("test", false, "t");

    if (!argumentParser.parse(argc, argv)) {
        Util::System::error << argumentParser.getErrorString() << Util::Io::PrintStream::endl
                            << Util::Io::PrintStream::flush;
        return -1;
    }

    if (argumentParser.hasArgument("number")) {
        auto arg = argumentParser.getArgument("number")[0];
        print("you called with number: " << arg);
    }

    auto lfbFile = Util::Io::File("/device/lfb");
    auto lfb = new LinearFrameBuffer(lfbFile);
//    auto blfb = new BufferedLinearFrameBuffer(lfb, true);
    Util::Graphic::Ansi::prepareGraphicalApplication(true);

    Util::Io::File::setAccessMode(Util::Io::STANDARD_INPUT, Util::Io::File::NON_BLOCKING);

    auto mouseFile = Util::Io::File("/device/mouse");
    auto mouseInputStream = new Util::Io::FileInputStream(mouseFile);
    mouseInputStream->setAccessMode(Util::Io::File::NON_BLOCKING);

    auto pic = new Pic(lfb, mouseInputStream);
    pic->run();
    delete pic;

    return 0;
}

Pic::Pic(Util::Graphic::LinearFrameBuffer *lfb, Util::Io::FileInputStream *mouseInputStream) {
    this->running = true;
    this->lfb = lfb;
    this->screenX = lfb->getResolutionX();
    this->screenY = lfb->getResolutionY();
    this->pitch = lfb->getPitch();
    this->mouseInputStream = mouseInputStream;
    this->mouse = new MouseData();
    this->keyDecoder = new Util::Io::KeyDecoder(new Util::Io::DeLayout());
    this->uiData = new UiData(new Util::HashMap<Util::String, GuiLayer *>());

    int width, height, channels;
    unsigned char *data = stbi_load("/user/pic/test.jpg", &width, &height, &channels, 0);
    print("Loaded image with width " << width << ", height " << height << ", and channels " << channels);
    auto *argbData = new uint32_t[width * height];
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            int i = y * width + x;
            int j = (height - 1 - y) * width + x;
            argbData[i] = (0xFF000000 |                   // Alpha: 255
                           (data[j * channels] << 16) |    // Red
                           (data[j * channels + 1] << 8) | // Green
                           data[j * channels + 2]);        // Blue
        }
    }
    auto **layers = new Layer *[7];
    auto *layer1 = new Layer(width, height, 0, 0, true, argbData);
    auto *layer2 = new Layer(width, height, 50, 50, true, argbData);
    auto *layer3 = new Layer(width, height, 100, 100, true, argbData);
    auto *layer4 = new Layer(width, height, 150, 150, true, argbData);
    auto *layer5 = new Layer(width, height, 200, 200, true, argbData);
    auto *layer6 = new Layer(width, height, 250, 250, true, argbData);
    layers[0] = layer1;
    layers[1] = layer2;
    layers[2] = layer3;
    layers[3] = layer4;
    layers[4] = layer5;
    layers[5] = layer6;
    stbi_image_free(data);

    init_gui();

    rData = new RenderData(mouse, layers, 6, 3, new RenderFlags(), uiData);

    auto renderer = new Renderer(rData, lfb);
    auto renderThread = Util::Async::Thread::createThread("renderer", renderer);
}


void Pic::run() {
    while (running) {
        this->checkMouseInput();
        this->checkKeyboardInput();
        Util::Async::Thread::sleep(Util::Time::Timestamp::ofMilliseconds(1));
    }
}

void Pic::checkMouseInput() {
    uint8_t mouseValues[4]{};
    uint32_t mouseValueIndex = 0;
    int16_t value = mouseInputStream->read();
    bool mouseChanged = false;
    while (value >= 0) {
        mouseValues[mouseValueIndex++] = value;
        if (mouseValueIndex == 4) {
            auto mouseUpdate = Util::Io::MouseDecoder::decode(mouseValues);
            auto min = [](int a, int b) { return a < b ? a : b; };
            auto max = [](int a, int b) { return a > b ? a : b; };
            mouse->X = max(0, min(screenX - 1, mouse->X + mouseUpdate.xMovement));
            mouse->Y = max(0, min(screenY - 1, mouse->Y - mouseUpdate.yMovement));
            mouse->leftButtonPressed = mouseUpdate.buttons & Util::Io::Mouse::LEFT_BUTTON;
            Util::Address<uint32_t>(mouseValues).setRange(0, 4);
            mouseValueIndex = 0;
            mouseChanged = true;
        }
        value = mouseInputStream->read();
        rData->flags->mouseChanged();
    }
    if (mouseChanged) {
        parseMouseToGui();
    }
}

void Pic::parseMouseToGui() {
    if (mouse->X < 200) { // Buttons
        for (int i = 0; i < uiData->currentGuiLayer->buttonCount; ++i) {
            auto button = uiData->currentGuiLayer->buttons[i];
            button->removeInteraction();
            if (mouse->Y >= i * 30 && mouse->Y < (i + 1) * 30) {
                if (mouse->leftButtonPressed) {
                    button->processClick(mouse->X, mouse->Y - i * 30);
                } else {
                    button->showHover(mouse->X, mouse->Y - i * 30);
                }
            }
            button->bufferChanged = true;  // TODO fix when actually changed
        }
    } else { // workArea

    }
    rData->flags->guiChanged(); // TODO fix when actually changed
}


void Pic::init_gui() {
    auto *gui_top = new GuiLayer();
    for (int i = 0; i < 15; ++i) {
        auto *button = new Button([](UiData *data) {
            data->currentGuiLayer = data->guiLayers->get("second");
        }, uiData);
        gui_top->addButton(button);
    }
    auto *gui_second = new GuiLayer();
    for (int i = 0; i < 10; ++i) {
        auto *button = new Button([](UiData *data) {
            data->currentGuiLayer = data->guiLayers->get("top");
        }, uiData);
        gui_second->addButton(button);
    }
    uiData->guiLayers->put("top", gui_top);
    uiData->guiLayers->put("second", gui_second);
    uiData->currentGuiLayer = gui_second;

}

// TODO: nur einmal pro Tastendruck ausführen (wie gedrückt halten dann?)
void Pic::checkKeyboardInput() {
    int16_t scancode = Util::System::in.read();
    while (scancode >= 0) {
        if (keyDecoder->parseScancode(scancode)) {
            auto key = keyDecoder->getCurrentKey();
            auto currentLayer = rData->layers[rData->currentLayer];
            switch (key.getScancode()) {
                case Util::Io::Key::ESC:
                    running = false;
                    break;
                case Util::Io::Key::UP:
                    currentLayer->setPosY(currentLayer->getPosY() - 10);
                    rData->flags->currentLayerChanged();
                    break;
                case Util::Io::Key::DOWN:
                    currentLayer->setPosY(currentLayer->getPosY() + 10);
                    rData->flags->currentLayerChanged();
                    break;
                case Util::Io::Key::LEFT:
                    currentLayer->setPosX(currentLayer->getPosX() - 10);
                    rData->flags->currentLayerChanged();
                    break;
                case Util::Io::Key::RIGHT:
                    currentLayer->setPosX(currentLayer->getPosX() + 10);
                    rData->flags->currentLayerChanged();
                    break;
                case Util::Io::Key::TAB:
                    rData->currentLayer = (rData->currentLayer + 1) % rData->layerCount;
                    rData->flags->layerOrderChanged();
                    break;
                case Util::Io::Key::SPACE:
                    rData->layers[rData->currentLayer]->setVisibility(
                            !rData->layers[rData->currentLayer]->getVisibility());
                    rData->flags->currentLayerChanged();
                    break;
            }
        }
        scancode = Util::System::in.read();
    }
}


