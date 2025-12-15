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

#ifndef HHUOS_LIB_LUNAR_CONTAINER_H
#define HHUOS_LIB_LUNAR_CONTAINER_H

#include <stddef.h>

#include "util/collection/ArrayList.h"
#include "util/graphic/LinearFrameBuffer.h"
#include "lunar/Layout.h"
#include "lunar/Widget.h"

namespace Lunar {

/// A container is a special type of widget that can contain other widgets.
/// Containers use layouts to arrange their child widgets automatically.
/// This allows for complex UI hierarchies, resulting in a tree structure of widgets.
/// The size of a container is determined by its layout and the preferred sizes of its child widgets.
class Container : public Widget {

public:
    /// Create a new container instance.
    Container() : Widget(false, false) {}

    /// Destroy the container instance, deleting the associated layout.
    ~Container() override;

    /// Set the layout for this container.
    /// The layout must be allocated on the heap and the container takes ownership of it.
    /// This means that the container will delete the layout when it is destroyed.
    /// If the container already has a layout, it will be deleted before setting the new layout.
    void setLayout(Layout *layout);

    /// Add a child widget to the container with optional layout arguments.
    /// The layout arguments are passed to the layout when arranging the child widgets.
    /// The number and meaning of the layout arguments depend on the specific layout used.
    /// The widget must be allocated on the heap and the container takes ownership of it.
    /// This means that the container will delete the widget when it is destroyed.
    void addChild(Widget *widget, const Util::Array<size_t> &layoutArgs = Util::Array<size_t>());

    /// Get the preferred width of the container in pixels.
    /// The preferred width is determined by the layout and the preferred sizes of the child widgets.
    size_t getPreferredWidth() const override {
        return layout == nullptr ? 0 : layout->getPreferredWidth(children);
    }

    /// Get the preferred height of the container in pixels.
    /// The preferred height is determined by the layout and the preferred sizes of the child widgets.
    size_t getPreferredHeight() const override {
        return layout == nullptr ? 0 : layout->getPreferredHeight(children);
    }

    /// Check whether the container or any of its child widgets need to be redrawn.
    bool requiresRedraw() const override;

    /// Get the child widget at the given point (posX, posY) in absolute pixel coordinates.
    /// The appropriate child widget is searched recursively, meaning that if a child widget is itself
    /// a container, its children are also searched.
    Widget* getChildAtPoint(size_t posX, size_t posY) override;

    /// Draw the container and its child widgets on the given linear frame buffer.
    void draw(const Util::Graphic::LinearFrameBuffer &lfb) override;

protected:
    /// Create a new container instance with the given size.
    /// This constructor is needed to create a root container with a specific size.
    Container(const size_t width, const size_t height) : Container() {
        Widget::setSize(width, height);
    }

private:

    void rearrangeChildren() override;

    friend class Widget;

    Layout *layout = nullptr;
    Util::ArrayList<Layout::WidgetEntry> children;
};

}

#endif