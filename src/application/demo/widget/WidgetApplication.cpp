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

#include "io/key/MouseDecoder.h"

WidgetApplication::WidgetApplication(Util::Graphic::LinearFrameBuffer &lfb, const size_t posX, const size_t posY, const size_t width, const size_t height) :
    lfb(lfb), bufferedLfb(lfb), root(posX, posY, width, height), mouseInputStream("/device/mouse")
{
    mouseInputStream.setAccessMode(Util::Io::File::NON_BLOCKING);
}

void WidgetApplication::addWidget(Util::Graphic::Widget &widget, const size_t posX, const size_t posY) {
    root.addChild(widget, posX, posY);
}

void WidgetApplication::update() {
    bool mousePositionChanged = false;
    auto mouseByte = mouseInputStream.read();
    while (mouseByte >= 0) {
        mouseInput[mouseInputIndex++] = mouseByte;

        if (mouseInputIndex == 4) {
            // Calculate new mouse position
            const auto mouseUpdate = Util::Io::MouseDecoder::decode(mouseInput);

            if (mouseUpdate.xMovement != 0 || mouseUpdate.yMovement != 0) {
                mousePositionChanged = true;
                mouseX += mouseUpdate.xMovement;
                mouseY -= mouseUpdate.yMovement;
            }

            if (mouseX < static_cast<int32_t>(root.getPosX())) {
                mouseX = static_cast<int32_t>(root.getPosX());
            } else if (mouseX >= static_cast<int32_t>(root.getPosX() + root.getWidth())) {
                mouseX = static_cast<int32_t>(root.getPosX() + root.getWidth() - 1);
            }

            if (mouseY < static_cast<int32_t>(root.getPosY())) {
                mouseY = static_cast<int32_t>(root.getPosY());
            } else if (mouseY >= static_cast<int32_t>(root.getPosY() + root.getHeight())) {
                mouseY = static_cast<int32_t>(root.getPosY() + root.getHeight() - 1);
            }

            // Handle mouse enter/leave events
            const auto *hoveredWidget = root.getChildAtPoint(mouseX, mouseY);
            if (hoveredWidget != lastHoveredWidget) {
                if (lastHoveredWidget != nullptr) {
                    lastHoveredWidget->mouseLeave();
                }
                if (hoveredWidget != nullptr) {
                    hoveredWidget->mouseEnter();
                }
            }
            lastHoveredWidget = hoveredWidget;

            // Handle mouse press/release events
            if (!mouseButtonLeft && mouseUpdate.buttons & Util::Io::MouseDecoder::LEFT_BUTTON) {
                // Mouse pressed
                lastPressedWidget = root.getChildAtPoint(mouseX, mouseY);
                if (lastPressedWidget != nullptr) {
                    lastPressedWidget->mousePress();
                }
            }

            if (mouseButtonLeft && !(mouseUpdate.buttons & Util::Io::MouseDecoder::LEFT_BUTTON)) {
                // Mouse released
                if (lastPressedWidget != nullptr) {
                    lastPressedWidget->mouseRelease();
                    lastPressedWidget->mouseClick();
                }
            }

            mouseButtonLeft = mouseUpdate.buttons & Util::Io::MouseDecoder::LEFT_BUTTON;

            mouseInputIndex = 0;
        }

        mouseByte = mouseInputStream.read();
    }

    // Draw screen
    const auto redrawRootContainer = root.requiresRedraw();
    if (redrawRootContainer || mousePositionChanged) {
        // Draw widgets
        if (redrawRootContainer) {
            root.draw(bufferedLfb);
        }
        bufferedLfb.flush();

        // Draw mouse cursor
        lfb.drawLine(mouseX - 10, mouseY, mouseX + 10, mouseY, Util::Graphic::Colors::RED);
        lfb.drawLine(mouseX, mouseY - 10, mouseX, mouseY + 10, Util::Graphic::Colors::RED);
    }
}