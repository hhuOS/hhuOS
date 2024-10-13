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
    this->renderer = new Renderer(data);

    data->layers = makeTestLayers();
    data->layerCount = 6;
    data->currentLayer = 3;

    init_gui();

//    auto renderer = new Renderer(data);
//    auto renderThread = Util::Async::Thread::createThread("renderer", renderer);
}


void Pic::run() {
    while (data->running) {
        this->checkMouseInput();
        this->checkKeyboardInput();
        this->renderer->render();
        Util::Async::Thread::sleep(Util::Time::Timestamp::ofMilliseconds(1));
    }
}

void Pic::checkMouseInput() {
    auto min = [](int a, int b) { return a < b ? a : b; };
    auto max = [](int a, int b) { return a > b ? a : b; };
    uint8_t mouseValues[4]{};
    uint32_t mouseValueIndex = 0;
    int16_t value = data->mouseInputStream->read();
    bool mouseChanged = false;
    bool clicked = false;
    while (value >= 0) {
        mouseValues[mouseValueIndex++] = value;
        if (mouseValueIndex == 4) {
            auto mouseUpdate = Util::Io::MouseDecoder::decode(mouseValues);
            data->oldMouseX = data->mouseX;
            data->oldMouseY = data->mouseY;
            data->oldLeftButtonPressed = data->leftButtonPressed;
            data->mouseX = max(0, min(data->screenX - 1, data->mouseX + mouseUpdate.xMovement));
            data->mouseY = max(0, min(data->screenY - 1, data->mouseY - mouseUpdate.yMovement));
            data->leftButtonPressed = mouseUpdate.buttons & Util::Io::Mouse::LEFT_BUTTON;
            Util::Address<uint32_t>(mouseValues).setRange(0, 4);
            mouseValueIndex = 0;
            mouseChanged = true;
            if (data->leftButtonPressed && !data->oldLeftButtonPressed) { // if newly clicked
                data->mouseClicks->clear();
                data->clickStartedOnGui = data->mouseX < 200;
            }
            if (!data->leftButtonPressed && data->oldLeftButtonPressed) { // if newly released
                clicked = true;
            }
            if (data->leftButtonPressed) {
                data->mouseClicks->offer(Util::Pair<int, int>(data->mouseX, data->mouseY));
            }
        }
        value = data->mouseInputStream->read();
        data->flags->mouseChanged();
    }
    if (mouseChanged) {
        parseMouse(clicked);
    }
}

// TODO guiButtonChanged muss theoretisch nicht immer, wenn button selbst sich gar net ändert
void Pic::parseMouse(bool clicked) {
    if (data->mouseX < 200 && !(data->leftButtonPressed && !data->clickStartedOnGui)) {
        int buttonIndex = data->mouseY / 30;
        if (buttonIndex != data->lastInteractedButton) {
            if (data->lastInteractedButton != -1 && data->lastInteractedButton < data->currentGuiLayer->buttonCount) {
                data->currentGuiLayer->buttons[data->lastInteractedButton]->removeInteraction();
            } else if (data->lastInteractedButton != -1 && data->lastInteractedButton == 19) {
                data->textButton->removeInteraction();
            }
            data->lastInteractedButton = buttonIndex;
        }
        if (buttonIndex < data->currentGuiLayer->buttonCount) { // top buttons
            auto button = data->currentGuiLayer->buttons[buttonIndex];
            if (data->leftButtonPressed)
                button->showClick(data->mouseX, data->mouseY - buttonIndex * 30);
            else
                button->showHover(data->mouseX, data->mouseY - buttonIndex * 30);
            if (clicked)
                button->processClick(data->mouseX, data->mouseY - buttonIndex * 30);
        }
        if (buttonIndex == 19) { // text input button
            if (data->leftButtonPressed)
                data->textButton->showClick(data->mouseX, data->mouseY - buttonIndex * 30);
            else
                data->textButton->showHover(data->mouseX, data->mouseY - buttonIndex * 30);
            if (clicked)
                data->textButton->processClick(data->mouseX, data->mouseY - buttonIndex * 30);
        }
    } else {
        if (data->lastInteractedButton != -1 && data->lastInteractedButton < data->currentGuiLayer->buttonCount) {
            data->currentGuiLayer->buttons[data->lastInteractedButton]->removeInteraction();
            data->lastInteractedButton = -1;
        }
        if (data->lastInteractedButton == 19) {
            data->textButton->removeInteraction();
            data->lastInteractedButton = -1;
        }
        // TODO do stuff in workArea
    }
    data->debugString = String::format("Mouse: %d %d, LastButton: %d", data->mouseX, data->mouseY,
                                       data->lastInteractedButton).operator const char *();
    data->flags->overlayChanged();

}

