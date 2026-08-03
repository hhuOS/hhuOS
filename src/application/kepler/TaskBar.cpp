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
 */

#include "TaskBar.h"

#include "TaskBarEntry.h"
#include "lunar/HorizontalLayout.h"

TaskBar::TaskBar() {
    setLayout(new Lunar::HorizontalLayout(8, Lunar::HorizontalLayout::LEFT));
}

void TaskBar::addEntry(ClientWindow &window) {
    auto *image = new TaskBarEntry(window);

    taskBarEntries.put(window.getId(), image);
    addChild(image);
}

void TaskBar::removeEntry(const ClientWindow &window) {
    if (taskBarEntries.containsKey(window.getId())) {
        const auto *image = taskBarEntries.get(window.getId());
        taskBarEntries.remove(window.getId());
        removeChild(image);
    }
}

void TaskBar::updateEntry(const ClientWindow &window) {
    if (taskBarEntries.containsKey(window.getId())) {
        auto *image = taskBarEntries.get(window.getId());
        image->setImage(window.getIcon().scale(ICON_SIZE, ICON_SIZE));
    }
}

size_t TaskBar::getPreferredHeight() const {
    return ICON_SIZE + 8;
}
