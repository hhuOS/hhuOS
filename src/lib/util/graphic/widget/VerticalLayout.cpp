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

#include "VerticalLayout.h"

#include "graphic/widget/Container.h"

namespace Util::Graphic {

VerticalLayout::VerticalLayout(const size_t spacing) : spacing(spacing) {}

void VerticalLayout::arrangeWidgets(const ArrayList<WidgetEntry> &widgets) const {
    const auto width = getContainer().getWidth();
    const auto posX = getContainer().getPosX();

    size_t widgetHeightSum = 0;
    for (auto &entry : widgets) {
        widgetHeightSum += entry.widget->getHeight() + spacing;
    }
    widgetHeightSum -= spacing;

    auto posY = getContainer().getPosY() + (getContainer().getHeight() - widgetHeightSum + spacing) / 2;
    for (auto &entry : widgets) {
        auto &widget = *entry.widget;

        if (widget.isContainer()) {
            auto &container = reinterpret_cast<Container&>(widget);
            container.width = width;
            container.height = widgetHeightSum;
            container.rearrangeChildren();
        }

        const auto widgetPosX = posX + (width - widget.getWidth()) / 2;
        widget.setPosition(widgetPosX, posY);

        if (widget.isContainer()) {
            auto &container = reinterpret_cast<Container&>(widget);
            container.rearrangeChildren();
        }

        posY += widget.getHeight() + spacing;
    }
}

}
