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

#include "Container.h"

namespace Util::Graphic {

Container::Container(const size_t posX, const size_t posY, const size_t width, const size_t height,
    const Style& style) : Widget(posX, posY), width(width), height(height), style(style) {}

Container::Container(const size_t posX, const size_t posY, const size_t width, const size_t height) :
    Container (posX, posY, width, height, DefaultTheme::container()) {}

void Container::addChild(Widget &child, const size_t relPosX, const size_t relPosY) {
    if (relPosX >= width || relPosY >= height) {
        Panic::fire(Panic::INVALID_ARGUMENT, "Container: Child position out of bounds");
    }

    child.setPosition(getPosX() + relPosX, getPosY() + relPosY);
    children.add(&child);

    requireRedraw();
}

size_t Container::getWidth() const {
    return width;
}

size_t Container::getHeight() const {
    return height;
}

bool Container::requiresRedraw() const {
    bool childNeedsRedraw = false;
    for (const auto *child : children) {
        if (child->requiresRedraw()) {
            childNeedsRedraw = true;
            break;
        }
    }

    return Widget::requiresRedraw() || childNeedsRedraw;
}

void Container::draw(LinearFrameBuffer &lfb) {
    // Check if the whole container needs to be redrawn
    bool redraw = false;
    for (const auto *child : children) {
        if (child->requiresParentRedraw()) {
            redraw = true;
            break;
        }
    }

    if (redraw) {
        const auto posX = getPosX();
        const auto posY = getPosY();

        // Draw frame
        lfb.drawRectangle(posX, posY, width, height, style.borderColor);

        // Fill background
        lfb.fillRectangle(posX + 1, posY + 1, width - 2, height - 2, style.backgroundColor);

        // Draw children
        for (auto *child : children) {
            child->draw(lfb);
        }
    } else {
        // Only draw children that need to be redrawn
        for (auto *child : children) {
            if (child->requiresRedraw()) {
                child->draw(lfb);
            }
        }
    }

    Widget::draw(lfb);
}

Widget* Container::getChildAtPoint(const size_t posX, const size_t posY) {
    for (auto *child : children) {
        auto *widget = child->getChildAtPoint(posX, posY);
        if (widget != nullptr) {
            return widget;
        }
    }

    if (containsPoint(posX, posY)) {
        return this;
    }

    return nullptr;
}

}