// TODO: nur einmal pro Tastendruck ausführen (wie gedrückt halten dann?)
void Pic::checkKeyboardInput() {
    int16_t scancode = Util::System::in.read();
    while (scancode >= 0) {
        if (scancode == data->lastScancode) {
            scancode = Util::System::in.read();
            continue;
        }
        data->lastScancode = scancode;
        if (data->keyDecoder->parseScancode(scancode)) {
            auto key = data->keyDecoder->getCurrentKey();
            if (!key.isPressed()) {
                scancode = Util::System::in.read();
                continue;
            }
            auto currentLayer = data->layers[data->currentLayer];
            if (data->captureInput) {
                if (key.getScancode() == Util::Io::Key::BACKSPACE) {
                    *data->currentInput = data->currentInput->substring(0, data->currentInput->length() - 1);
                    data->textButton->render();
                } else {
                    char c = key.getAscii();
                    if (c != 0) {
                        *data->currentInput += c;
                        data->textButton->render();
                    }
                }
            } else {
                switch (scancode) {
                    case Util::Io::Key::ESC:
                        data->running = false;
                        break;
                    case Util::Io::Key::UP:
                        currentLayer->posY = currentLayer->posY - 10;
                        data->flags->currentLayerChanged();
                        break;
                    case Util::Io::Key::DOWN:
                        currentLayer->posY = currentLayer->posY + 10;
                        data->flags->currentLayerChanged();
                        break;
                    case Util::Io::Key::LEFT:
                        currentLayer->posX = currentLayer->posX - 10;
                        data->flags->currentLayerChanged();
                        break;
                    case Util::Io::Key::RIGHT:
                        currentLayer->posX = currentLayer->posX + 10;
                        data->flags->currentLayerChanged();
                        break;
                    case Util::Io::Key::TAB:
                        data->currentLayer = (data->currentLayer + 1) % data->layerCount;
                        data->flags->layerOrderChanged();
                        break;
                    case Util::Io::Key::SPACE:
                        data->layers[data->currentLayer]->isVisible = !data->layers[data->currentLayer]->isVisible;
                        data->flags->currentLayerChanged();
                        break;
                }
            }
        }
        scancode = Util::System::in.read();
    }
}

void changeGuiLayerTo(DataWrapper *data, const char *layer) {
    data->currentGuiLayer = data->guiLayers->get(layer);
    data->currentGuiLayer->appear();
}

void Pic::init_gui() {
    auto *gui_main = new GuiLayer();
    gui_main->addButton((new Button(data))
                                ->setInfo("quit")
                                ->setMethodButton([](DataWrapper *data) {
                                    data->running = false;
                                })
                                ->setRenderFlagMethod(&RenderFlags::guiLayerChanged)
    );
    gui_main->addButton((new Button(data))
                                ->setInfo("file")
                                ->setMethodButton([](DataWrapper *data) {
                                    changeGuiLayerTo(data, "file");
                                })
                                ->setRenderFlagMethod(&RenderFlags::guiLayerChanged)
    );
    gui_main->addButton((new Button(data))
                                ->setInfo("tools")
                                ->setMethodButton([](DataWrapper *data) {
                                    changeGuiLayerTo(data, "tools");
                                })
                                ->setRenderFlagMethod(&RenderFlags::guiLayerChanged)
    );
    gui_main->addButton((new Button(data))
                                ->setInfo("layers")
                                ->setMethodButton([](DataWrapper *data) {
                                    changeGuiLayerTo(data, "layers");
                                })
                                ->setRenderFlagMethod(&RenderFlags::guiLayerChanged)
    );

    auto *gui_file = new GuiLayer();
    gui_file->addButton((new Button(data))
                                ->setInfo("back")
                                ->setMethodButton([](DataWrapper *data) {
                                    changeGuiLayerTo(data, "main");
                                })
                                ->setRenderFlagMethod(&RenderFlags::guiLayerChanged)
    );
    gui_file->addButton((new Button(data))
                                ->setInfo("load Project")
                                ->setMethodButton([](DataWrapper *data) {
                                    // TODO
                                })
    );
    gui_file->addButton((new Button(data))
                                ->setInfo("save Project")
                                ->setMethodButton([](DataWrapper *data) {
                                    // TODO
                                })
    );
    gui_file->addButton((new Button(data))
                                ->setInfo("import Image")
                                ->setMethodButton([](DataWrapper *data) {
                                    // TODO
                                }));
    gui_file->addButton((new Button(data))
                                ->setInfo("export Image as PNG")
                                ->setMethodButton([](DataWrapper *data) {
                                    // TODO
                                })
    );
    gui_file->addButton((new Button(data))
                                ->setInfo("export Image as JPEG")
                                ->setMethodButton([](DataWrapper *data) {
                                    // TODO
                                })
    );
    gui_file->addButton((new Button(data))
                                ->setInfo("undo")
                                ->setMethodButton([](DataWrapper *data) {
                                    // TODO
                                })
    );
    gui_file->addButton((new Button(data))
                                ->setInfo("redo")
                                ->setMethodButton([](DataWrapper *data) {
                                    // TODO
                                })
    );

    auto *gui_tools = new GuiLayer();
    gui_tools->addButton((new Button(data))
                                 ->setInfo("back")
                                 ->setMethodButton([](DataWrapper *data) {
                                     changeGuiLayerTo(data, "main");
                                 })
                                 ->setRenderFlagMethod(&RenderFlags::guiLayerChanged)
    );
    gui_tools->addButton((new Button(data))
                                 ->setInfo("currPosX")
                                 ->setIntValueButton(&data->layers[data->currentLayer]->posX)
                                 ->setRenderFlagMethod(&RenderFlags::currentLayerChanged)
    );
    gui_tools->addButton((new Button(data))
                                 ->setInfo("currPosY")
                                 ->setIntValueButton(&data->layers[data->currentLayer]->posY)
                                 ->setRenderFlagMethod(&RenderFlags::currentLayerChanged)
    );

    auto *gui_layers = new GuiLayer();
    gui_layers->addButton((new Button(data))
                                  ->setInfo("back")
                                  ->setMethodButton([](DataWrapper *data) {
                                      changeGuiLayerTo(data, "main");
                                  })
                                  ->setRenderFlagMethod(&RenderFlags::guiLayerChanged)
    );

    data->textButton = (new Button(data))
            ->setInfo("input: ")
            ->setInputButton(data->currentInput, &data->captureInput)
            ->setRenderFlagMethod(&RenderFlags::guiButtonChanged);

    data->guiLayers->put("main", gui_main);
    data->guiLayers->put("file", gui_file);
    data->guiLayers->put("tools", gui_tools);
    data->guiLayers->put("layers", gui_layers);
    data->currentGuiLayer = gui_main;
}


