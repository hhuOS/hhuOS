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

#ifndef HHUOS_LIB_LUNAR_FREELAYOUT_H
#define HHUOS_LIB_LUNAR_FREELAYOUT_H

#include <stddef.h>

#include "lunar/Layout.h"

namespace Lunar {

/// A layout that uses absolute positioning for its child widgets.
/// The x and y coordinates of each widget must be given explicitly when adding the widget to the container
/// and the size of each widget is not modified by the layout.
/// The preferred size of the layout is determined by the maximum extents of its child widgets.
class FreeLayout final : public Layout {

public:

    /// Arrange the given widgets within the container according to the layout's rules.
    /// This sets each widget's position to the x and y coordinates specified in the layout arguments
    /// when the widget was added to the container.
    void arrangeWidgets(const Util::ArrayList<WidgetEntry> &widgets) const override;

    /// Get the preferred width of the layout in pixels.
    /// The preferred width is determined by the maximum x coordinate plus the width of the corresponding widget.
    size_t getPreferredWidth(const Util::ArrayList<WidgetEntry> &widgets) const override;

    /// Get the preferred height of the layout in pixels.
    /// The preferred height is determined by the maximum y coordinate plus the height of the corresponding widget.
    size_t getPreferredHeight(const Util::ArrayList<WidgetEntry> &widgets) const override;
};

}

#endif