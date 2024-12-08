#include "Pic.h"

#include "lib/util/graphic/LinearFrameBuffer.h"
#include "lib/util/base/ArgumentParser.h"
#include "lib/util/io/stream/FileInputStream.h"
#include "lib/util/io/key/KeyDecoder.h"
#include "lib/util/io/key/layout/DeLayout.h"
#include "lib/util/io/key/MouseDecoder.h"

#include "Button.h"
#include "Layers.h"
#include "Layer.h"
#include "GuiLayer.h"
#include "Renderer.h"
#include "DataWrapper.h"
#include "History.h"
#include "Settings.h"
#include "MessageHandler.h"

int32_t main([[maybe_unused]] int32_t argc, [[maybe_unused]] char *argv[]) {
    auto argumentParser = Util::ArgumentParser();
    argumentParser.setHelpText("Pic - A Basic Image Editing Program.\n"
                               "Usage: pic\n"
                               "Options:\n"
                               "  -r, --resolution: Set display resolution\n"
                               "  -h, --help: Show this help message");

    argumentParser.addArgument("resolution", false, "r");

    if (!argumentParser.parse(argc, argv)) {
        Util::System::error << argumentParser.getErrorString() << Util::Io::PrintStream::endl << Util::Io::PrintStream::flush;
        return -1;
    }

    auto lfbFile = new Util::Io::File("/device/lfb");

    if (argumentParser.hasArgument("resolution")) {
        auto split1 = argumentParser.getArgument("resolution").split("x");
        auto split2 = split1[1].split("@");

        uint32_t resolutionX = Util::String::parseInt(split1[0]);
        uint32_t resolutionY = Util::String::parseInt(split2[0]);
        uint32_t colorDepth = split2.length() > 1 ? Util::String::parseInt(split2[1]) : 32;
        if (resolutionX < 800 || resolutionY < 600) {
            resolutionX = 800;
            resolutionY = 600;
        }

        lfbFile->controlFile(Util::Graphic::LinearFrameBuffer::SET_RESOLUTION,
                             Util::Array<uint32_t>({resolutionX, resolutionY, colorDepth}));
    }

    auto pic = new Pic(lfbFile);
    print("starting render loop...");
    pic->run();
    delete pic;

    Util::System::out << Util::Io::PrintStream::flush;

    return 0;
}

Pic::Pic(Util::Io::File *lfbFile) {
    this->data = new DataWrapper(lfbFile);
    this->renderer = new Renderer(data);

    data->history = new History(data->mHandler);
    data->layers = new Layers(data->mHandler, data->history);
    data->layers->addEmpty(50, 50, data->workAreaX - 100, data->workAreaY - 100);
    data->layers->addPicture("/user/pic/test.jpg", 100, 100);

    init_gui();
}


void Pic::run() {
    data->mHandler->setPrintBool(false); // only write to logs before and after render loop
    while (data->running) {
        checkMouseInput();
        checkKeyboardInput();
        data->mHandler->update();
        if (data->mHandler->hasChangedAndReset()) data->flags->messagesChanged();
        renderer->render();
    }
    data->mHandler->setPrintBool(true); // only write to logs before and after render loop
}

