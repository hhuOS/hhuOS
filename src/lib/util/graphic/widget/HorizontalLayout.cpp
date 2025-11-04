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

#include "HorizontalLayout.h"

#include "util/graphic/widget/Container.h"

namespace Util::Graphic {

HorizontalLayout::HorizontalLayout(const size_t spacing) : spacing(spacing) {}

void HorizontalLayout::arrangeWidgets(const ArrayList<WidgetEntry> &widgets) const {
    const auto containerPosX = getContainer().getPosX();
    const auto containerPosY = getContainer().getPosY();
    const auto containerWidth = getContainer().getWidth();
    const auto containerHeight = getContainer().getHeight();

    size_t widgetWidthSum = 0;
    for (auto &entry : widgets) {
        auto &widget = *entry.widget;

        const auto preferredWidth = widget.getPreferredWidth();
        const auto preferredHeight = widget.getPreferredHeight();
        widget.setSize(preferredWidth, preferredHeight > containerHeight ? containerHeight : preferredHeight);

        widgetWidthSum += widget.getWidth() + spacing;
    }
    widgetWidthSum = widgetWidthSum > 0 ? widgetWidthSum - spacing : 0;

    auto posX = getContainer().getPosX() + (getContainer().getWidth() - widgetWidthSum) / 2;
    for (auto &entry : widgets) {
        auto &widget = *entry.widget;

        // Not enough space to draw further widgets
        if (posX + widget.getWidth() > containerPosX + containerWidth) {
            widget.setSize(0, 0);
            break;
        }

        // Adapt widget height if it is too high
        if (widget.height > containerHeight) {
            widget.setSize(widget.getWidth(), containerHeight);
        }

        // Set widget position
        widget.setPosition(posX, containerPosY + (containerHeight - widget.getHeight()) / 2);

        posX += widget.getWidth() + spacing;
    }
}

size_t HorizontalLayout::getPreferredWidth(const ArrayList<WidgetEntry> &widgets) const {
    size_t widgetWidthSum = 0;
    for (const auto &entry : widgets) {
        widgetWidthSum += entry.widget->getPreferredWidth() + spacing;
    }

    return widgetWidthSum > 0 ? widgetWidthSum - spacing : 0;
}

size_t HorizontalLayout::getPreferredHeight(const ArrayList<WidgetEntry> &widgets) const {
    size_t maxHeight = 0;
    for (const auto &entry : widgets) {
        const auto widgetHeight = entry.widget->getPreferredHeight();
        if (widgetHeight > maxHeight) {
            maxHeight = widgetHeight;
        }
    }

    return maxHeight;
}

}
