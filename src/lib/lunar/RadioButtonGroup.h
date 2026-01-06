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
 *
 * The widget and layout system is based on a bachelor's thesis, written by Michael Zuchniewski.
 * The original source code can be found here: https://git.hhu.de/bsinfo/thesis/ba-mizuc100
 */

#ifndef HHUOS_LIB_LUNAR_RADIOBUTTONGROUP_H
#define HHUOS_LIB_LUNAR_RADIOBUTTONGROUP_H

#include "util/collection/ArrayList.h"
#include "lunar/RadioButton.h"

namespace Lunar {

/// A radio button group manages a set of `RadioButton` widgets, ensuring that only one radio button in the group
/// can be selected at a time. Every time a radio button is selected, the previously selected radio button (if any)
/// is automatically deselected. The group is not a widget itself, but rather a helper class to manage the state
/// of multiple radio buttons.
class RadioButtonGroup {

public:
    /// Add a radio button to the group.
    void add(RadioButton &radioButton);

    /// Select the radio button at the given index in the group.
    void select(int32_t index);

    /// Get the currently selected radio button in the group, or `nullptr` if no button is selected.
    const RadioButton* getSelectedButton() const {
        return selectedIndex >= 0 ? buttons.get(selectedIndex) : nullptr;
    }

    /// Get the index of the currently selected radio button in the group, or -1 if no button is selected.
    int32_t getSelectedIndex() const {
        return selectedIndex;
    }

private:

    Util::ArrayList<RadioButton*> buttons;
    int32_t selectedIndex = -1;
};

}

#endif