Pic::~Pic() {
    print("saving settings...");
    data->settings->saveToFile();
    print("done");

    print("freeing memory...");
    auto keys = data->guiLayers->keys();
    for (uint32_t i = 0; i < keys.length(); i++) {
        GuiLayer *layer = data->guiLayers->get(keys[i]);
        delete layer;
    }
    delete data->textButton;
    data->guiLayers->clear();
    delete data;
    delete renderer;
    print("done");

    print("bye :)");
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
    int buttonIndexBottom = buttonIndex - (data->buttonCount - 1) + data->currentGuiLayerBottom->buttonCount;
    int lastInteractedBottom = data->lastInteractedButton - (data->buttonCount - 1) + data->currentGuiLayerBottom->buttonCount;

    if (buttonIndex != data->lastInteractedButton || data->mouseX >= 200) {
        if (data->lastInteractedButton != -1) {
            if (data->lastInteractedButton < data->currentGuiLayer->buttonCount)
                data->currentGuiLayer->buttons[data->lastInteractedButton]->removeInteraction();
            else if (lastInteractedBottom >= 0 && lastInteractedBottom < data->currentGuiLayerBottom->buttonCount)
                data->currentGuiLayerBottom->buttons[lastInteractedBottom]->removeInteraction();
            else if (data->lastInteractedButton == (data->buttonCount - 1))
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
        if (buttonIndex == (data->buttonCount - 1))
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
            if (data->layers->currentNum() >= 0) {
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
            }
        } else if (data->currentTool == Tool::ROTATE) {
            data->rotateDeg += data->xMovement;
            if (data->rotateDeg > 180) data->rotateDeg -= 360;
            if (data->rotateDeg < -180) data->rotateDeg += 360;
            data->flags->guiButtonChanged();
            data->flags->overlayChanged();
            data->currentGuiLayerBottom->appear();
        } else if ((data->currentTool == Tool::PEN || data->currentTool == Tool::ERASER) && data->mouseClicks->size() > 0) {
            if (data->layers->currentNum() >= 0) {
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
                    data->layers->drawLineCurrent(lastX, lastY, relX, relY,
                                                  data->currentTool == Tool::PEN ? penColor : 0x00000000, data->penSize);
                    lastX = relX, lastY = relY;
                }
                // put last click back for next iteration
                data->mouseClicks->offer(Util::Pair<int, int>(data->mouseX, data->mouseY));
                data->flags->currentLayerChanged();
            }
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
        } else if (data->currentTool == Tool::SHAPE) {
            if (data->newlyPressed) {
                auto click = data->mouseClicks->peek();
                data->shapeX = click.first - 200;
                data->shapeY = click.second;
                while (data->mouseClicks->size() > 0) {
                    click = data->mouseClicks->poll();
                    data->shapeW = click.first - 200 - data->shapeX;
                    data->shapeH = click.second - data->shapeY;
                }
            } else {
                data->shapeW = data->mouseX - 200 - data->shapeX;
                data->shapeH = data->mouseY - data->shapeY;
            }
            data->flags->guiButtonChanged();
            data->flags->overlayChanged();
            data->currentGuiLayerBottom->appear();
        } else if (data->currentTool == Tool::REPLACE_COLOR) {
            if (data->layers->currentNum() >= 0) {
                data->replaceColorX = data->mouseX - 200;
                data->replaceColorY = data->mouseY;
                data->flags->guiButtonChanged();
                data->flags->overlayChanged();
                data->currentGuiLayerBottom->appear();
            }
        }
    } else if (data->mouseX >= 200 && !data->clickStartedOnGui && clicked &&
               (data->currentTool == Tool::PEN || data->currentTool == Tool::ERASER)) {
        data->layers->prepareNextDrawingCurrent(); // proper blending for the next drawing
    }

    data->debugString = Util::String::format(
            "Mouse: %d %d, currentTool: %d, queueLength: %d, clickStartedOnGui: %d",
            data->mouseX, data->mouseY, data->currentTool, data->mouseClicks->size(), data->clickStartedOnGui
    ).operator const char *();
    data->flags->overlayChanged();
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
            case Tool::SHAPE:
                data->currentGuiLayerBottom = data->guiLayers->get("bottom_shape");
                break;
            case Tool::REPLACE_COLOR:
                data->currentGuiLayerBottom = data->guiLayers->get("bottom_replace_color");
                break;
            case Tool::NOTHING:
                data->currentGuiLayerBottom = data->guiLayers->get("empty");
                break;
        }
    }
    data->currentGuiLayerBottom->appear();
    data->currentGuiLayer->appear();
    data->flags->overlayChanged();
}

