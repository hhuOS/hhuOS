/*
 * Copyright (C) 2017-2026 Heinrich Heine University Düsseldorf,
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

#ifndef HHUOS_LIB_LUNAR_LAYOUT_H
#define HHUOS_LIB_LUNAR_LAYOUT_H

#include "util/collection/ArrayList.h"
#include "lunar/Widget.h"

namespace Lunar {
class Container;
}

namespace Lunar {

/// Base class for all layouts.
/// A layout is responsible for arranging widgets within a container.
/// Layouts determine the position and size of each widget based on the
/// available space and the preferred size of the widgets.
class Layout {

public:
    /// When adding a widget to a container via `Container::addChild()`, an arbitrary list of
    /// arguments can be passed to specify layout-specific parameters for the widget.
    /// The container stores these arguments in a `WidgetEntry` structure along with a pointer to the widget.
    struct WidgetEntry {
        /// A pointer to the widget added to the container and arranged by the layout.
        Widget *widget;
        /// An array of layout-specific arguments provided when adding the widget to the container.
        Util::Array<size_t> args;

        /// Compare this widget entry to another one for inequality.
        bool operator!=(const WidgetEntry &other) const {
            return widget != other.widget;
        }
    };

    /// Create a new layout instance.
    Layout() = default;

    /// Destroy the layout instance.
    virtual ~Layout() = default;

    /// Arrange the given widgets within the container according to the layout's rules.
    virtual void arrangeWidgets(const Util::ArrayList<WidgetEntry>& widgets) const = 0;

    /// Get the preferred width of the container this layout is assigned to,
    /// based on the preferred sizes of the given widgets and the layout's rules.
    virtual size_t getPreferredWidth(const Util::ArrayList<WidgetEntry> &widgets) const = 0;

    /// Get the preferred height of the container this layout is assigned to,
    /// based on the preferred sizes of the given widgets and the layout's rules.
    virtual size_t getPreferredHeight(const Util::ArrayList<WidgetEntry> &widgets) const = 0;

protected:
    /// Get the container this layout is assigned to.
    const Container& getContainer() const {
        if (container == nullptr) {
            Util::Panic::fire(Util::Panic::ILLEGAL_STATE, "Layout: Not assigned to any container!");
        }

        return *container;
    }

private:

    friend class Container;

    const Container *container = nullptr;
};

}

#endif