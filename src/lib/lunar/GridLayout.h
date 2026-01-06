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

#ifndef HHUOS_LIB_LUNAR_GRIDLAYOUT_H
#define HHUOS_LIB_LUNAR_GRIDLAYOUT_H

#include "lunar/Layout.h"

namespace Lunar {

/// A layout that arranges widgets in a grid with a fixed number of rows and columns.
/// Widgets are placed in the grid from left to right and top to bottom.
/// The preferred size of the layout is determined by the maximum preferred width and height of its child
/// widgets, multiplied by the number of columns and rows, respectively, plus the gaps between them.
class GridLayout final : public Layout {

public:
    /// Create a new grid layout instance with the given number of rows and columns,
    /// and optional gaps between rows and columns.
    /// If rows is set to 0, the number of rows will be determined by the number of widgets and columns.
    /// If columns is set to 0, the number of columns will be determined by the number of widgets and rows.
    /// If both rows and columns are set to 0, a panic is fired.
    GridLayout(size_t rows, size_t columns, size_t verticalGap = 0, size_t horizontalGap = 0);

    /// Arrange the given widgets within the container according to the layout's rules.
    /// This sets each widget's position and size based on its cell in the grid,
    /// taking into account the gaps between rows and columns.
    void arrangeWidgets(const Util::ArrayList<WidgetEntry> &widgets) const override;

    /// Get the preferred width of the layout in pixels.
    /// The preferred width is determined by the maximum preferred width of its child widgets,
    /// multiplied by the number of columns, plus the gaps between them.
    size_t getPreferredWidth(const Util::ArrayList<WidgetEntry> &widgets) const override;

    /// Get the preferred height of the layout in pixels.
    /// The preferred height is determined by the maximum preferred height of its child widgets,
    /// multiplied by the number of rows, plus the gaps between them.
    size_t getPreferredHeight(const Util::ArrayList<WidgetEntry> &widgets) const override;

private:

    const size_t rows;
    const size_t columns;

    const size_t verticalGap;
    const size_t horizontalGap;
};

}

#endif