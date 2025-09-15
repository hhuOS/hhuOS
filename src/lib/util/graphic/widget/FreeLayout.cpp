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

#include "FreeLayout.h"

#include "InputField.h"
#include "graphic/widget/Container.h"

namespace Util::Graphic {

void FreeLayout::arrangeWidgets(const ArrayList<WidgetEntry> &widgets) const {
    for (const auto &entry : widgets) {
        const auto containerPosX = getContainer().getPosX();
        const auto containerPosY = getContainer().getPosY();
        const auto containerWidth = getContainer().getWidth();
        const auto containerHeight = getContainer().getHeight();
        const auto widgetPosX = containerPosX + entry.args[0];
        const auto widgetPosY = containerPosY + entry.args[1];

        auto &widget = *entry.widget;
        auto widgetWidth = widget.getPreferredWidth();
        auto widgetHeight = widget.getPreferredHeight();

        if (widgetPosX + widgetWidth > containerPosX + containerWidth) {
            widgetWidth = containerPosX + containerWidth - widgetPosX;
        }
        if (widgetPosY + widgetHeight > containerPosY + containerHeight) {
            widgetHeight = containerPosY + containerHeight - widgetPosY;
        }

        widget.setSize(widgetWidth, widgetHeight);
        widget.setPosition(widgetPosX, widgetPosY);
    }
}

size_t FreeLayout::getPreferredWidth(const ArrayList<WidgetEntry> &widgets) const {
    size_t maxPosX = 0;

    for (const auto &entry : widgets) {
        const auto containerPosX = getContainer().getPosX();
        const auto widgetPosX = containerPosX + entry.args[0];
        const auto widgetWidth = entry.widget->getPreferredWidth();

        if (widgetPosX + widgetWidth > maxPosX) {
            maxPosX = widgetPosX + widgetWidth;
        }
    }

    return maxPosX;
}

size_t FreeLayout::getPreferredHeight(const ArrayList<WidgetEntry> &widgets) const {
    size_t maxPosY = 0;

    for (const auto &entry : widgets) {
        const auto containerPosY = getContainer().getPosY();
        const auto widgetPosY = containerPosY + entry.args[1];
        const auto widgetHeight = entry.widget->getPreferredHeight();

        if (widgetPosY + widgetHeight > maxPosY) {
            maxPosY = widgetPosY + widgetHeight;
        }
    }

    return maxPosY;
}

}
