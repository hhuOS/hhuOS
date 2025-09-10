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
 *
 * The widget and layout system is based on a bachelor's thesis, written by Michael Zuchniewski.
 * The original source code can be found here: https://git.hhu.de/bsinfo/thesis/ba-mizuc100
 */

#include "Widget.h"

namespace Util::Graphic {

Widget::Widget(const size_t posX, const size_t posY) : posX(posX), posY(posY) {}

Widget::~Widget() {
    for (const auto *listener : actionListeners) {
        delete listener;
    }
}

size_t Widget::getPosX() const {
    return posX;
}

size_t Widget::getPosY() const {
    return posY;
}

bool Widget::requiresRedraw() const {
    return needsRedraw;
}

bool Widget::requiresParentRedraw() const {
    return needsParentRedraw;
}

void Widget::draw([[maybe_unused]] LinearFrameBuffer &lfb) {
    needsRedraw = false;
    needsParentRedraw = false;
}

void Widget::requireRedraw() {
    needsRedraw = true;
}

void Widget::requireParentRedraw() {
    needsRedraw = true;
    needsParentRedraw = true;
}

void Widget::addActionListener(ActionListener *listener) {
    actionListeners.add(listener);
}

void Widget::mouseClick() const {
    for (auto *listener : actionListeners) {
        listener->onMouseClick();
    }
}

void Widget::mousePress() const {
    for (auto *listener : actionListeners) {
        listener->onMousePress();
    }
}

void Widget::mouseRelease() const {
    for (auto *listener : actionListeners) {
        listener->onMouseRelease();
    }
}

void Widget::mouseEnter() const {
    for (auto *listener : actionListeners) {
        listener->onMouseEnter();
    }
}

void Widget::mouseLeave() const {
    for (auto *listener : actionListeners) {
        listener->onMouseLeave();
    }
}

bool Widget::containsPoint(const size_t px, const size_t py) const {
    const auto x = getPosX();
    const auto y = getPosY();
    const auto w = getWidth();
    const auto h = getHeight();

    return px >= x && px < x + w && py >= y && py < y + h;
}

Widget* Widget::getChildAtPoint(const size_t posX, const size_t posY) {
    if (containsPoint(posX, posY)) {
        return this;
    }

    return nullptr;
}

void Widget::setPosition(const size_t x, const size_t y) {
    posX = x;
    posY = y;
}

}
