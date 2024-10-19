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
    auto **layers = new Layer *[4];
    auto *layer1 = new Layer(width, height, 50, 50, true, argbData);
    auto *layer2 = new Layer(width, height, 150, 150, true, argbData);
    auto *layer3 = new Layer(width, height, 250, 250, true, argbData);
    auto *layer4 = new Layer(500, 500, 50, 50, true);
    layers[0] = layer1;
    layers[1] = layer2;
    layers[2] = layer3;
    layers[3] = layer4;
    stbi_image_free(img);
    return layers;
}

int32_t main([[maybe_unused]] int32_t argc, [[maybe_unused]] char *argv[]) {
    auto pic = new Pic();
    pic->run();
    delete pic;

    return 0;
}

Pic::Pic() {
    this->data = new DataWrapper();
    this->renderer = new Renderer(data);

    data->layers = makeTestLayers();
    data->layerCount = 4;
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
//        Util::Async::Thread::sleep(Util::Time::Timestamp::ofMilliseconds(1) );
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
    data->xMovement = 0, data->yMovement = 0;
    while (value >= 0) {
        mouseValues[mouseValueIndex++] = value;
        if (mouseValueIndex == 4) {
            auto mouseUpdate = Util::Io::MouseDecoder::decode(mouseValues);
            data->oldMouseX = data->mouseX;
            data->oldMouseY = data->mouseY;
            data->oldLeftButtonPressed = data->leftButtonPressed;
            data->xMovement += mouseUpdate.xMovement;
            data->yMovement -= mouseUpdate.yMovement;
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

void Pic::parseMouse(bool clicked) {
    int buttonIndex = data->mouseY / 30;
    int buttonIndexBottom = buttonIndex - 19 + data->currentGuiLayerBottom->buttonCount;
    int lastInteractedBottom = data->lastInteractedButton - 19 + data->currentGuiLayerBottom->buttonCount;

    if (buttonIndex != data->lastInteractedButton || data->mouseX >= 200) {
        if (data->lastInteractedButton != -1) {
            if (data->lastInteractedButton < data->currentGuiLayer->buttonCount)
                data->currentGuiLayer->buttons[data->lastInteractedButton]->removeInteraction();
            else if (lastInteractedBottom >= 0 && lastInteractedBottom < data->currentGuiLayerBottom->buttonCount)
                data->currentGuiLayerBottom->buttons[lastInteractedBottom]->removeInteraction();
            else if (data->lastInteractedButton == 19)
                data->textButton->removeInteraction();
        }
        data->lastInteractedButton = buttonIndex;
    }

    if (data->mouseX < 200 && !(data->leftButtonPressed && !data->clickStartedOnGui)) {
        Button *button = nullptr;
        if (buttonIndex < data->currentGuiLayer->buttonCount)
            button = data->currentGuiLayer->buttons[buttonIndex];
        if (buttonIndexBottom >= 0 && buttonIndexBottom < data->currentGuiLayerBottom->buttonCount)
            button = data->currentGuiLayerBottom->buttons[buttonIndexBottom];
        if (buttonIndex == 19)
            button = data->textButton;
        if (button != nullptr) {
            if (data->leftButtonPressed)
                button->showClick(data->mouseX, data->mouseY - buttonIndex * 30);
            else
                button->showHover(data->mouseX, data->mouseY - buttonIndex * 30);
            if (clicked && data->clickStartedOnGui)
                button->processClick(data->mouseX, data->mouseY - buttonIndex * 30);
        }
    } else if (data->mouseX >= 200 && data->leftButtonPressed && !data->clickStartedOnGui) {
        if (data->currentTool == Tool::MOVE) {
            data->moveX += data->xMovement;
            data->moveY += data->yMovement;
            data->flags->guiButtonChanged();
            data->flags->overlayChanged();
            data->currentGuiLayerBottom->appear();
        } else if (data->currentTool == Tool::SCALE) {
            if (data->toolCorner == ToolCorner::TOP_LEFT)
                data->scale += (-data->xMovement - data->yMovement) * 0.005;
            else if (data->toolCorner == ToolCorner::TOP_RIGHT)
                data->scale += (data->xMovement - data->yMovement) * 0.005;
            else if (data->toolCorner == ToolCorner::BOTTOM_LEFT)
                data->scale += (-data->xMovement + data->yMovement) * 0.005;
            else if (data->toolCorner == ToolCorner::BOTTOM_RIGHT)
                data->scale += (data->xMovement + data->yMovement) * 0.005;
            if (data->scale < 0.01) data->scale = 0.01;
            data->flags->guiButtonChanged();
            data->flags->overlayChanged();
            data->currentGuiLayerBottom->appear();
        } else if (data->currentTool == Tool::CROP) {
            if (data->toolCorner == ToolCorner::TOP_LEFT)
                data->cropLeft += data->xMovement, data->cropTop += data->yMovement;
            else if (data->toolCorner == ToolCorner::TOP_RIGHT)
                data->cropRight -= data->xMovement, data->cropTop += data->yMovement;
            else if (data->toolCorner == ToolCorner::BOTTOM_LEFT)
                data->cropLeft += data->xMovement, data->cropBottom -= data->yMovement;
            else if (data->toolCorner == ToolCorner::BOTTOM_RIGHT)
                data->cropRight -= data->xMovement, data->cropBottom -= data->yMovement;
            data->flags->guiButtonChanged();
            data->flags->overlayChanged();
            data->currentGuiLayerBottom->appear();
        } else if (data->currentTool == Tool::COLOR) {
            Layer *l = data->layers[data->currentLayer];
            int relX = data->mouseX - 200 - l->posX;
            int relY = data->mouseY - l->posY;
            if (relX < 0 || relX >= l->width || relY < 0 || relY >= l->height) return;
            uint32_t c = l->getPixel(relX, relY);
            data->colorA = (c >> 24) & 0xFF;
            data->colorR = (c >> 16) & 0xFF;
            data->colorG = (c >> 8) & 0xFF;
            data->colorB = c & 0xFF;
            data->currentGuiLayer->appear();
            data->flags->guiButtonChanged();
            data->flags->overlayChanged();
            data->currentGuiLayerBottom->appear();
        } else if (data->currentTool == Tool::ROTATE) {
            data->rotateDeg += data->xMovement;
            if (data->rotateDeg > 180) data->rotateDeg -= 360;
            if (data->rotateDeg < -180) data->rotateDeg += 360;
            data->flags->guiButtonChanged();
            data->flags->overlayChanged();
            data->currentGuiLayerBottom->appear();
        } else if ((data->currentTool == Tool::PEN || data->currentTool == Tool::ERASER) && data->mouseClicks->size() > 0) {
            // ✅ TODO bug: sometimes skips some points?
            // ❎ TODO fix: if alpha!=FF: circles from drawing blend onto themselves -> should only blend once
            Layer *l = data->layers[data->currentLayer];
            auto click = data->mouseClicks->peek();
            int lastX = click.first - l->posX - 200;
            int lastY = click.second - l->posY;
            while (data->mouseClicks->size() > 0) {
                click = data->mouseClicks->poll();
                int relX = click.first - l->posX - 200;
                int relY = click.second - l->posY;
                if ((relX < 0 || relX >= l->width || relY < 0 || relY >= l->height) &&
                    (lastX < 0 || lastX >= l->width || lastY < 0 || lastY >= l->height)) {
                    continue; // both points are outside of the layer
                }
                uint32_t penColor = (data->colorA << 24) | (data->colorR << 16) | (data->colorG << 8) | data->colorB;
                l->drawLine(lastX, lastY, relX, relY, data->currentTool == Tool::PEN ? penColor : 0x00000000, data->penSize);
                lastX = relX, lastY = relY;
            }
            // put last click back for next iteration
            data->mouseClicks->offer(Util::Pair<int, int>(data->mouseX, data->mouseY));
            data->flags->currentLayerChanged();
        }
    }

    data->debugString = String::format(
            "Mouse: %d %d, currentTool: %d, queueLength: %d, clickStartedOnGui: %d",
            data->mouseX, data->mouseY, data->currentTool, data->mouseClicks->size(), data->clickStartedOnGui
    ).operator const char *();
    data->flags->overlayChanged();
}

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
            if (data->captureInput) { // TODO: kaputt, wenn man tab drückt (ist vll riesen char?? :D)
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
                        data->flags->overlayChanged();
                        break;
                    case Util::Io::Key::DOWN:
                        currentLayer->posY = currentLayer->posY + 10;
                        data->flags->currentLayerChanged();
                        data->flags->overlayChanged();
                        break;
                    case Util::Io::Key::LEFT:
                        currentLayer->posX = currentLayer->posX - 10;
                        data->flags->currentLayerChanged();
                        data->flags->overlayChanged();
                        break;
                    case Util::Io::Key::RIGHT:
                        currentLayer->posX = currentLayer->posX + 10;
                        data->flags->currentLayerChanged();
                        data->flags->overlayChanged();
                        break;
                    case Util::Io::Key::TAB:
                        data->currentLayer = (data->currentLayer + 1) % data->layerCount;
                        data->flags->layerOrderChanged();
                        data->flags->overlayChanged();
                        break;
                    case Util::Io::Key::SPACE:
                        data->layers[data->currentLayer]->isVisible = !data->layers[data->currentLayer]->isVisible;
                        data->flags->currentLayerChanged();
                        data->flags->overlayChanged();
                        break;
                    default:
                        break;
                }
            }
        }
        scancode = Util::System::in.read();
    }
}