void changeGuiLayerTo(DataWrapper *data, const char *layer) {
    if (Util::String(layer) == "main") {
        data->inMainMenu = true;
    } else {
        data->inMainMenu = false;
    }
    data->currentGuiLayer = data->guiLayers->get(layer);
    data->currentGuiLayer->appear();
    swapTool(data, Tool::NOTHING);
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
                if (data->settings->activateHotkeys) {
                    switch (scancode) {
                        case 2: // 1
                            changeGuiLayerTo(data, "file");
                            data->flags->guiLayerChanged();
                            break;
                        case 3: // 2
                            changeGuiLayerTo(data, "tools");
                            data->flags->guiLayerChanged();
                            break;
                        case 4: // 3
                            changeGuiLayerTo(data, "layers");
                            data->flags->guiLayerChanged();
                            break;
                        case 5: // 4
                            changeGuiLayerTo(data, "layerTools");
                            data->flags->guiLayerChanged();
                            break;
                        case 6: // 5
                            changeGuiLayerTo(data, "filter");
                            data->flags->guiLayerChanged();
                            break;
                        case 7: // 6
                            changeGuiLayerTo(data, "settings");
                            data->flags->guiLayerChanged();
                            break;
                        case Util::Io::Key::ESC:
                            if (data->inMainMenu) {
                                data->running = false;
                            } else {
                                changeGuiLayerTo(data, "main");
                                data->flags->guiLayerChanged();
                            }
                            break;
                        case Util::Io::Key::Z: // undo
                            data->history->undo(data->layers);
                            data->flags->layerOrderChanged();
                            break;
                        case Util::Io::Key::Y: // redo
                            data->history->redo(data->layers);
                            data->flags->layerOrderChanged();
                            break;
                        case Util::Io::Key::TAB: // switch layer
                            data->layers->setCurrentToNext();
                            data->layers->prepareNextDrawingCurrent();
                            data->flags->layerOrderChanged();
                            data->flags->overlayChanged();
                            data->currentGuiLayer->appear();
                            break;
                        case Util::Io::Key::Q: // new empty layer
                            changeGuiLayerTo(data, "layerTools");
                            swapTool(data, Tool::NEW_EMPTY);
                            data->flags->guiLayerChanged();
                            data->flags->overlayChanged();
                            data->currentGuiLayerBottom->appear();
                            data->currentGuiLayer->appear();
                            break;
                        case Util::Io::Key::W: // new img layer
                            data->layers->addPicture(data->currentInput->operator const char *(), 0, 0);
                            if (!data->settings->textCaptureAfterUse) data->captureInput = false, data->textButton->render(), data->flags->guiButtonChanged();
                            data->flags->layerOrderChanged();
                            break;
                        case Util::Io::Key::P: // pen
                            changeGuiLayerTo(data, "tools");
                            data->penSize = 10;
                            swapTool(data, Tool::PEN);
                            break;
                        case Util::Io::Key::E: // eraser
                            changeGuiLayerTo(data, "tools");
                            data->penSize = 10;
                            swapTool(data, Tool::ERASER);
                            break;
                        case Util::Io::Key::N: // shapes
                            changeGuiLayerTo(data, "tools");
                            data->shapeX = 0;
                            data->shapeY = 0;
                            data->shapeW = 0;
                            data->shapeH = 0;
                            swapTool(data, Tool::SHAPE);
                            break;
                        case Util::Io::Key::C: // color
                            changeGuiLayerTo(data, "tools");
                            swapTool(data, Tool::COLOR);
                            break;
                        case Util::Io::Key::M: // move
                            changeGuiLayerTo(data, "tools");
                            if (data->layers->currentNum() >= 0) {
                                data->moveX = data->layers->current()->posX;
                                data->moveY = data->layers->current()->posY;
                            } else {
                                data->moveX = 0;
                                data->moveY = 0;
                            }
                            swapTool(data, Tool::MOVE);
                            break;
                        case Util::Io::Key::R: // rotate
                            changeGuiLayerTo(data, "tools");
                            data->rotateDeg = 0;
                            swapTool(data, Tool::ROTATE);
                            break;
                        case Util::Io::Key::S: // scale
                            changeGuiLayerTo(data, "tools");
                            data->scale = 1.0;
                            swapTool(data, Tool::SCALE);
                            break;
                        case Util::Io::Key::O: // crop
                            changeGuiLayerTo(data, "tools");
                            data->cropLeft = 0;
                            data->cropRight = 0;
                            data->cropTop = 0;
                            data->cropBottom = 0;
                            swapTool(data, Tool::CROP);
                            break;
                        case Util::Io::Key::T: // replace color
                            changeGuiLayerTo(data, "tools");
                            data->replaceColorX = 0;
                            data->replaceColorY = 0;
                            data->replaceColorTolerance = 0.0;
                            swapTool(data, Tool::REPLACE_COLOR);
                            break;
                        case Util::Io::Key::X:
                            this->data->layers->history->printCommands();
                            break;
                        default:
                            break;
                    }
                }
            }
        }
        scancode = Util::System::in.read();
    }
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
                                ->set16Bitmap(Bitmaps::arrow_forward)
                                ->setHotkey('1')
    );
    gui_main->addButton((new Button(data))
                                ->setInfo("tools")
                                ->setMethodButton([](DataWrapper *data) {
                                    changeGuiLayerTo(data, "tools");
                                })
                                ->setRenderFlagMethod(&RenderFlags::guiLayerChanged)
                                ->set16Bitmap(Bitmaps::arrow_forward)
                                ->setHotkey('2')
    );
    gui_main->addButton((new Button(data))
                                ->setInfo("layers")
                                ->setMethodButton([](DataWrapper *data) {
                                    changeGuiLayerTo(data, "layers");
                                })
                                ->setRenderFlagMethod(&RenderFlags::guiLayerChanged)
                                ->set16Bitmap(Bitmaps::arrow_forward)
                                ->setHotkey('3')
    );
    gui_main->addButton((new Button(data))
                                ->setInfo("layer-Tools")
                                ->setMethodButton([](DataWrapper *data) {
                                    changeGuiLayerTo(data, "layerTools");
                                })
                                ->setRenderFlagMethod(&RenderFlags::guiLayerChanged)
                                ->set16Bitmap(Bitmaps::arrow_forward)
                                ->setHotkey('4')
    );
    gui_main->addButton((new Button(data))
                                ->setInfo("filter")
                                ->setMethodButton([](DataWrapper *data) {
                                    changeGuiLayerTo(data, "filter");
                                })
                                ->setRenderFlagMethod(&RenderFlags::guiLayerChanged)
                                ->set16Bitmap(Bitmaps::arrow_forward)
                                ->setHotkey('5')
    );
    gui_main->addButton((new Button(data))
                                ->setInfo("settings")
                                ->setMethodButton([](DataWrapper *data) {
                                    changeGuiLayerTo(data, "settings");
                                })
                                ->setRenderFlagMethod(&RenderFlags::guiLayerChanged)
                                ->set16Bitmap(Bitmaps::arrow_forward)
                                ->setHotkey('6')
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
                                    data->history->loadFromFileInto(data->layers, data->currentInput->operator const char *());
                                    if (!data->settings->textCaptureAfterUse) data->captureInput = false, data->textButton->render(), data->flags->guiButtonChanged();
                                })
                                ->set16Bitmap(Bitmaps::play)
                                ->setRenderFlagMethod(&RenderFlags::layerOrderChanged)
    );
    gui_file->addButton((new Button(data))
                                ->setInfo("save Project")
                                ->setMethodButton([](DataWrapper *data) {
                                    data->history->saveToFile(data->currentInput->operator const char *());
                                    if (!data->settings->textCaptureAfterUse) data->captureInput = false, data->textButton->render(), data->flags->guiButtonChanged();
                                })
                                ->set16Bitmap(Bitmaps::play)
    );
    gui_file->addButton((new Button(data))
                                ->setInfo("undo")
                                ->setMethodButton([](DataWrapper *data) {
                                    data->history->undo(data->layers);
                                })
                                ->set16Bitmap(Bitmaps::play)
                                ->setRenderFlagMethod(&RenderFlags::layerOrderChanged)
                                ->setHotkey('Z')
    );
    gui_file->addButton((new Button(data))
                                ->setInfo("redo")
                                ->setMethodButton([](DataWrapper *data) {
                                    data->history->redo(data->layers);
                                })
                                ->set16Bitmap(Bitmaps::play)
                                ->setRenderFlagMethod(&RenderFlags::layerOrderChanged)
                                ->setHotkey('Y')
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
                                     if (data->layers->currentNum() >= 0) {
                                         data->moveX = data->layers->current()->posX;
                                         data->moveY = data->layers->current()->posY;
                                     } else {
                                         data->moveX = 0;
                                         data->moveY = 0;
                                     }
                                     swapTool(data, Tool::MOVE);
                                 })
                                 ->changeGreenIfTool(Tool::MOVE)
                                 ->setRenderFlagMethod(&RenderFlags::guiLayerChanged)
                                 ->set16Bitmap(Bitmaps::tool)
                                 ->setHotkey('M')
    );
    gui_tools->addButton((new Button(data))
                                 ->setInfo("Rotate")
                                 ->setMethodButton([](DataWrapper *data) {
                                     data->rotateDeg = 0;
                                     swapTool(data, Tool::ROTATE);
                                 })
                                 ->changeGreenIfTool(Tool::ROTATE)
                                 ->setRenderFlagMethod(&RenderFlags::guiLayerChanged)
                                 ->set16Bitmap(Bitmaps::tool)
                                 ->setHotkey('R')
    );
    gui_tools->addButton((new Button(data))
                                 ->setInfo("Scale")
                                 ->setMethodButton([](DataWrapper *data) {
                                     data->scale = 1.0;
                                     swapTool(data, Tool::SCALE);
                                 })
                                 ->changeGreenIfTool(Tool::SCALE)
                                 ->setRenderFlagMethod(&RenderFlags::guiLayerChanged)
                                 ->set16Bitmap(Bitmaps::tool)
                                 ->setHotkey('S')
    );
    gui_tools->addButton((new Button(data))
                                 ->setInfo("auto Scale")
                                 ->setMethodButton([](DataWrapper *data) {
                                     if (data->layers->currentNum() >= 0) {
                                         auto l = data->layers->current();
                                         if (l->width > data->workAreaX || l->height > data->workAreaY) {
                                             data->layers->moveCurrent(0, 0);
                                             double factor = min((double) data->workAreaX / (double) l->width,
                                                                 (double) data->workAreaY / (double) l->height);
                                             data->layers->scaleCurrent(factor, ToolCorner::BOTTOM_RIGHT);
                                         } else if (l->posX + l->width > data->workAreaX || l->posY + l->height > data->workAreaY) {
                                             data->layers->moveCurrent(0, 0);
                                             data->mHandler->addMessage("Layer was just moved onto Screen, no scaling needed");
                                         } else {
                                             data->mHandler->addMessage("No scaling or moving needed");
                                         }
                                     }
                                 })
                                 ->setRenderFlagMethod(&RenderFlags::currentLayerChanged)
                                 ->set16Bitmap(Bitmaps::play)
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
                                 ->set16Bitmap(Bitmaps::tool)
                                 ->setHotkey('O')
    );
    gui_tools->addButton((new Button(data))
                                 ->setInfo("auto Crop")
                                 ->setMethodButton([](DataWrapper *data) {
                                     data->layers->autoCropCurrent();
                                 })
                                 ->setRenderFlagMethod(&RenderFlags::guiLayerChanged)
                                 ->set16Bitmap(Bitmaps::play)
    );
    gui_tools->addButton((new Button(data))
                                 ->setInfo("replace Color")
                                 ->setMethodButton([](DataWrapper *data) {
                                     data->replaceColorX = 0;
                                     data->replaceColorY = 0;
                                     data->replaceColorTolerance = 0.0;
                                     swapTool(data, Tool::REPLACE_COLOR);
                                 })
                                 ->changeGreenIfTool(Tool::REPLACE_COLOR)
                                 ->setRenderFlagMethod(&RenderFlags::guiLayerChanged)
                                 ->set16Bitmap(Bitmaps::tool)
                                 ->setHotkey('T')
    );
    gui_tools->addButton((new Button(data))
                                 ->setInfo("Shapes")
                                 ->setMethodButton([](DataWrapper *data) {
                                     data->shapeX = 0;
                                     data->shapeY = 0;
                                     data->shapeW = 0;
                                     data->shapeH = 0;
                                     swapTool(data, Tool::SHAPE);
                                 })
                                 ->changeGreenIfTool(Tool::SHAPE)
                                 ->setRenderFlagMethod(&RenderFlags::guiLayerChanged)
                                 ->set16Bitmap(Bitmaps::tool)
                                 ->setHotkey('N')
    );
    gui_tools->addButton((new Button(data))
                                 ->setInfo("Pen")
                                 ->setMethodButton([](DataWrapper *data) {
                                     data->penSize = 10;
                                     swapTool(data, Tool::PEN);
                                 })
                                 ->changeGreenIfTool(Tool::PEN)
                                 ->setRenderFlagMethod(&RenderFlags::guiLayerChanged)
                                 ->set16Bitmap(Bitmaps::tool)
                                 ->setHotkey('P')
    );
    gui_tools->addButton((new Button(data))
                                 ->setInfo("Eraser")
                                 ->setMethodButton([](DataWrapper *data) {
                                     data->penSize = 10;
                                     swapTool(data, Tool::ERASER);
                                 })
                                 ->changeGreenIfTool(Tool::ERASER)
                                 ->setRenderFlagMethod(&RenderFlags::guiLayerChanged)
                                 ->set16Bitmap(Bitmaps::tool)
                                 ->setHotkey('E')
    );
    gui_tools->addButton((new Button(data))
                                 ->setInfo("Color")
                                 ->setMethodButton([](DataWrapper *data) {
                                     swapTool(data, Tool::COLOR);
                                 })
                                 ->changeGreenIfTool(Tool::COLOR)
                                 ->setColor(&data->colorA, &data->colorR, &data->colorG, &data->colorB)
                                 ->setRenderFlagMethod(&RenderFlags::guiLayerChanged)
                                 ->set16Bitmap(Bitmaps::tool)
                                 ->setHotkey('C')
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
                                           if (data->layers->currentNum() >= 0) {
                                               data->moveX = data->layers->current()->posX;
                                               data->moveY = data->layers->current()->posY;
                                           } else {
                                               data->moveX = 0;
                                               data->moveY = 0;
                                           }
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
                                            if (data->settings->resetValuesAfterConfirm)
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
                                           if (data->toolCorner == ToolCorner::TOP_LEFT)
                                               data->toolCorner = ToolCorner::TOP_RIGHT;
                                           else if (data->toolCorner == ToolCorner::TOP_RIGHT)
                                               data->toolCorner = ToolCorner::BOTTOM_RIGHT;
                                           else if (data->toolCorner == ToolCorner::BOTTOM_RIGHT)
                                               data->toolCorner = ToolCorner::BOTTOM_LEFT;
                                           else if (data->toolCorner == ToolCorner::BOTTOM_LEFT)
                                               data->toolCorner = ToolCorner::TOP_LEFT;
                                       })
    );
    gui_bottom_crop->addButton((new Button(data))
                                       ->setInfo("CROP")
                                       ->setConfirmButton([](DataWrapper *data) {
                                           data->cropLeft = 0, data->cropRight = 0, data->cropTop = 0, data->cropBottom = 0;
                                       }, [](DataWrapper *data) {
                                           data->layers->cropCurrent(data->cropLeft, data->cropRight, data->cropTop, data->cropBottom);
                                           if (data->settings->resetValuesAfterConfirm)
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
                                             if (data->settings->resetValuesAfterConfirm)
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

    auto gui_bottom_shape = new GuiLayer();
    gui_bottom_shape->addButton((new Button(data))
                                        ->setInfo("Rectangle")
                                        ->setMethodButton([](DataWrapper *data) {
                                            data->currentShape = Shape::RECTANGLE;
                                        })
                                        ->changeGreenIfShape(Shape::RECTANGLE)
                                        ->setAppearBottomOnChange(true)
    );
    gui_bottom_shape->addButton((new Button(data))
                                        ->setInfo("Square")
                                        ->setMethodButton([](DataWrapper *data) {
                                            data->currentShape = Shape::SQUARE;
                                        })
                                        ->changeGreenIfShape(Shape::SQUARE)
                                        ->setAppearBottomOnChange(true)
    );
    gui_bottom_shape->addButton((new Button(data))
                                        ->setInfo("Ellipse")
                                        ->setMethodButton([](DataWrapper *data) {
                                            data->currentShape = Shape::ELLIPSE;
                                        })
                                        ->changeGreenIfShape(Shape::ELLIPSE)
                                        ->setAppearBottomOnChange(true)
    );
    gui_bottom_shape->addButton((new Button(data))
                                        ->setInfo("Circle")
                                        ->setMethodButton([](DataWrapper *data) {
                                            data->currentShape = Shape::CIRCLE;
                                        })
                                        ->changeGreenIfShape(Shape::CIRCLE)
                                        ->setAppearBottomOnChange(true)
    );
    gui_bottom_shape->addButton((new Button(data))
                                        ->setInfo("SHAPE")
                                        ->setConfirmButton([](DataWrapper *data) {
                                            data->shapeX = 0, data->shapeY = 0, data->shapeW = 0, data->shapeH = 0;
                                        }, [](DataWrapper *data) {
                                            if (data->layers->currentNum() >= 0) {
                                                uint32_t penColor =
                                                        (data->colorA << 24) |
                                                        (data->colorR << 16) |
                                                        (data->colorG << 8) |
                                                        data->colorB;
                                                Layer *l = data->layers->current();
                                                int relX = data->shapeX - l->posX, relY = data->shapeY - l->posY;
                                                data->layers->drawShapeCurrent(data->currentShape, relX, relY, data->shapeW, data->shapeH,
                                                                               penColor);
                                                if (data->settings->resetValuesAfterConfirm)
                                                    data->shapeX = 0, data->shapeY = 0, data->shapeW = 0, data->shapeH = 0;
                                            }
                                        })
                                        ->setRenderFlagMethod(&RenderFlags::currentLayerChanged)
                                        ->setAppearBottomOnChange(true)
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
                                      ->set16Bitmap(Bitmaps::tool)
                                      ->setHotkey('Q')
    );
    gui_layerTools->addButton((new Button(data))
                                      ->setInfo("load Image (path)")
                                      ->setMethodButton([](DataWrapper *data) {
                                          data->layers->addPicture(data->currentInput->operator const char *(), 0, 0);
                                          if (!data->settings->textCaptureAfterUse) data->captureInput = false, data->textButton->render(), data->flags->guiButtonChanged();
                                      })
                                      ->setRenderFlagMethod(&RenderFlags::layerOrderChanged)
                                      ->set16Bitmap(Bitmaps::play)
                                      ->setHotkey('W')
    );
    gui_layerTools->addButton((new Button(data))
                                      ->setInfo("Export PNG (path)")
                                      ->setMethodButton([](DataWrapper *data) {
                                          swapTool(data, Tool::EXPORT_PNG);
                                      })
                                      ->changeGreenIfTool(Tool::EXPORT_PNG)
                                      ->setRenderFlagMethod(&RenderFlags::guiLayerChanged)
                                      ->set16Bitmap(Bitmaps::tool)
    );
    gui_layerTools->addButton((new Button(data))
                                      ->setInfo("Export JPG (path)")
                                      ->setMethodButton([](DataWrapper *data) {
                                          swapTool(data, Tool::EXPORT_JPG);
                                      })
                                      ->changeGreenIfTool(Tool::EXPORT_JPG)
                                      ->setRenderFlagMethod(&RenderFlags::guiLayerChanged)
                                      ->set16Bitmap(Bitmaps::tool)
    );
    gui_layerTools->addButton((new Button(data))
                                      ->setInfo("Export BMP (path)")
                                      ->setMethodButton([](DataWrapper *data) {
                                          swapTool(data, Tool::EXPORT_BMP);
                                      })
                                      ->changeGreenIfTool(Tool::EXPORT_BMP)
                                      ->setRenderFlagMethod(&RenderFlags::guiLayerChanged)
                                      ->set16Bitmap(Bitmaps::tool)
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
                                      ->set16Bitmap(Bitmaps::tool)
    );
    gui_layerTools->addButton((new Button(data))
                                      ->setInfo("duplicate Layer")
                                      ->setMethodButton([](DataWrapper *data) {
                                          data->dupeIndex = 0;
                                          swapTool(data, Tool::DUPLICATE);
                                      })
                                      ->changeGreenIfTool(Tool::DUPLICATE)
                                      ->setRenderFlagMethod(&RenderFlags::guiLayerChanged)
                                      ->set16Bitmap(Bitmaps::tool)
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
                                               if (!data->settings->textCaptureAfterUse) data->captureInput = false, data->textButton->render(), data->flags->guiButtonChanged();
                                           }
                                           if (data->settings->resetValuesAfterConfirm)
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
                                              if (data->settings->resetValuesAfterConfirm)
                                                  data->combineFirst = 0, data->combineSecond = 1;
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
                                                if (data->settings->resetValuesAfterConfirm)
                                                    data->dupeIndex = 0;
                                            })
                                            ->setRenderFlagMethod(&RenderFlags::layerOrderChanged)
                                            ->setAppearBottomOnChange(true)
    );

    auto gui_bottom_replace_color = new GuiLayer();
    gui_bottom_replace_color->addButton((new Button(data))
                                                ->setInfo("X")
                                                ->setIntValueButton(&data->replaceColorX)
    );
    gui_bottom_replace_color->addButton((new Button(data))
                                                ->setInfo("Y")
                                                ->setIntValueButton(&data->replaceColorY)
    );
    gui_bottom_replace_color->addButton((new Button(data))
                                                ->setInfo("Tolerance")
                                                ->setDoubleValueButton(&data->replaceColorTolerance, 0.0, 1.0)
    );
    gui_bottom_replace_color->addButton((new Button(data))
                                                ->setInfo("Replace")
                                                ->setConfirmButton([](DataWrapper *data) {
                                                    data->replaceColorX = 0;
                                                    data->replaceColorY = 0;
                                                    data->replaceColorTolerance = 0.0;
                                                }, [](DataWrapper *data) {
                                                    if (data->layers->currentNum() >= 0) {
                                                        uint32_t penColor =
                                                                (data->colorA << 24) | (data->colorR << 16) | (data->colorG << 8) |
                                                                data->colorB;
                                                        Layer *l = data->layers->current();
                                                        int relX = data->replaceColorX - l->posX, relY = data->replaceColorY - l->posY;
                                                        data->layers->replaceColorCurrent(relX, relY, penColor,
                                                                                          data->replaceColorTolerance);
                                                    }
                                                    if (data->settings->resetValuesAfterConfirm)
                                                        data->replaceColorX = 0, data->replaceColorY = 0, data->replaceColorTolerance = 0.0;
                                                })
                                                ->setRenderFlagMethod(&RenderFlags::currentLayerChanged)
                                                ->setAppearBottomOnChange(true)
    );

    auto gui_filter = new GuiLayer();
    gui_filter->addButton((new Button(data))
                                  ->setInfo("Back")
                                  ->setMethodButton([](DataWrapper *data) {
                                      changeGuiLayerTo(data, "main");
                                  })
                                  ->setRenderFlagMethod(&RenderFlags::guiLayerChanged)
                                  ->set16Bitmap(Bitmaps::arrow_back)
    );
    gui_filter->addButton((new Button(data))
                                  ->setInfo("BlackWhite")
                                  ->setMethodButton([](DataWrapper *data) {
                                      data->layers->filterBlackWhiteCurrent();
                                  })
                                  ->setRenderFlagMethod(&RenderFlags::currentLayerChanged)
                                  ->set16Bitmap(Bitmaps::play)
    );
    gui_filter->addButton((new Button(data))
                                  ->setInfo("Invert")
                                  ->setMethodButton([](DataWrapper *data) {
                                      data->layers->filterInvertCurrent();
                                  })
                                  ->setRenderFlagMethod(&RenderFlags::currentLayerChanged)
                                  ->set16Bitmap(Bitmaps::play)
    );
    gui_filter->addButton((new Button(data))
                                  ->setInfo("Sepia")
                                  ->setMethodButton([](DataWrapper *data) {
                                      data->layers->filterSepiaCurrent();
                                  })
                                  ->setRenderFlagMethod(&RenderFlags::currentLayerChanged)
                                  ->set16Bitmap(Bitmaps::play)
    );
    // kernels from:
    // https://medium.com/@timothy_terati/image-convolution-filtering-a54dce7c786b
    // https://en.wikipedia.org/wiki/Kernel_(image_processing)
    gui_filter->addButton((new Button(data))
                                  ->setInfo("Box Blur")
                                  ->setMethodButton([](DataWrapper *data) {
                                      int boxBlurKernel[9] = {
                                              1, 1, 1,
                                              1, 1, 1,
                                              1, 1, 1
                                      };
                                      data->layers->filterKernelCurrent(boxBlurKernel, 9, 0);
                                  })
                                  ->setRenderFlagMethod(&RenderFlags::currentLayerChanged)
                                  ->set16Bitmap(Bitmaps::play)
    );
    gui_filter->addButton((new Button(data))
                                  ->setInfo("Gaussian Blur")
                                  ->setMethodButton([](DataWrapper *data) {
                                      int boxBlurKernel[9] = {
                                              1, 2, 1,
                                              2, 4, 2,
                                              1, 2, 1
                                      };
                                      data->layers->filterKernelCurrent(boxBlurKernel, 16, 0);
                                  })
                                  ->setRenderFlagMethod(&RenderFlags::currentLayerChanged)
                                  ->set16Bitmap(Bitmaps::play)
    );
    gui_filter->addButton((new Button(data))
                                  ->setInfo("Sharpen")
                                  ->setMethodButton([](DataWrapper *data) {
                                      int boxBlurKernel[9] = {
                                              0, -1, 0,
                                              -1, 5, -1,
                                              0, -1, 0
                                      };
                                      data->layers->filterKernelCurrent(boxBlurKernel, 1, 0);
                                  })
                                  ->setRenderFlagMethod(&RenderFlags::currentLayerChanged)
                                  ->set16Bitmap(Bitmaps::play)
    );
    gui_filter->addButton((new Button(data))
                                  ->setInfo("Edge Detection")
                                  ->setMethodButton([](DataWrapper *data) {
                                      int boxBlurKernel[9] = {
                                              -1, -1, -1,
                                              -1, 8, -1,
                                              -1, -1, -1
                                      };
                                      data->layers->filterKernelCurrent(boxBlurKernel, 1, 0);
                                  })
                                  ->setRenderFlagMethod(&RenderFlags::currentLayerChanged)
                                  ->set16Bitmap(Bitmaps::play)
    );


    auto gui_settings = new GuiLayer();
    gui_settings->addButton((new Button(data))
                                    ->setInfo("Back")
                                    ->setMethodButton([](DataWrapper *data) {
                                        changeGuiLayerTo(data, "main");
                                    })
                                    ->setRenderFlagMethod(&RenderFlags::guiLayerChanged)
                                    ->set16Bitmap(Bitmaps::arrow_back)
    );
    gui_settings->addButton((new Button(data))
                                    ->setInfo("reset Settings")
                                    ->setMethodButton([](DataWrapper *data) {
                                        data->settings->resetToDefault();
                                    })
                                    ->setRenderFlagMethod(&RenderFlags::allChanged)
                                    ->setAppearTopOnChange(true)
                                    ->set16Bitmap(Bitmaps::play)
    );
    gui_settings->addButton((new Button(data))
                                    ->setInfo("load Settings")
                                    ->setMethodButton([](DataWrapper *data) {
                                        data->settings->loadFromFile();
                                    })
                                    ->setRenderFlagMethod(&RenderFlags::allChanged)
                                    ->setAppearTopOnChange(true)
                                    ->set16Bitmap(Bitmaps::play)
    );
    gui_settings->addButton((new Button(data))
                                    ->setInfo("save Settings")
                                    ->setMethodButton([](DataWrapper *data) {
                                        data->settings->saveToFile();
                                    })
                                    ->setRenderFlagMethod(&RenderFlags::allChanged)
                                    ->setAppearTopOnChange(true)
                                    ->set16Bitmap(Bitmaps::play)
    );
    gui_settings->addButton((new Button(data))
                                    ->setInfo("checkeredBackground")
                                    ->setBooleanButton(&data->settings->checkeredBackground)
                                    ->setRenderFlagMethod(&RenderFlags::guiButtonChanged)
                                    ->setSecondRenderFlagMethod(&RenderFlags::baseChanged)
    );
    gui_settings->addButton((new Button(data))
                                    ->setInfo("optimizeRendering")
                                    ->setBooleanButton(&data->settings->optimizeRendering)
                                    ->setRenderFlagMethod(&RenderFlags::guiButtonChanged)
    );
    gui_settings->addButton((new Button(data))
                                    ->setInfo("currentLayerOverlay")
                                    ->setBooleanButton(&data->settings->currentLayerOverlay)
                                    ->setRenderFlagMethod(&RenderFlags::guiButtonChanged)
                                    ->setSecondRenderFlagMethod(&RenderFlags::overlayChanged)
    );
    gui_settings->addButton((new Button(data))
                                    ->setInfo("activateHotkeys")
                                    ->setBooleanButton(&data->settings->activateHotkeys)
                                    ->setRenderFlagMethod(&RenderFlags::guiButtonChanged)
    );
    gui_settings->addButton((new Button(data))
                                    ->setInfo("showFPS")
                                    ->setBooleanButton(&data->settings->showFPS)
                                    ->setRenderFlagMethod(&RenderFlags::guiButtonChanged)
    );
    gui_settings->addButton((new Button(data))
                                    ->setInfo("textCaptureAfterUse")
                                    ->setBooleanButton(&data->settings->textCaptureAfterUse)
                                    ->setRenderFlagMethod(&RenderFlags::guiButtonChanged)
    );
    gui_settings->addButton((new Button(data))
                                    ->setInfo("resetValuesAfterConfirm")
                                    ->setBooleanButton(&data->settings->resetValuesAfterConfirm)
                                    ->setRenderFlagMethod(&RenderFlags::guiButtonChanged)
    );
    gui_settings->addButton((new Button(data))
                                    ->setInfo("useBufferedBuffer")
                                    ->setBooleanButton(&data->settings->useBufferedBuffer)
                                    ->setRenderFlagMethod(&RenderFlags::guiButtonChanged)
    );
    gui_settings->addButton((new Button(data))
                                    ->setInfo("showMouseHelper")
                                    ->setBooleanButton(&data->settings->showMouseHelper)
                                    ->setRenderFlagMethod(&RenderFlags::guiButtonChanged)
                                    ->setSecondRenderFlagMethod(&RenderFlags::overlayChanged)
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
    data->guiLayers->put("filter", gui_filter);
    data->guiLayers->put("settings", gui_settings);
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
    data->guiLayers->put("bottom_shape", gui_bottom_shape);
    data->guiLayers->put("bottom_replace_color", gui_bottom_replace_color);
    data->currentGuiLayer = gui_main;
    data->currentGuiLayerBottom = gui_empty;
    data->currentGuiLayer->appear();
    data->currentGuiLayerBottom->appear();
    data->textButton->removeInteraction();
}
