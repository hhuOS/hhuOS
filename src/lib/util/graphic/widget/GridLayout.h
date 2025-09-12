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

#ifndef HHUOS_LIB_UTIL_GRAPHIC_WIDGET_GRIDLAYOUT_H
#define HHUOS_LIB_UTIL_GRAPHIC_WIDGET_GRIDLAYOUT_H

#include "graphic/widget/Layout.h"

namespace Util::Graphic {

class GridLayout final : public Layout {

public:

    explicit GridLayout(size_t rows, size_t columns, size_t verticalGap = 0, size_t horizontalGap = 0);

    void arrangeWidgets(const ArrayList<WidgetEntry> &widgets) const override;

private:

    const size_t rows;
    const size_t columns;

    const size_t verticalGap;
    const size_t horizontalGap;
};

}

#endif