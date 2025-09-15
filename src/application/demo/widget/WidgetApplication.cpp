/*
 * Copyright (C) 2017-2025 Heinrich Heine University Düsseldorf,
 * Institute of Computer Science, Department Operating Systems
 * Main developers: Christian Gesse <christian.gesse@hhu.de>, Fabian Ruhland <ruhland@hhu.de>
 * Original development team: Burak Akguel, Christian Gesse, Fabian Ruhland, Filip Krakowski, Michael Schöttner
 * This project has been supported by several students.
 * A full list of integrated student theses can be found here: https://github.com/hhuOS/hhuOS/wiki/Student-theses
 *
 * This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public
 * License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any
 * later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied
 * warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>
 */

#include "WidgetApplication.h"

#include "base/System.h"
#include "io/key/MouseDecoder.h"
#include "graphic/Colors.h"

WidgetApplication::WidgetApplication(Util::Graphic::LinearFrameBuffer &lfb, const size_t width, const size_t height) :
    Container(width, height), lfb(lfb), bufferedLfb(lfb), mouseInputStream("/device/mouse")
{
    Util::Io::File::setAccessMode(Util::Io::STANDARD_INPUT, Util::Io::File::NON_BLOCKING);
    mouseInputStream.setAccessMode(Util::Io::File::NON_BLOCKING);
}

void WidgetApplication::pack() {
    const auto preferredWidth = getPreferredWidth();
    const auto preferredHeight = getPreferredHeight();
    const auto lfbWidth = lfb.getResolutionX();
    const auto lfbHeight = lfb.getResolutionY();

    setSize(preferredWidth > lfbWidth ? lfbWidth : preferredWidth,
        preferredHeight > lfbHeight ? lfbHeight : preferredHeight);
}

void WidgetApplication::setSize(const size_t width, const size_t height) {
    bufferedLfb.clear();
    Container::setSize(width, height);
}

void WidgetApplication::update() {
    auto scancode = Util::System::in.read();
    while (scancode >= 0) {
        if (keyDecoder.parseScancode(scancode)) {
            auto key = keyDecoder.getCurrentKey();

            if (lastPressedWidget != nullptr) {
                if (key.isPressed()) {
                    lastPressedWidget->keyPressed(key);
                } else {
                    lastPressedWidget->keyReleased(key);
                    lastPressedWidget->keyTyped(key);
                }
            }
        }

        scancode = Util::System::in.read();
    }

    bool mousePositionChanged = false;
    auto mouseByte = mouseInputStream.read();
    while (mouseByte >= 0) {
        mouseInput[mouseInputIndex++] = mouseByte;

        if (mouseInputIndex == 4) {
            // Calculate new mouse position
            const auto mouseUpdate = Util::Io::MouseDecoder::decode(mouseInput);
            const auto containerPosX = static_cast<int32_t>(getPosX());
            const auto containerPosY = static_cast<int32_t>(getPosY());

            if (mouseUpdate.xMovement != 0 || mouseUpdate.yMovement != 0) {
                mousePositionChanged = true;
                mouseX += mouseUpdate.xMovement;
                mouseY -= mouseUpdate.yMovement;
            }

            if (mouseX < containerPosX) {
                mouseX = containerPosX;
            } else if (mouseX >= static_cast<int32_t>(containerPosX + getWidth())) {
                mouseX = static_cast<int32_t>(containerPosX + getWidth() - 1);
            }

            if (mouseY < containerPosY) {
                mouseY = containerPosY;
            } else if (mouseY >= static_cast<int32_t>(containerPosY + getHeight())) {
                mouseY = static_cast<int32_t>(containerPosY + getHeight() - 1);
            }

            // Handle mouse enter/leave events
            auto *hoveredWidget = getChildAtPoint(mouseX, mouseY);
            if (hoveredWidget != lastHoveredWidget) {
                if (lastHoveredWidget != nullptr) {
                    lastHoveredWidget->mouseExited();
                }
                if (hoveredWidget != nullptr) {
                    hoveredWidget->mouseEntered();
                }
            }
            lastHoveredWidget = hoveredWidget;

            // Handle mouse press/release events
            if (!mouseButtonLeft && mouseUpdate.buttons & Util::Io::MouseDecoder::LEFT_BUTTON) {
                // Mouse pressed
                if (lastPressedWidget != nullptr) {
                    lastPressedWidget->setFocused(false);
                }

                lastPressedWidget = getChildAtPoint(mouseX, mouseY);
                if (lastPressedWidget != nullptr) {
                    lastPressedWidget->setFocused(true);
                    lastPressedWidget->mousePressed();
                }
            }

            if (mouseButtonLeft && !(mouseUpdate.buttons & Util::Io::MouseDecoder::LEFT_BUTTON)) {
                // Mouse released
                if (lastPressedWidget != nullptr) {
                    lastPressedWidget->mouseReleased();
                    lastPressedWidget->mouseClicked();
                }
            }

            mouseButtonLeft = mouseUpdate.buttons & Util::Io::MouseDecoder::LEFT_BUTTON;

            mouseInputIndex = 0;
        }

        mouseByte = mouseInputStream.read();
    }

    // Draw screen
    const auto redrawRootContainer = requiresRedraw();
    if (redrawRootContainer || mousePositionChanged) {
        // Draw widgets
        if (redrawRootContainer) {
            draw(bufferedLfb);
        }
        bufferedLfb.flush();

        // Draw mouse cursor
        lfb.drawLine(mouseX - 10, mouseY, mouseX + 10, mouseY, Util::Graphic::Colors::RED);
        lfb.drawLine(mouseX, mouseY - 10, mouseX, mouseY + 10, Util::Graphic::Colors::RED);
    }
}
