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

#ifndef HHUOS_LIB_LUNAR_HORIZONTALLAYOUT_H
#define HHUOS_LIB_LUNAR_HORIZONTALLAYOUT_H

#include "lunar/Layout.h"

namespace Lunar {

/// A layout that arranges widgets horizontally in a single row.
/// Widgets are placed next to each other from left to right, with an optional spacing between them.
/// The preferred size of the layout is determined by the sum of the preferred widths of its child widgets,
/// plus the spacing between them, and the maximum preferred height of its child widgets.
class HorizontalLayout final : public Layout {

public:
    /// Create a new horizontal layout instance with the given spacing between widgets.
    explicit HorizontalLayout(const size_t spacing = 0) : spacing(spacing) {}

    /// Arrange the given widgets within the container according to the layout's rules.
    /// This sets each widget's position next to the previous widget, with the specified spacing in between.
    void arrangeWidgets(const Util::ArrayList<WidgetEntry> &widgets) const override;

    /// Get the preferred width of the layout in pixels.
    /// The preferred width is determined by the sum of the preferred widths of its child widgets,
    /// plus the spacing between them.
    size_t getPreferredWidth(const Util::ArrayList<WidgetEntry> &widgets) const override;

    /// Get the preferred height of the layout in pixels.
    /// The preferred height is determined by the maximum preferred height of its child widgets.
    size_t getPreferredHeight(const Util::ArrayList<WidgetEntry> &widgets) const override;

private:

    const size_t spacing;
};

}

#endif