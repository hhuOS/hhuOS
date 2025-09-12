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

#include "graphic/widget/Container.h"

namespace Util::Graphic {

BorderLayout::BorderLayout() {}

void BorderLayout::arrangeWidgets(const ArrayList<WidgetEntry> &widgets) const {
    size_t northWidgets = 0;
    size_t southWidgets = 0;
    size_t westWidgets = 0;
    size_t eastWidgets = 0;
    size_t centerWidgets = 0;

    for (auto &entry : widgets) {
        if (entry.args.length() == 0) {
            Panic::fire(Panic::INVALID_ARGUMENT, "BorderLayout: No position argument given!");
        }

        const auto position = static_cast<Position>(entry.args[0]);
        switch (position) {
            case NORTH:
                northWidgets++;
                break;
            case SOUTH:
                southWidgets++;
                break;
            case EAST:
                eastWidgets++;
                break;
            case WEST:
                westWidgets++;
                break;
            case CENTER:
                centerWidgets++;
                break;
            default:
                Panic::fire(Panic::INVALID_ARGUMENT, "BorderLayout: Invalid position argument!");
        }
    }

    if (northWidgets > 1 || southWidgets > 1 || westWidgets > 1 || eastWidgets > 1 || centerWidgets > 1) {
        Panic::fire(Panic::INVALID_ARGUMENT, "BorderLayout: More than one widget assigned to a position!");
    }

    const auto width = getContainer().getWidth();
    const auto height = getContainer().getHeight();
    const auto posX = getContainer().getPosX();
    const auto posY = getContainer().getPosY();

    const auto borderHeight = width * (100 - CENTER_HEIGHT_PERCENTAGE) / 200;
    const auto borderWidth = width * (100 - CENTER_WIDTH_PERCENTAGE) / 200;

    const auto northHeight = northWidgets ? borderHeight : 0;
    const auto southHeight = southWidgets ? borderHeight : 0;
    const auto westWidth = westWidgets ? borderWidth : 0;
    const auto westHeight = height - northHeight - southHeight;
    const auto eastWidth = eastWidgets ? borderWidth : 0;
    const auto eastHeight = height - northHeight - southHeight;
    const auto centerHeight = height - northHeight - southHeight;
    const auto centerWidth = width - westWidth - eastWidth;

    for (auto &entry : widgets) {
        auto &widget = *entry.widget;
        const auto position = static_cast<Position>(entry.args[0]);

        switch (position) {
            case NORTH: {
                if (widget.isContainer()) {
                    auto &container = reinterpret_cast<Container&>(widget);
                    container.width = width;
                    container.height = northHeight;
                    container.rearrangeChildren();
                }

                widget.setPosition(posX + (width - widget.getWidth()) / 2,
                    posY + (northHeight - widget.getHeight()) / 2);
                
                if (widget.isContainer()) {
                    auto &container = reinterpret_cast<Container&>(widget);
                    container.rearrangeChildren();
                }
                break;
            }
            case SOUTH: {
                if (widget.isContainer()) {
                    auto &container = reinterpret_cast<Container&>(widget);
                    container.width = width;
                    container.height = southHeight;
                }

                const auto southPosY = posY + height - southHeight;
                widget.setPosition(posX + (width - widget.getWidth()) / 2,
                    southPosY + (southHeight - widget.getHeight()) / 2);
                
                if (widget.isContainer()) {
                    auto &container = reinterpret_cast<Container&>(widget);
                    container.rearrangeChildren();
                }
                break;
            }
            case EAST: {
                if (widget.isContainer()) {
                    auto &container = reinterpret_cast<Container&>(widget);
                    container.width = eastWidth;
                    container.height = eastHeight;
                }

                const auto eastPosX = posX + width - eastWidth;
                widget.setPosition(eastPosX + (eastWidth - widget.getWidth()) / 2,
                    posY + (height - widget.getHeight()) / 2);
                
                if (widget.isContainer()) {
                    auto &container = reinterpret_cast<Container&>(widget);
                    container.rearrangeChildren();
                }
                break;
            }
            case WEST: {
                if (widget.isContainer()) {
                    auto &container = reinterpret_cast<Container&>(widget);
                    container.width = westWidth;
                    container.height = westHeight;
                }

                widget.setPosition(posX + (westWidth - widget.getWidth()) / 2,
                    posY + (height - widget.getHeight()) / 2);
                
                if (widget.isContainer()) {
                    auto &container = reinterpret_cast<Container&>(widget);
                    container.rearrangeChildren();
                }
                break;
            }
            case CENTER: {
                if (widget.isContainer()) {
                    auto &container = reinterpret_cast<Container&>(widget);
                    container.width = centerWidth;
                    container.height = centerHeight;
                }

                const auto centerPosX = posX + westWidth;
                const auto centerPosY = posY + northHeight;
                widget.setPosition(centerPosX + (centerWidth - widget.getWidth()) / 2,
                    centerPosY + (centerHeight - widget.getHeight()) / 2);
                
                if (widget.isContainer()) {
                    auto &container = reinterpret_cast<Container&>(widget);
                    container.rearrangeChildren();
                }
                break;
            }
        }
    }
}

}
