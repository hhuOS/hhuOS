#include "Pic.h"

#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION

#include "lib/util/graphic/stb_image.h"
#include "lib/util/graphic/stb_image_write.h"


#define print(i) Util::System::out << i << Util::Io::PrintStream::endl << Util::Io::PrintStream::flush

using namespace Util;
using namespace Graphic;


Layer **makeTestLayers() {
    int width, height, channels;
    unsigned char *img = stbi_load("/user/pic/test.jpg", &width, &height, &channels, 0);
    print("Loaded image with width " << width << ", height " << height << ", and channels " << channels);
    auto *argbData = new uint32_t[width * height];
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            int i = y * width + x;
            int j = (height - 1 - y) * width + x;
            argbData[i] = (0xFF000000 |                   // Alpha: 255
                           (img[j * channels] << 16) |    // Red
                           (img[j * channels + 1] << 8) | // Green
                           img[j * channels + 2]);        // Blue
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
    stbi_image_free(img);
    return layers;
}

int32_t main(int32_t argc, char *argv[]) {

//    auto argumentParser = Util::ArgumentParser();
//    argumentParser.setHelpText("Test\n");
//    argumentParser.addArgument("test", false, "t");
//
//    if (!argumentParser.parse(argc, argv)) {
//        Util::System::error << argumentParser.getErrorString() << Util::Io::PrintStream::endl
//                            << Util::Io::PrintStream::flush;
//        return -1;
//    }
//
//    if (argumentParser.hasArgument("number")) {
//        auto arg = argumentParser.getArgument("number")[0];
//        print("you called with number: " << arg);
//    }

    auto pic = new Pic();
    pic->run();
    delete pic;

    return 0;
}

Pic::Pic() {
    this->data = new DataWrapper();
//    this->renderer = new Renderer(data);

    init_gui();

    data->layers = makeTestLayers();
    data->layerCount = 6;
    data->currentLayer = 3;

    auto renderer = new Renderer(data);
    auto renderThread = Util::Async::Thread::createThread("renderer", renderer);
}


void Pic::run() {
    while (data->running) {
        this->checkMouseInput();
        this->checkKeyboardInput();
//        this->renderer->render();
        Util::Async::Thread::sleep(Util::Time::Timestamp::ofMilliseconds(1));
    }
}

void Pic::checkMouseInput() {
    uint8_t mouseValues[4]{};
    uint32_t mouseValueIndex = 0;
    int16_t value = data->mouseInputStream->read();
    bool mouseChanged = false;
    bool oldUpdated = false;
    while (value >= 0) {
        mouseValues[mouseValueIndex++] = value;
        if (mouseValueIndex == 4) {
            auto mouseUpdate = Util::Io::MouseDecoder::decode(mouseValues);
            auto min = [](int a, int b) { return a < b ? a : b; };
            auto max = [](int a, int b) { return a > b ? a : b; };
            if (!oldUpdated) {
                data->oldMouseX = data->mouseX;
                data->oldMouseY = data->mouseY;
                data->oldLeftButtonPressed = data->leftButtonPressed;
                oldUpdated = true;
            }
            data->mouseX = max(0, min(data->screenX - 1, data->mouseX + mouseUpdate.xMovement));
            data->mouseY = max(0, min(data->screenY - 1, data->mouseY - mouseUpdate.yMovement));
            data->leftButtonPressed = mouseUpdate.buttons & Util::Io::Mouse::LEFT_BUTTON;
            Util::Address<uint32_t>(mouseValues).setRange(0, 4);
            mouseValueIndex = 0;
            mouseChanged = true;
        }
        value = data->mouseInputStream->read();
        data->flags->mouseChanged();
    }
    if (mouseChanged) {
        parseMouseToGui();
    }
}

void Pic::parseMouseToGui() {
    if (data->mouseX < 200) { // Buttons
        for (int i = 0; i < data->currentGuiLayer->buttonCount; ++i) {
            auto button = data->currentGuiLayer->buttons[i];
            button->removeInteraction();
            if (data->mouseY >= i * 30 && data->mouseY < (i + 1) * 30) {
                if (data->leftButtonPressed) {
                    button->processClick(data->mouseX, data->mouseY - i * 30);
                } else {
                    button->showHover(data->mouseX, data->mouseY - i * 30);
                }
            }
            button->bufferChanged = true;  // TODO fix when actually changed
        }
    } else { // workArea

    }
    data->flags->guiChanged(); // TODO fix when actually changed
}


void Pic::init_gui() {
    auto *gui_top = new GuiLayer();
    for (int i = 0; i < 15; ++i) {
        auto *button = new Button([](DataWrapper *data) {
            data->currentGuiLayer = data->guiLayers->get("second");
        }, data);
        gui_top->addButton(button);
    }
    auto *gui_second = new GuiLayer();
    for (int i = 0; i < 10; ++i) {
        auto *button = new Button([](DataWrapper *data) {
            data->currentGuiLayer = data->guiLayers->get("top");
        }, data);
        gui_second->addButton(button);
    }
    data->guiLayers->put("top", gui_top);
    data->guiLayers->put("second", gui_second);
    data->currentGuiLayer = gui_second;
}

// TODO: nur einmal pro Tastendruck ausführen (wie gedrückt halten dann?)
void Pic::checkKeyboardInput() {
    int16_t scancode = Util::System::in.read();
    while (scancode >= 0) {
        if (data->keyDecoder->parseScancode(scancode)) {
            auto key = data->keyDecoder->getCurrentKey();
            auto currentLayer = data->layers[data->currentLayer];
            switch (key.getScancode()) {
                case Util::Io::Key::ESC:
                    data->running = false;
                    break;
                case Util::Io::Key::UP:
                    currentLayer->setPosY(currentLayer->getPosY() - 10);
                    data->flags->currentLayerChanged();
                    break;
                case Util::Io::Key::DOWN:
                    currentLayer->setPosY(currentLayer->getPosY() + 10);
                    data->flags->currentLayerChanged();
                    break;
                case Util::Io::Key::LEFT:
                    currentLayer->setPosX(currentLayer->getPosX() - 10);
                    data->flags->currentLayerChanged();
                    break;
                case Util::Io::Key::RIGHT:
                    currentLayer->setPosX(currentLayer->getPosX() + 10);
                    data->flags->currentLayerChanged();
                    break;
                case Util::Io::Key::TAB:
                    data->currentLayer = (data->currentLayer + 1) % data->layerCount;
                    data->flags->layerOrderChanged();
                    break;
                case Util::Io::Key::SPACE:
                    data->layers[data->currentLayer]->setVisibility(
                            !data->layers[data->currentLayer]->getVisibility());
                    data->flags->currentLayerChanged();
                    break;
            }
        }
        scancode = Util::System::in.read();
    }
}


