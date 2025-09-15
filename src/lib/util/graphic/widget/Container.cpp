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

#include "graphic/widget/Theme.h"

namespace Util::Graphic {

Container::Container() : Widget( false, false) {}

Container::Container(const size_t width, const size_t height) : Container() {
    Widget::setSize(width, height);
}

Container::~Container() {
    delete layout;
}

void Container::setLayout(Layout *layout) {
    delete Container::layout;
    Container::layout = layout;

    layout->container = this;
}

void Container::addChild(Widget &widget, const Array<size_t> &layoutArgs) {
    children.add(Layout::WidgetEntry{&widget, layoutArgs});
    widget.parent = this;

    rearrangeChildren();
}

bool Container::requiresRedraw() const {
    bool childNeedsRedraw = false;
    for (const auto &child : children) {
        if (child.widget->requiresRedraw()) {
            childNeedsRedraw = true;
            break;
        }
    }

    return Widget::requiresRedraw() || childNeedsRedraw;
}

void Container::draw(const LinearFrameBuffer &lfb) {
    const auto &style = Theme::CURRENT_THEME.container().getStyle(*this);

    if (Widget::requiresRedraw()) {
        // Redraw whole container
        const auto posX = getPosX();
        const auto posY = getPosY();

        // Fill background
        lfb.fillRectangle(posX, posY, width, height, style.widgetColor);

        // Draw frame
        lfb.drawRectangle(posX, posY, width, height, style.borderColor);

        // Draw children
        for (const auto &child : children) {
            child.widget->draw(lfb);
        }
    } else {
        // Only draw children that need to be redrawn
        for (const auto &child : children) {
            if (child.widget->requiresRedraw()) {
                child.widget->draw(lfb);
            }
        }
    }

    Widget::draw(lfb);
}

Widget* Container::getChildAtPoint(const size_t posX, const size_t posY) {
    for (auto &child : children) {
        auto *widget = child.widget->getChildAtPoint(posX, posY);
        if (widget != nullptr) {
            return widget;
        }
    }

    if (containsPoint(posX, posY)) {
        return this;
    }

    return nullptr;
}

void Container::rearrangeChildren() {
    if (layout != nullptr) {
        layout->arrangeWidgets(children);
    }
}

}
