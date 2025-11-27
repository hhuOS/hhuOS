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

#ifndef HHUOS_LIB_UTIL_GRAPHIC_WIDGET_LAYOUT_H
#define HHUOS_LIB_UTIL_GRAPHIC_WIDGET_LAYOUT_H

#include "util/collection/ArrayList.h"
#include "lunar/Widget.h"

namespace Lunar {

class Container;

class Layout {

public:

    struct WidgetEntry {
        Widget *widget;
        Util::Array<size_t> args;

        bool operator!=(const WidgetEntry &other) const;
    };

    Layout() = default;

    Layout(const Layout &other) = delete;

    Layout& operator=(const Layout &other) = delete;

    virtual ~Layout() = default;

    virtual void arrangeWidgets(const Util::ArrayList<WidgetEntry>& widgets) const = 0;

    [[nodiscard]] virtual size_t getPreferredWidth(const Util::ArrayList<WidgetEntry> &widgets) const = 0;

    [[nodiscard]] virtual size_t getPreferredHeight(const Util::ArrayList<WidgetEntry> &widgets) const = 0;

protected:

    [[nodiscard]] const Container& getContainer() const;

private:

    friend class Container;

    const Container *container = nullptr;
};

}

#endif