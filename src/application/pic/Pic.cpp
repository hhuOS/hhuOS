#include "Pic.h"

using namespace Util;
using namespace Graphic;


int32_t main([[maybe_unused]] int32_t argc, [[maybe_unused]] char *argv[]) {
    auto pic = new Pic();
    pic->run();
    delete pic;

    return 0;
}

Pic::Pic() {
    this->data = new DataWrapper();
    this->renderer = new Renderer(data);

    data->layers = new Layers(data->mHandler);
    data->layers->addPicture("/user/pic/test.jpg", 0, 0);
    data->layers->addPicture("/user/pic/test.jpg", 100, 100);
    data->layers->addPicture("/user/pic/test.jpg", 400, 100);
    data->layers->addEmpty(50, 50, data->workAreaX - 100, data->workAreaY - 100);

    init_gui();

//    auto renderer = new Renderer(data);
//    auto renderThread = Util::Async::Thread::createThread("renderer", renderer);
}


void Pic::run() {
    while (data->running) {
        checkMouseInput();
        checkKeyboardInput();
        data->mHandler->update();
        if (data->mHandler->hasChangedAndReset()) data->flags->messagesChanged();
        renderer->render();
//        Util::Async::Thread::sleep(Util::Time::Timestamp::ofMilliseconds(1) );
    }
}

