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

#include "GridLayout.h"

#include "lunar/Container.h"

namespace Lunar {

GridLayout::GridLayout(const size_t rows, const size_t columns, const size_t verticalGap, const size_t horizontalGap) :
    rows(rows), columns(columns), verticalGap(verticalGap), horizontalGap(horizontalGap)
{
    if (rows == 0 && columns == 0) {
        Util::Panic::fire(Util::Panic::INVALID_ARGUMENT, "GridLayout: Rows and columns cannot be 0 at the same time!");
    }
}

void GridLayout::arrangeWidgets(const Util::ArrayList<WidgetEntry> &widgets) const {
    const auto containerPosX = getContainer().getPosX();
    const auto containerPosY = getContainer().getPosY();
    const auto containerWidth = getContainer().getWidth();
    const auto containerHeight = getContainer().getHeight();

    const auto rows = GridLayout::rows == 0 ? widgets.size() : GridLayout::rows;
    const auto columns = GridLayout::columns == 0 ? widgets.size() : GridLayout::columns;
    const auto cellWidth = containerWidth / columns - horizontalGap * (columns - 1) / 2;
    const auto cellHeight = containerHeight / rows - verticalGap * (rows - 1) / 2;
    const auto lastCellWidth = cellWidth + containerWidth - columns * cellWidth;
    const auto lastCellHeight = cellHeight + containerHeight - rows * cellHeight;

    for (size_t i = 0; i < widgets.size() && i < rows * columns; i++) {
        const auto row = i / columns;
        const auto column = i % columns;

        auto &widget = *widgets.get(i).widget;
        const auto width = column == columns - 1 ? lastCellWidth : cellWidth;
        const auto height = row == rows - 1 ? lastCellHeight : cellHeight;

        widget.setSize(width, height);
        widget.setPosition(containerPosX + column * (cellWidth + horizontalGap),
            containerPosY + row * (cellHeight + verticalGap));
    }
}

size_t GridLayout::getPreferredWidth(const Util::ArrayList<WidgetEntry> &widgets) const {
    const auto columns = GridLayout::columns == 0 ? widgets.size() : GridLayout::columns;

    size_t maxWidth = 0;
    for (const auto &entry : widgets) {
        const auto widgetWidth = entry.widget->getPreferredWidth();
        if (widgetWidth > maxWidth) {
            maxWidth = widgetWidth;
        }
    }

    return maxWidth * columns + horizontalGap * (columns - 1);
}

size_t GridLayout::getPreferredHeight(const Util::ArrayList<WidgetEntry> &widgets) const {
    const auto rows = GridLayout::rows == 0 ? widgets.size() : GridLayout::rows;

    size_t maxHeight = 0;
    for (const auto &entry : widgets) {
        const auto widgetHeight = entry.widget->getPreferredHeight();
        if (widgetHeight > maxHeight) {
            maxHeight = widgetHeight;
        }
    }

    return maxHeight * rows + verticalGap * (rows - 1);
}

}
