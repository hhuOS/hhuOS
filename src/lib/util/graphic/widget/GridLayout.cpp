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

#include "GridLayout.h"

#include "graphic/widget/Container.h"

namespace Util::Graphic {

GridLayout::GridLayout(const size_t rows, const size_t columns, const size_t verticalGap, const size_t horizontalGap) :
    rows(rows), columns(columns), verticalGap(verticalGap), horizontalGap(horizontalGap)
{
    if (rows == 0 && columns == 0) {
        Panic::fire(Panic::INVALID_ARGUMENT, "GridLayout: Rows and columns cannot be 0 at the same time!");
    }
}

void GridLayout::arrangeWidgets(const ArrayList<WidgetEntry> &widgets) const {
    const auto width = getContainer().getWidth();
    const auto height = getContainer().getHeight();
    const auto containerPosX = getContainer().getPosX();
    const auto containerPosY = getContainer().getPosY();

    const auto rows = GridLayout::rows == 0 ? widgets.size() : GridLayout::rows;
    const auto columns = GridLayout::columns == 0 ? widgets.size() : GridLayout::columns;
    const auto cellWidth = width / rows - horizontalGap * (columns - 1) / 2;
    const auto cellHeight = height / rows - verticalGap * (rows - 1) / 2;

    for (size_t i = 0; i < widgets.size() && i < rows * columns; i++) {
        const auto row = i / columns;
        const auto column = i % columns;

        auto &widget = *widgets.get(i).widget;
        if (widget.isContainer()) {
            auto &container = reinterpret_cast<Container&>(widget);
            container.width = cellWidth;
            container.height = cellHeight;
            container.rearrangeChildren();
        }

        const auto widgetPosX = containerPosX + column * (cellWidth + horizontalGap)
            + (cellWidth - widget.getWidth()) / 2;
        const auto widgetPosY = containerPosY + row * (cellHeight + verticalGap)
            + (cellHeight - widget.getHeight()) / 2;

        widget.setPosition(widgetPosX, widgetPosY);

        if (widget.isContainer()) {
            auto &container = reinterpret_cast<Container&>(widget);
            container.rearrangeChildren();
        }
    }
}

}
