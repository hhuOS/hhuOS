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
 */

#ifndef HHUOS_LIB_LUNAR_BORDERLAYOUT_H
#define HHUOS_LIB_LUNAR_BORDERLAYOUT_H

#include "lunar/Layout.h"

namespace Lunar {

/// A layout that arranges widgets in five regions: north, south, east, west, and center.
/// The north and south regions span the full width of the container at the top and bottom, respectively.
/// The east and west regions are placed on the right and left sides of the container, respectively,
/// taking up the remaining vertical space between the north and south regions.
/// The center region occupies the remaining space in the middle of the container.
/// The preferred size of the layout is determined by the sum of the preferred sizes of its child
/// widgets in each region.
/// Each region can hold at most one widget. If multiple widgets are assigned to the same region,
/// a panic is fired during arrangement.
class BorderLayout final : public Layout {

public:
    /// Possible positions for widgets in the border layout.
    enum Position {
        /// Widget is placed at the top of the container.
        NORTH,
        /// Widget is placed at the bottom of the container.
        SOUTH,
        /// Widget is placed on the right side of the container.
        EAST,
        /// Widget is placed on the left side of the container.
        WEST,
        /// Widget is placed in the center of the container.
        CENTER
    };

    /// Arrange the given widgets within the container according to the layout's rules.
    /// This sets each widget's position and size based on its assigned region in the border layout.
    void arrangeWidgets(const Util::ArrayList<WidgetEntry> &widgets) const override;

    /// Get the preferred width of the layout in pixels.
    /// The preferred width is determined by the maximum preferred widths of the west, center, and east widgets.
    size_t getPreferredWidth(const Util::ArrayList<WidgetEntry> &widgets) const override;

    /// Get the preferred height of the layout in pixels.
    /// The preferred height is determined by the sum of the preferred heights of the north, center, and south widgets.
    size_t getPreferredHeight(const Util::ArrayList<WidgetEntry> &widgets) const override;
};

}

#endif