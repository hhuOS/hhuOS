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

#include "lunar/Container.h"

namespace Lunar {

Widget::Widget(const bool redrawOnMouseStatusChange, const bool redrawOnFocusChange) :
    redrawOnMouseStatusChange(redrawOnMouseStatusChange), redrawOnFocusChange(redrawOnFocusChange)
{
    addActionListener(new MouseListener(*this));
}

Widget::~Widget() {
    for (const auto *listener : actionListeners) {
        delete listener;
    }
}

void Widget::reportPreferredSizeChange() const {
    if (parent != nullptr) {
        parent->rearrangeChildren();
    }
}

void Widget::mouseClicked() const {
    for (auto *listener : actionListeners) {
        listener->onMouseClicked();
    }
}

void Widget::mousePressed() const {
    for (auto *listener : actionListeners) {
        listener->onMousePressed();
    }
}

void Widget::mouseReleased() const {
    for (auto *listener : actionListeners) {
        listener->onMouseReleased();
    }
}

void Widget::mouseEntered() const {
    for (auto *listener : actionListeners) {
        listener->onMouseEntered();
    }
}

void Widget::mouseExited() const {
    for (auto *listener : actionListeners) {
        listener->onMouseExited();
    }
}

void Widget::keyPressed(const Util::Io::Key &key) const {
    for (auto *listener : actionListeners) {
        listener->onKeyPressed(key);
    }
}

void Widget::keyReleased(const Util::Io::Key &key) const {
    for (auto *listener : actionListeners) {
        listener->onKeyReleased(key);
    }
}

void Widget::keyTyped(const Util::Io::Key &key) const {
    for (auto *listener : actionListeners) {
        listener->onKeyTyped(key);
    }
}

void Widget::setSize(const size_t width, const size_t height) {
    const auto oldWidth = Widget::width;
    const auto oldHeight = Widget::height;

    Widget::width = width;
    Widget::height = height;

    if (oldWidth != width || oldHeight != height) {
        rearrangeChildren();
        requireRedraw();
    }
}

void Widget::setFocused(const bool focused) {
    Widget::focused = focused;
    requireRedraw();
}

Widget* Widget::getChildAtPoint(const size_t posX, const size_t posY) {
    if (containsPoint(posX, posY)) {
        return this;
    }

    return nullptr;
}

void Widget::setPosition(const size_t x, const size_t y) {
    const auto oldPosX = posX;
    const auto oldPosY = posY;

    posX = x;
    posY = y;

    if (oldPosX != x || oldPosY != y) {
        rearrangeChildren();
        requireRedraw();
    }
}

void Widget::MouseListener::onMouseEntered() {
    widget.hovered = true;

    if (widget.redrawOnMouseStatusChange) {
        widget.requireRedraw();
    }
}

void Widget::MouseListener::onMouseExited() {
    widget.hovered = false;

    if (widget.redrawOnMouseStatusChange) {
        widget.requireRedraw();
    }
}

void Widget::MouseListener::onMousePressed() {
    widget.pressed = true;

    if (widget.redrawOnMouseStatusChange) {
        widget.requireRedraw();
    }
}

void Widget::MouseListener::onMouseReleased() {
    widget.pressed = false;

    if (widget.redrawOnMouseStatusChange) {
        widget.requireRedraw();
    }
}

}
