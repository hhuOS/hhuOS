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

#include "BorderLayout.h"

#include "util/graphic/widget/Container.h"

namespace Util::Graphic {

void BorderLayout::arrangeWidgets(const ArrayList<WidgetEntry> &widgets) const {
    Widget *northWidget = nullptr;
    Widget *southWidget = nullptr;
    Widget *eastWidget = nullptr;
    Widget *westWidget = nullptr;
    Widget *centerWidget = nullptr;

    for (auto &entry : widgets) {
        if (entry.args.length() == 0) {
            Panic::fire(Panic::INVALID_ARGUMENT, "BorderLayout: No position argument given!");
        }

        const auto position = static_cast<Position>(entry.args[0]);
        switch (position) {
            case NORTH:
                if (northWidget != nullptr) {
                    Panic::fire(Panic::INVALID_ARGUMENT, "BorderLayout: Multiple NORTH widgets given!");
                }
                northWidget = entry.widget;
                break;
            case SOUTH:
                if (southWidget != nullptr) {
                    Panic::fire(Panic::INVALID_ARGUMENT, "BorderLayout: Multiple SOUTH widgets given!");
                }
                southWidget = entry.widget;
                break;
            case EAST:
                if (eastWidget != nullptr) {
                    Panic::fire(Panic::INVALID_ARGUMENT, "BorderLayout: Multiple EAST widgets given!");
                }
                eastWidget = entry.widget;
                break;
            case WEST:
                if (westWidget != nullptr) {
                    Panic::fire(Panic::INVALID_ARGUMENT, "BorderLayout: Multiple WEST widgets given!");
                }
                westWidget = entry.widget;
                break;
            case CENTER:
                if (centerWidget != nullptr) {
                    Panic::fire(Panic::INVALID_ARGUMENT, "BorderLayout: Multiple CENTER widgets given!");
                }
                centerWidget = entry.widget;
                break;
            default:
                Panic::fire(Panic::INVALID_ARGUMENT, "BorderLayout: Invalid position argument!");
        }
    }

    const auto containerPosX = getContainer().getPosX();
    const auto containerPosY = getContainer().getPosY();
    const auto containerWidth = getContainer().getWidth();
    const auto containerHeight = getContainer().getHeight();

    if (northWidget != nullptr) {
        northWidget->setPosition(containerPosX, containerPosY);
        northWidget->setSize(containerWidth, northWidget->getPreferredHeight());
    }
    if (southWidget != nullptr) {
        const auto widgetHeight = southWidget->getPreferredHeight();
        southWidget->setPosition(containerPosX, containerPosY + containerHeight - widgetHeight);
        southWidget->setSize(containerWidth, widgetHeight);
    }
    if (eastWidget != nullptr) {
        const auto widgetWidth = eastWidget->getPreferredWidth();
        const auto widgetHeight = containerHeight
            - (northWidget ? northWidget->getHeight() : 0)
            - (southWidget ? southWidget->getHeight() : 0);
        const auto widgetPosY = containerPosY + (northWidget ? northWidget->getHeight() : 0);
        eastWidget->setPosition(containerPosX + containerWidth - widgetWidth, widgetPosY);
        eastWidget->setSize(eastWidget->getPreferredWidth(), widgetHeight);
    }
    if (westWidget != nullptr) {
        const auto widgetHeight = containerHeight
            - (northWidget ? northWidget->getHeight() : 0)
            - (southWidget ? southWidget->getHeight() : 0);
        const auto widgetPosY = containerPosY + (northWidget ? northWidget->getHeight() : 0);
        westWidget->setPosition(containerPosX, widgetPosY);
        westWidget->setSize(westWidget->getPreferredWidth(), widgetHeight);
    }
    if (centerWidget != nullptr) {
        const auto widgetWidth = containerWidth
            - (eastWidget ? eastWidget->getWidth() : 0)
            - (westWidget ? westWidget->getWidth() : 0);
        const auto widgetHeight = containerHeight
            - (northWidget ? northWidget->getHeight() : 0)
            - (southWidget ? southWidget->getHeight() : 0);
        const auto widgetPosX = containerPosX + (westWidget ? westWidget->getWidth() : 0);
        const auto widgetPosY = containerPosY + (northWidget ? northWidget->getHeight() : 0);
        centerWidget->setPosition(widgetPosX, widgetPosY);
        centerWidget->setSize(widgetWidth, widgetHeight);
    }
}

size_t BorderLayout::getPreferredWidth(const ArrayList<WidgetEntry> &widgets) const {
    size_t northWidth = 0;
    size_t southWidth = 0;
    size_t centerWidth = 0;

    for (const auto &entry : widgets) {
        const auto position = static_cast<Position>(entry.args[0]);
        switch (position) {
            case NORTH:
                northWidth = entry.widget->getPreferredWidth();
                break;
            case SOUTH:
                southWidth = entry.widget->getPreferredWidth();
                break;
            case EAST:
            case WEST:
            case CENTER:
                centerWidth += entry.widget->getPreferredWidth();
                break;
            default:
                Panic::fire(Panic::INVALID_ARGUMENT, "BorderLayout: Invalid position argument!");
        }
    }

    return northWidth > southWidth ?
        northWidth > centerWidth ? northWidth : centerWidth :
        southWidth > centerWidth ? southWidth : centerWidth;
}

size_t BorderLayout::getPreferredHeight(const ArrayList<WidgetEntry> &widgets) const {
    size_t northHeight = 0;
    size_t southHeight = 0;
    size_t westHeight = 0;
    size_t eastHeight = 0;
    size_t centerHeight = 0;

    for (const auto &entry : widgets) {
        const auto position = static_cast<Position>(entry.args[0]);
        switch (position) {
            case NORTH:
                northHeight = entry.widget->getPreferredHeight();
                break;
            case SOUTH:
                southHeight = entry.widget->getPreferredHeight();
                break;
            case EAST:
                eastHeight = entry.widget->getPreferredHeight();
                break;
            case WEST:
                westHeight = entry.widget->getPreferredHeight();
                break;
            case CENTER:
                centerHeight += entry.widget->getPreferredHeight();
                break;
            default:
                Panic::fire(Panic::INVALID_ARGUMENT, "BorderLayout: Invalid position argument!");
        }
    }

    return northHeight + southHeight +
        (centerHeight > westHeight ? centerHeight : westHeight > eastHeight ? westHeight : eastHeight);
}

}