void swapTool(DataWrapper *data, Tool tool) {
    if (data->currentTool == tool) {
        data->currentTool = Tool::NOTHING;
        data->currentGuiLayerBottom = data->guiLayers->get("empty");
    } else {
        data->currentTool = tool;
        switch (tool) {
            case Tool::MOVE:
                data->currentGuiLayerBottom = data->guiLayers->get("bottom_move");
                break;
            case Tool::ROTATE:
                data->currentGuiLayerBottom = data->guiLayers->get("bottom_rotate");
                break;
            case Tool::SCALE:
                data->currentGuiLayerBottom = data->guiLayers->get("bottom_scale");
                break;
            case Tool::CROP:
                data->currentGuiLayerBottom = data->guiLayers->get("bottom_crop");
                break;
            case Tool::PEN:
            case Tool::ERASER:
                data->currentGuiLayerBottom = data->guiLayers->get("bottom_pen");
                break;
            case Tool::COLOR:
                data->currentGuiLayerBottom = data->guiLayers->get("bottom_color");
                break;
            case Tool::NOTHING:
                data->currentGuiLayerBottom = data->guiLayers->get("empty");
                break;
        }
    }
    data->currentGuiLayerBottom->appear();
    data->currentGuiLayer->appear();
}

void changeGuiLayerTo(DataWrapper *data, const char *layer) {
    data->currentGuiLayer = data->guiLayers->get(layer);
    data->currentGuiLayer->appear();
    swapTool(data, Tool::NOTHING);
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
                                 ->setInfo("Back")
                                 ->setMethodButton([](DataWrapper *data) {
                                     changeGuiLayerTo(data, "main");
                                 })
                                 ->setRenderFlagMethod(&RenderFlags::guiLayerChanged)
    );
    gui_tools->addButton((new Button(data))
                                 ->setInfo("Move")
                                 ->setMethodButton([](DataWrapper *data) {
                                     data->moveX = data->layers[data->currentLayer]->posX;
                                     data->moveY = data->layers[data->currentLayer]->posY;
                                     swapTool(data, Tool::MOVE);
                                 })
                                 ->changeGreenIfTool(Tool::MOVE)
                                 ->setRenderFlagMethod(&RenderFlags::guiLayerChanged)
    );
    gui_tools->addButton((new Button(data))
                                 ->setInfo("Rotate")
                                 ->setMethodButton([](DataWrapper *data) {
                                     data->rotateDeg = 0;
                                     swapTool(data, Tool::ROTATE);
                                 })
                                 ->changeGreenIfTool(Tool::ROTATE)
                                 ->setRenderFlagMethod(&RenderFlags::guiLayerChanged)
    );
    gui_tools->addButton((new Button(data))
                                 ->setInfo("Scale")
                                 ->setMethodButton([](DataWrapper *data) {
                                     data->scale = 1.0;
                                     swapTool(data, Tool::SCALE);
                                 })
                                 ->changeGreenIfTool(Tool::SCALE)
                                 ->setRenderFlagMethod(&RenderFlags::guiLayerChanged)
    );
    gui_tools->addButton((new Button(data))
                                 ->setInfo("Crop")
                                 ->setMethodButton([](DataWrapper *data) {
                                     data->cropLeft = 0;
                                     data->cropRight = 0;
                                     data->cropTop = 0;
                                     data->cropBottom = 0;
                                     swapTool(data, Tool::CROP);
                                 })
                                 ->changeGreenIfTool(Tool::CROP)
                                 ->setRenderFlagMethod(&RenderFlags::guiLayerChanged)
    );
    gui_tools->addButton((new Button(data))
                                 ->setInfo("Pen")
                                 ->setMethodButton([](DataWrapper *data) {
                                     data->penSize = 10;
                                     swapTool(data, Tool::PEN);
                                 })
                                 ->changeGreenIfTool(Tool::PEN)
                                 ->setRenderFlagMethod(&RenderFlags::guiLayerChanged)
    );
    gui_tools->addButton((new Button(data))
                                 ->setInfo("Eraser")
                                 ->setMethodButton([](DataWrapper *data) {
                                     data->penSize = 10;
                                     swapTool(data, Tool::ERASER);
                                 })
                                 ->changeGreenIfTool(Tool::ERASER)
                                 ->setRenderFlagMethod(&RenderFlags::guiLayerChanged)
    );
    gui_tools->addButton((new Button(data))
                                 ->setInfo("Color")
                                 ->setMethodButton([](DataWrapper *data) {
                                     swapTool(data, Tool::COLOR);
                                 })
                                 ->changeGreenIfTool(Tool::COLOR)
                                 ->setColor(&data->colorA, &data->colorR, &data->colorG, &data->colorB)
                                 ->setRenderFlagMethod(&RenderFlags::guiLayerChanged)
    );


    auto *gui_layers = new GuiLayer();
    gui_layers->addButton((new Button(data))
                                  ->setInfo("back")
                                  ->setMethodButton([](DataWrapper *data) {
                                      changeGuiLayerTo(data, "main");
                                  })
                                  ->setRenderFlagMethod(&RenderFlags::guiLayerChanged)
    );
    gui_layers->addButton((new Button(data))
                                  ->setInfo("currPosX")
                                  ->setIntValueButton(&data->layers[data->currentLayer]->posX)
                                  ->setRenderFlagMethod(&RenderFlags::currentLayerChanged)
    );
    gui_layers->addButton((new Button(data))
                                  ->setInfo("currPosY")
                                  ->setIntValueButton(&data->layers[data->currentLayer]->posY)
                                  ->setRenderFlagMethod(&RenderFlags::currentLayerChanged)
    );

    auto gui_bottom_move = new GuiLayer();
    gui_bottom_move->addButton((new Button(data))
                                       ->setInfo("moveX")
                                       ->setIntValueButton(&data->moveX)
                                       ->setRenderFlagMethod(&RenderFlags::guiButtonChanged)
    );
    gui_bottom_move->addButton((new Button(data))
                                       ->setInfo("moveY")
                                       ->setIntValueButton(&data->moveY)
                                       ->setRenderFlagMethod(&RenderFlags::guiButtonChanged)
    );
    gui_bottom_move->addButton((new Button(data))
                                       ->setInfo("MOVE")
                                       ->setConfirmButton([](DataWrapper *data) {
                                           data->moveX = data->layers[data->currentLayer]->posX;
                                           data->moveY = data->layers[data->currentLayer]->posY;
                                           data->currentGuiLayerBottom->appear();
                                       }, [](DataWrapper *data) {
                                           data->layers[data->currentLayer]->posX = data->moveX;
                                           data->layers[data->currentLayer]->posY = data->moveY;
                                       })
                                       ->setRenderFlagMethod(&RenderFlags::currentLayerChanged)
    );

    auto gui_bottom_scale = new GuiLayer();
    gui_bottom_scale->addButton((new Button(data))
                                        ->setInfo("scale")
                                        ->setDoubleValueButton(&data->scale, 0.1, 10.0)
                                        ->setRenderFlagMethod(&RenderFlags::guiButtonChanged)
    );
    gui_bottom_scale->addButton((new Button(data))
                                        ->setInfo("switch Corner")
                                        ->setMethodButton([](DataWrapper *data) {
                                            if (data->toolCorner == ToolCorner::TOP_LEFT)
                                                data->toolCorner = ToolCorner::TOP_RIGHT;
                                            else if (data->toolCorner == ToolCorner::TOP_RIGHT)
                                                data->toolCorner = ToolCorner::BOTTOM_RIGHT;
                                            else if (data->toolCorner == ToolCorner::BOTTOM_RIGHT)
                                                data->toolCorner = ToolCorner::BOTTOM_LEFT;
                                            else if (data->toolCorner == ToolCorner::BOTTOM_LEFT)
                                                data->toolCorner = ToolCorner::TOP_LEFT;
                                        })
                                        ->setRenderFlagMethod(&RenderFlags::overlayChanged)
    );
    gui_bottom_scale->addButton((new Button(data))
                                        ->setInfo("SCALE")
                                        ->setConfirmButton([](DataWrapper *data) {
                                            data->scale = 1.0;
                                            data->currentGuiLayerBottom->appear();
                                        }, [](DataWrapper *data) {
                                            data->layers[data->currentLayer]->scale(data->scale, data->toolCorner);
                                            data->scale = 1.0;
                                            data->currentGuiLayerBottom->appear();
                                        })
                                        ->setRenderFlagMethod(&RenderFlags::currentLayerChanged)
    );

    auto gui_bottom_crop = new GuiLayer();
    gui_bottom_crop->addButton((new Button(data))
                                       ->setInfo("cropLeft")
                                       ->setIntValueButton(&data->cropLeft)
                                       ->setRenderFlagMethod(&RenderFlags::guiButtonChanged)
    );
    gui_bottom_crop->addButton((new Button(data))
                                       ->setInfo("cropRight")
                                       ->setIntValueButton(&data->cropRight)
                                       ->setRenderFlagMethod(&RenderFlags::guiButtonChanged)
    );
    gui_bottom_crop->addButton((new Button(data))
                                       ->setInfo("cropTop")
                                       ->setIntValueButton(&data->cropTop)
                                       ->setRenderFlagMethod(&RenderFlags::guiButtonChanged)
    );
    gui_bottom_crop->addButton((new Button(data))
                                       ->setInfo("cropBottom")
                                       ->setIntValueButton(&data->cropBottom)
                                       ->setRenderFlagMethod(&RenderFlags::guiButtonChanged)
    );
    gui_bottom_crop->addButton((new Button(data))
                                       ->setInfo("switch Corner")
                                       ->setMethodButton([](DataWrapper *data) {
                                           if (data->toolCorner == ToolCorner::TOP_LEFT) data->toolCorner = ToolCorner::TOP_RIGHT;
                                           else if (data->toolCorner == ToolCorner::TOP_RIGHT) data->toolCorner = ToolCorner::BOTTOM_RIGHT;
                                           else if (data->toolCorner == ToolCorner::BOTTOM_RIGHT)
                                               data->toolCorner = ToolCorner::BOTTOM_LEFT;
                                           else if (data->toolCorner == ToolCorner::BOTTOM_LEFT) data->toolCorner = ToolCorner::TOP_LEFT;
                                       })
                                       ->setRenderFlagMethod(&RenderFlags::guiButtonChanged)
    );
    gui_bottom_crop->addButton((new Button(data))
                                       ->setInfo("CROP")
                                       ->setConfirmButton([](DataWrapper *data) {
                                           data->cropLeft = 0, data->cropRight = 0, data->cropTop = 0, data->cropBottom = 0;
                                           data->currentGuiLayerBottom->appear();
                                       }, [](DataWrapper *data) {
                                           data->layers[data->currentLayer]->crop(data->cropLeft, data->cropRight, data->cropTop,
                                                                                  data->cropBottom);
                                           data->cropLeft = 0, data->cropRight = 0, data->cropTop = 0, data->cropBottom = 0;
                                           data->currentGuiLayerBottom->appear();
                                       })
                                       ->setRenderFlagMethod(&RenderFlags::currentLayerChanged)
    );

    auto gui_bottom_color = new GuiLayer();
    gui_bottom_color->addButton((new Button(data))
                                        ->setInfo("Alpha")
                                        ->setIntValueButton(&data->colorA, 0, 255)
                                        ->setRenderFlagMethod(&RenderFlags::guiButtonChanged)
                                        ->setAppearTopOnChange(true)
    );
    gui_bottom_color->addButton((new Button(data))
                                        ->setInfo("Red")
                                        ->setIntValueButton(&data->colorR, 0, 255)
                                        ->setRenderFlagMethod(&RenderFlags::guiButtonChanged)
                                        ->setAppearTopOnChange(true)
    );
    gui_bottom_color->addButton((new Button(data))
                                        ->setInfo("Green")
                                        ->setIntValueButton(&data->colorG, 0, 255)
                                        ->setRenderFlagMethod(&RenderFlags::guiButtonChanged)
                                        ->setAppearTopOnChange(true)
    );
    gui_bottom_color->addButton((new Button(data))
                                        ->setInfo("Blue")
                                        ->setIntValueButton(&data->colorB, 0, 255)
                                        ->setRenderFlagMethod(&RenderFlags::guiButtonChanged)
                                        ->setAppearTopOnChange(true)
    );

    auto gui_bottom_rotate = new GuiLayer();
    gui_bottom_rotate->addButton((new Button(data))
                                         ->setInfo("rotate")
                                         ->setIntValueButton(&data->rotateDeg, -180, 180)
                                         ->setRenderFlagMethod(&RenderFlags::guiButtonChanged)
    );
    gui_bottom_rotate->addButton((new Button(data))
                                         ->setInfo("ROTATE")
                                         ->setConfirmButton([](DataWrapper *data) {
                                             data->rotateDeg = 0;
                                             data->currentGuiLayerBottom->appear();
                                         }, [](DataWrapper *data) {
                                             data->layers[data->currentLayer]->rotate(data->rotateDeg);
                                             data->rotateDeg = 0;
                                             data->currentGuiLayerBottom->appear();
                                         })
                                         ->setRenderFlagMethod(&RenderFlags::currentLayerChanged)
    );

    auto gui_bottom_pen = new GuiLayer();
    gui_bottom_pen->addButton((new Button(data))
                                      ->setInfo("Pen Size")
                                      ->setIntValueButton(&data->penSize, 1, 100)
                                      ->setRenderFlagMethod(&RenderFlags::guiButtonChanged)
    );

    auto gui_empty = new GuiLayer();

    data->textButton = (new Button(data))
            ->setInfo("input: ")
            ->setInputButton(data->currentInput, &data->captureInput)
            ->setRenderFlagMethod(&RenderFlags::guiButtonChanged);

    data->guiLayers->put("main", gui_main);
    data->guiLayers->put("file", gui_file);
    data->guiLayers->put("tools", gui_tools);
    data->guiLayers->put("layers", gui_layers);
    data->guiLayers->put("empty", gui_empty);
    data->guiLayers->put("bottom_move", gui_bottom_move);
    data->guiLayers->put("bottom_rotate", gui_bottom_rotate);
    data->guiLayers->put("bottom_scale", gui_bottom_scale);
    data->guiLayers->put("bottom_crop", gui_bottom_crop);
    data->guiLayers->put("bottom_pen", gui_bottom_pen);
    data->guiLayers->put("bottom_color", gui_bottom_color);
    data->currentGuiLayer = gui_main;
    data->currentGuiLayerBottom = gui_empty;
}
