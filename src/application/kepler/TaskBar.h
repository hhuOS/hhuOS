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

#ifndef HHUOS_TASKBAR_H
#define HHUOS_TASKBAR_H

#include "ClientWindow.h"
#include "lunar/Container.h"
#include "lunar/GridLayout.h"
#include "util/collection/HashMap.h"

class TaskBarEntry;

class TaskBar : public Lunar::Container {

public:

    TaskBar();

    ~TaskBar() override = default;

    void addEntry(ClientWindow &window);

    void removeEntry(const ClientWindow &window);

    void updateEntry(const ClientWindow &window);

    void setSize(size_t width, size_t height) override;

    static constexpr size_t ICON_SIZE = 32;

private:

    class TaskBarSwitchButtonListener : public Lunar::ActionListener {

    public:

        enum Direction {
            PREVIOUS,
            NEXT
        };

        TaskBarSwitchButtonListener(TaskBar &taskBar, const Direction direction) : taskBar(taskBar), direction(direction) {}

        void onMousePressed() override;

    private:

        TaskBar &taskBar;
        Direction direction;
    };

    void updateTaskContainer();

    size_t maxTaskBarEntries = 4;
    size_t currentTaskBarIndex = 0;

    Lunar::Image *startIcon;

    Container *taskContainer;

    Lunar::Label *dateLabel;
    Lunar::Label *clockLabel;

    Util::HashMap<size_t, TaskBarEntry*> taskBarEntries;
    Util::ArrayList<size_t> windowIds;
};

#endif
