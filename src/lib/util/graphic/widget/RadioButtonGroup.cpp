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

#include "RadioButtonGroup.h"

namespace Util::Graphic {

void RadioButtonGroup::add(RadioButton &radioButton) {
    buttons.add(&radioButton);

    radioButton.group = this;
    radioButton.groupIndex = static_cast<int32_t>(buttons.size() - 1);
}

void RadioButtonGroup::select(const int32_t index) {
    if (index < 0 || static_cast<size_t>(index) >= buttons.size()) {
        Panic::fire(Panic::OUT_OF_BOUNDS, "RadioButtonGroup: Index out of bounds!");
    }

    if (index == selectedIndex) {
        return;
    }

    if (selectedIndex >= 0) {
        auto &oldButton = *buttons.get(selectedIndex);
        oldButton.selected = false;
        oldButton.requireRedraw();
    }

    selectedIndex = index;

    auto &newButton = *buttons.get(selectedIndex);
    newButton.selected = true;
    newButton.requireRedraw();
}

const RadioButton* RadioButtonGroup::getSelectedButton() const {
    return selectedIndex >= 0 ? buttons.get(selectedIndex) : nullptr;
}

int32_t RadioButtonGroup::getSelectedIndex() const {
    return selectedIndex;
}

}