void Pic::checkMouseInput() {
    uint8_t mouseValues[4]{};
    uint32_t mouseValueIndex = 0;
    int16_t value = data->mouseInputStream->read();
    bool mouseChanged = false;
    bool clicked = false;
    data->newlyPressed = false;
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
                data->newlyPressed = true;
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
            Layer *l = data->layers->current();
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
            Layer *l = data->layers->current();
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
                data->layers->drawLineCurrent(lastX, lastY, relX, relY, data->currentTool == Tool::PEN ? penColor : 0x00000000,
                                              data->penSize);
                lastX = relX, lastY = relY;
            }
            // put last click back for next iteration
            data->mouseClicks->offer(Util::Pair<int, int>(data->mouseX, data->mouseY));
            data->flags->currentLayerChanged();
        } else if (data->currentTool == Tool::EXPORT_PNG || data->currentTool == Tool::EXPORT_JPG ||
                   data->currentTool == Tool::EXPORT_BMP || data->currentTool == Tool::NEW_EMPTY) {
            if (data->newlyPressed) {
                auto click = data->mouseClicks->peek();
                data->layerX = click.first - 200;
                data->layerY = click.second;
                while (data->mouseClicks->size() > 0) {
                    click = data->mouseClicks->poll();
                    data->layerW = click.first - 200 - data->layerX;
                    data->layerH = click.second - data->layerY;
                }
            } else {
                data->layerW = data->mouseX - 200 - data->layerX;
                data->layerH = data->mouseY - data->layerY;
            }
            data->flags->guiButtonChanged();
            data->flags->overlayChanged();
            data->currentGuiLayerBottom->appear();
        }
    } else if (data->mouseX >= 200 && !data->clickStartedOnGui && clicked &&
               (data->currentTool == Tool::PEN || data->currentTool == Tool::ERASER)) {
        data->layers->prepareNextDrawingCurrent(); // proper blending for the next drawing
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
                auto currentLayer = data->layers->current();
                switch (scancode) {
                    case Util::Io::Key::ESC:
                        data->running = false;
                        break;
                    case Util::Io::Key::UP:
                        data->layers->moveCurrent(currentLayer->posX, currentLayer->posY - 10);
                        data->flags->currentLayerChanged();
                        data->flags->overlayChanged();
                        break;
                    case Util::Io::Key::DOWN:
                        data->layers->moveCurrent(currentLayer->posX, currentLayer->posY + 10);
                        data->flags->currentLayerChanged();
                        data->flags->overlayChanged();
                        break;
                    case Util::Io::Key::LEFT:
                        data->layers->moveCurrent(currentLayer->posX - 10, currentLayer->posY);
                        data->flags->currentLayerChanged();
                        data->flags->overlayChanged();
                        break;
                    case Util::Io::Key::RIGHT:
                        data->layers->moveCurrent(currentLayer->posX + 10, currentLayer->posY);
                        data->flags->currentLayerChanged();
                        data->flags->overlayChanged();
                        break;
                    case Util::Io::Key::TAB:
                        data->layers->setCurrentToNext();
                        data->layers->prepareNextDrawingCurrent();
                        data->flags->layerOrderChanged();
                        data->flags->overlayChanged();
                        data->currentGuiLayer->appear();
                        break;
                    case Util::Io::Key::SPACE:
                        data->layers->current()->isVisible = !data->layers->current()->isVisible;
                        data->flags->currentLayerChanged();
                        data->flags->overlayChanged();
                        data->currentGuiLayer->appear();
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
                data->layers->prepareNextDrawingCurrent();
                break;
            case Tool::COLOR:
                data->currentGuiLayerBottom = data->guiLayers->get("bottom_color");
                break;
            case Tool::EXPORT_PNG:
            case Tool::EXPORT_JPG:
            case Tool::EXPORT_BMP:
            case Tool::NEW_EMPTY:
                data->currentGuiLayerBottom = data->guiLayers->get("bottom_xywh");
                break;
            case Tool::COMBINE:
                data->currentGuiLayerBottom = data->guiLayers->get("bottom_combine");
                break;
            case Tool::DUPLICATE:
                data->currentGuiLayerBottom = data->guiLayers->get("bottom_duplicate");
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
                                ->set16Bitmap(Bitmaps::cross)
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
    gui_main->addButton((new Button(data))
                                ->setInfo("layer-Tools")
                                ->setMethodButton([](DataWrapper *data) {
                                    changeGuiLayerTo(data, "layerTools");
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
                                ->set16Bitmap(Bitmaps::arrow_back)
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
                                 ->set16Bitmap(Bitmaps::arrow_back)
    );
    gui_tools->addButton((new Button(data))
                                 ->setInfo("Move")
                                 ->setMethodButton([](DataWrapper *data) {
                                     data->moveX = data->layers->current()->posX;
                                     data->moveY = data->layers->current()->posY;
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
                                 ->setInfo("auto Scale & to (0,0)")
                                 ->setMethodButton([](DataWrapper *data) {
                                     data->layers->moveCurrent(0, 0);
                                     auto l = data->layers->current();
                                     if (l->posX + l->width > data->workAreaX || l->posY + l->height > data->workAreaY) {
                                         double factor = min((double) data->workAreaX / (double) l->width,
                                                             (double) data->workAreaY / (double) l->height);
                                         data->layers->scaleCurrent(factor, ToolCorner::BOTTOM_RIGHT);
                                     }
                                 })
                                 ->setRenderFlagMethod(&RenderFlags::currentLayerChanged)
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
                                 ->setInfo("auto Crop")
                                 ->setMethodButton([](DataWrapper *data) {
                                     data->layers->autoCropCurrent();
                                 })
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
                                      data->currentGuiLayerBottom = data->guiLayers->get("empty");
                                  })
                                  ->setRenderFlagMethod(&RenderFlags::guiLayerChanged)
                                  ->set16Bitmap(Bitmaps::arrow_back)
    );
    for (int i = 0; i < data->layers->maxNum(); i++) {
        gui_layers->addButton((new Button(data))
                                      ->setInfo("LayerButton")
                                      ->setLayerButton(i)
                                      ->setRenderFlagMethod(&RenderFlags::layerOrderChanged)
                                      ->setAppearTopOnChange(true)
        );
    }

    auto gui_bottom_move = new GuiLayer();
    gui_bottom_move->addButton((new Button(data))
                                       ->setInfo("moveX")
                                       ->setIntValueButton(&data->moveX)
    );
    gui_bottom_move->addButton((new Button(data))
                                       ->setInfo("moveY")
                                       ->setIntValueButton(&data->moveY)
    );
    gui_bottom_move->addButton((new Button(data))
                                       ->setInfo("MOVE")
                                       ->setConfirmButton([](DataWrapper *data) {
                                           data->moveX = data->layers->current()->posX;
                                           data->moveY = data->layers->current()->posY;
                                       }, [](DataWrapper *data) {
                                           data->layers->moveCurrent(data->moveX, data->moveY);
                                       })
                                       ->setRenderFlagMethod(&RenderFlags::currentLayerChanged)
                                       ->setAppearBottomOnChange(true)
    );

    auto gui_bottom_scale = new GuiLayer();
    gui_bottom_scale->addButton((new Button(data))
                                        ->setInfo("scale")
                                        ->setDoubleValueButton(&data->scale, 0.1, 10.0)
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
                                        }, [](DataWrapper *data) {
                                            data->layers->scaleCurrent(data->scale, data->toolCorner);
                                            data->scale = 1.0;
                                        })
                                        ->setRenderFlagMethod(&RenderFlags::currentLayerChanged)
                                        ->setAppearBottomOnChange(true)
    );

    auto gui_bottom_crop = new GuiLayer();
    gui_bottom_crop->addButton((new Button(data))
                                       ->setInfo("cropLeft")
                                       ->setIntValueButton(&data->cropLeft)
    );
    gui_bottom_crop->addButton((new Button(data))
                                       ->setInfo("cropRight")
                                       ->setIntValueButton(&data->cropRight)
    );
    gui_bottom_crop->addButton((new Button(data))
                                       ->setInfo("cropTop")
                                       ->setIntValueButton(&data->cropTop)
    );
    gui_bottom_crop->addButton((new Button(data))
                                       ->setInfo("cropBottom")
                                       ->setIntValueButton(&data->cropBottom)
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
    );
    gui_bottom_crop->addButton((new Button(data))
                                       ->setInfo("CROP")
                                       ->setConfirmButton([](DataWrapper *data) {
                                           data->cropLeft = 0, data->cropRight = 0, data->cropTop = 0, data->cropBottom = 0;
                                       }, [](DataWrapper *data) {
                                           data->layers->cropCurrent(data->cropLeft, data->cropRight, data->cropTop, data->cropBottom);
                                           data->cropLeft = 0, data->cropRight = 0, data->cropTop = 0, data->cropBottom = 0;
                                       })
                                       ->setRenderFlagMethod(&RenderFlags::currentLayerChanged)
                                       ->setAppearBottomOnChange(true)
    );

    auto gui_bottom_color = new GuiLayer();
    gui_bottom_color->addButton((new Button(data))
                                        ->setInfo("Alpha")
                                        ->setIntValueButton(&data->colorA, 0, 255)
                                        ->setAppearTopOnChange(true)
    );
    gui_bottom_color->addButton((new Button(data))
                                        ->setInfo("Red")
                                        ->setIntValueButton(&data->colorR, 0, 255)
                                        ->setAppearTopOnChange(true)
    );
    gui_bottom_color->addButton((new Button(data))
                                        ->setInfo("Green")
                                        ->setIntValueButton(&data->colorG, 0, 255)
                                        ->setAppearTopOnChange(true)
    );
    gui_bottom_color->addButton((new Button(data))
                                        ->setInfo("Blue")
                                        ->setIntValueButton(&data->colorB, 0, 255)
                                        ->setAppearTopOnChange(true)
    );

    auto gui_bottom_rotate = new GuiLayer();
    gui_bottom_rotate->addButton((new Button(data))
                                         ->setInfo("rotate")
                                         ->setIntValueButton(&data->rotateDeg, -180, 180)
    );
    gui_bottom_rotate->addButton((new Button(data))
                                         ->setInfo("ROTATE")
                                         ->setConfirmButton([](DataWrapper *data) {
                                             data->rotateDeg = 0;
                                         }, [](DataWrapper *data) {
                                             data->layers->rotateCurrent(data->rotateDeg);
                                             data->rotateDeg = 0;
                                         })
                                         ->setRenderFlagMethod(&RenderFlags::currentLayerChanged)
                                         ->setAppearBottomOnChange(true)
    );

    auto gui_bottom_pen = new GuiLayer();
    gui_bottom_pen->addButton((new Button(data))
                                      ->setInfo("Pen Size")
                                      ->setIntValueButton(&data->penSize, 1, 100)
    );

    auto gui_layerTools = new GuiLayer();
    gui_layerTools->addButton((new Button(data))
                                      ->setInfo("Back")
                                      ->setMethodButton([](DataWrapper *data) {
                                          changeGuiLayerTo(data, "main");
                                      })
                                      ->setRenderFlagMethod(&RenderFlags::guiLayerChanged)
                                      ->set16Bitmap(Bitmaps::arrow_back)
    );
    gui_layerTools->addButton((new Button(data))
                                      ->setInfo("new Empty Layer")
                                      ->setMethodButton([](DataWrapper *data) {
                                          swapTool(data, Tool::NEW_EMPTY);
                                      })
                                      ->changeGreenIfTool(Tool::NEW_EMPTY)
                                      ->setRenderFlagMethod(&RenderFlags::guiLayerChanged)
    );
    gui_layerTools->addButton((new Button(data))
                                      ->setInfo("new Image Layer (path)")
                                      ->setMethodButton([](DataWrapper *data) {
                                          data->layers->addPicture(data->currentInput->operator const char *(), 0, 0);
                                      })
                                      ->setRenderFlagMethod(&RenderFlags::layerOrderChanged)
    );
    gui_layerTools->addButton((new Button(data))
                                      ->setInfo("Export all PNG (path)")
                                      ->setMethodButton([](DataWrapper *data) {
                                          swapTool(data, Tool::EXPORT_PNG);
                                      })
                                      ->changeGreenIfTool(Tool::EXPORT_PNG)
                                      ->setRenderFlagMethod(&RenderFlags::guiLayerChanged)
    );
    gui_layerTools->addButton((new Button(data))
                                      ->setInfo("Export all JPG (path)")
                                      ->setMethodButton([](DataWrapper *data) {
                                          swapTool(data, Tool::EXPORT_JPG);
                                      })
                                      ->changeGreenIfTool(Tool::EXPORT_JPG)
                                      ->setRenderFlagMethod(&RenderFlags::guiLayerChanged)
    );
    gui_layerTools->addButton((new Button(data))
                                      ->setInfo("Export all BMP (path)")
                                      ->setMethodButton([](DataWrapper *data) {
                                          swapTool(data, Tool::EXPORT_BMP);
                                      })
                                      ->changeGreenIfTool(Tool::EXPORT_BMP)
                                      ->setRenderFlagMethod(&RenderFlags::guiLayerChanged)
    );
    gui_layerTools->addButton((new Button(data))
                                      ->setInfo("combine Layers")
                                      ->setMethodButton([](DataWrapper *data) {
                                          data->combineFirst = 0;
                                          data->combineSecond = 1;
                                          swapTool(data, Tool::COMBINE);
                                      })
                                      ->changeGreenIfTool(Tool::COMBINE)
                                      ->setRenderFlagMethod(&RenderFlags::guiLayerChanged)
    );
    gui_layerTools->addButton((new Button(data))
                                      ->setInfo("duplicate Layer")
                                      ->setMethodButton([](DataWrapper *data) {
                                          data->dupeIndex = 0;
                                          swapTool(data, Tool::DUPLICATE);
                                      })
                                      ->changeGreenIfTool(Tool::DUPLICATE)
                                      ->setRenderFlagMethod(&RenderFlags::guiLayerChanged)
    );


    auto gui_bottom_xywh = new GuiLayer();
    gui_bottom_xywh->addButton((new Button(data))
                                       ->setInfo("X")
                                       ->setIntValueButton(&data->layerX)
    );
    gui_bottom_xywh->addButton((new Button(data))
                                       ->setInfo("Y")
                                       ->setIntValueButton(&data->layerY)
    );
    gui_bottom_xywh->addButton((new Button(data))
                                       ->setInfo("Width")
                                       ->setIntValueButton(&data->layerW)
    );
    gui_bottom_xywh->addButton((new Button(data))
                                       ->setInfo("Height")
                                       ->setIntValueButton(&data->layerH)
    );
    gui_bottom_xywh->addButton((new Button(data))
                                       ->setInfo("EXPORT")
                                       ->setConfirmButton([](DataWrapper *data) {
                                           data->layerX = 0, data->layerY = 0, data->layerW = data->workAreaX, data->layerH = data->workAreaY;
                                       }, [](DataWrapper *data) {
                                           if (data->currentTool == Tool::NEW_EMPTY) {
                                               data->layers->addEmpty(data->layerX, data->layerY, data->layerW, data->layerH);
                                           } else {
                                               data->layers->exportPicture(data->currentInput->operator const char *(),
                                                                           data->layerX, data->layerY, data->layerW, data->layerH,
                                                                           data->currentTool == Tool::EXPORT_PNG,
                                                                           data->currentTool == Tool::EXPORT_JPG,
                                                                           data->currentTool == Tool::EXPORT_BMP);
                                           }
                                           data->layerX = 0, data->layerY = 0, data->layerW = data->workAreaX, data->layerH = data->workAreaY;
                                       })
                                       ->setRenderFlagMethod(&RenderFlags::currentLayerChanged)
                                       ->setAppearBottomOnChange(true)
    );

    auto gui_bottom_combine = new GuiLayer();
    gui_bottom_combine->addButton((new Button(data))
                                          ->setInfo("first")
                                          ->setIntValueButton(&data->combineFirst, 0, data->layers->maxNum())
    );
    gui_bottom_combine->addButton((new Button(data))
                                          ->setInfo("second")
                                          ->setIntValueButton(&data->combineSecond, 0, data->layers->maxNum())
    );
    gui_bottom_combine->addButton((new Button(data))
                                          ->setInfo("Combine")
                                          ->setConfirmButton([](DataWrapper *data) {
                                              data->combineFirst = 0;
                                              data->combineSecond = 1;
                                          }, [](DataWrapper *data) {
                                              data->layers->combine(data->combineFirst, data->combineSecond);
                                              data->combineFirst = 0;
                                              data->combineSecond = 1;
                                          })
                                          ->setRenderFlagMethod(&RenderFlags::layerOrderChanged)
                                          ->setAppearBottomOnChange(true)
    );

    auto gui_bottom_duplicate = new GuiLayer();
    gui_bottom_duplicate->addButton((new Button(data))
                                          ->setInfo("index")
                                          ->setIntValueButton(&data->dupeIndex, 0, data->layers->maxNum())
    );
    gui_bottom_duplicate->addButton((new Button(data))
                                          ->setInfo("Duplicate")
                                          ->setConfirmButton([](DataWrapper *data) {
                                              data->dupeIndex = 0;
                                          }, [](DataWrapper *data) {
                                              data->layers->duplicate(data->dupeIndex);
                                              data->dupeIndex = 0;
                                          })
                                          ->setRenderFlagMethod(&RenderFlags::layerOrderChanged)
                                          ->setAppearBottomOnChange(true)
    );


    auto gui_empty = new GuiLayer();

    data->textButton = (new Button(data))
            ->setInfo("input: ")
            ->setInputButton(data->currentInput, &data->captureInput)
            ->set16Bitmap(Bitmaps::cross);

    data->guiLayers->put("main", gui_main);
    data->guiLayers->put("file", gui_file);
    data->guiLayers->put("tools", gui_tools);
    data->guiLayers->put("layers", gui_layers);
    data->guiLayers->put("layerTools", gui_layerTools);
    data->guiLayers->put("empty", gui_empty);
    data->guiLayers->put("bottom_move", gui_bottom_move);
    data->guiLayers->put("bottom_rotate", gui_bottom_rotate);
    data->guiLayers->put("bottom_scale", gui_bottom_scale);
    data->guiLayers->put("bottom_crop", gui_bottom_crop);
    data->guiLayers->put("bottom_pen", gui_bottom_pen);
    data->guiLayers->put("bottom_color", gui_bottom_color);
    data->guiLayers->put("bottom_xywh", gui_bottom_xywh);
    data->guiLayers->put("bottom_combine", gui_bottom_combine);
    data->guiLayers->put("bottom_duplicate", gui_bottom_duplicate);
    data->currentGuiLayer = gui_main;
    data->currentGuiLayerBottom = gui_empty;
    data->currentGuiLayer->appear();
    data->currentGuiLayerBottom->appear();
    data->textButton->removeInteraction();
}
