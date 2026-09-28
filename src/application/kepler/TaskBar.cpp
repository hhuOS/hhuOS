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
#include "lunar/BorderLayout.h"
#include "lunar/HorizontalLayout.h"
#include "lunar/VerticalLayout.h"
#include "util/base/System.h"
#include "util/graphic/font/Terminal8x16.h"

TaskBar::TaskBar() {
    setLayout(new Lunar::BorderLayout());

    auto *leftContainer = new Container();
    startIcon = new Lunar::Image("/user/kepler/icon/start.bmp", ICON_SIZE, ICON_SIZE);
    leftContainer->setLayout(new Lunar::VerticalLayout());
    leftContainer->addChild(startIcon);

    // Wrap task container in vertical layout for centering
    centerContainer = new Container();
    centerContainer->setLayout(new Lunar::VerticalLayout());
    taskContainer = new Container();
    taskContainer->setLayout(new Lunar::GridLayout(1, 1, 0, TASK_BAR_ICON_GAP));
    centerContainer->addChild(taskContainer);

    auto *scrollContainer = new Container();
    scrollContainer->setLayout(new Lunar::VerticalLayout(4));
    auto *prevButton = new Lunar::Button("<");
    prevButton->addActionListener(new TaskBarSwitchButtonListener(*this, TaskBarSwitchButtonListener::PREVIOUS));
    auto *nextButton = new Lunar::Button(">");
    nextButton->addActionListener(new TaskBarSwitchButtonListener(*this, TaskBarSwitchButtonListener::NEXT));
    scrollContainer->addChild(prevButton);
    scrollContainer->addChild(nextButton);

    auto *indicatorLabelContainer = new Container();
    indicatorLabelContainer->setLayout(new Lunar::GridLayout(3, 1));
    currentIndexLabel = new Lunar::Label("1");
    maxIndexLabel = new Lunar::Label("1");
    indicatorLabelContainer->addChild(currentIndexLabel);
    indicatorLabelContainer->addChild(new Lunar::Label("/"));
    indicatorLabelContainer->addChild(maxIndexLabel);

    auto *clockContainer = new Container();
    clockContainer->setLayout(new Lunar::VerticalLayout(4));
    const Util::Time::Date date;
    clockLabel = new Lunar::Label(Util::String::formatDate(date, "%H:%M"), Util::Graphic::Fonts::TERMINAL_8x16);
    dateLabel = new Lunar::Label(Util::String::formatDate(date, "%d/%m/%Y"), Util::Graphic::Fonts::TERMINAL_8x8);
    clockContainer->addChild(clockLabel);
    clockContainer->addChild(dateLabel);

    auto *rightContainer = new Container();
    rightContainer->setLayout(new Lunar::HorizontalLayout(4));
    rightContainer->addChild(indicatorLabelContainer);
    rightContainer->addChild(scrollContainer);
    rightContainer->addChild(clockContainer);

    addChild(leftContainer, Util::Array<size_t>{Lunar::BorderLayout::WEST});
    addChild(centerContainer, Util::Array<size_t>{Lunar::BorderLayout::CENTER});
    addChild(rightContainer, Util::Array<size_t>{Lunar::BorderLayout::EAST});
}

void TaskBar::addEntry(ClientWindow &window) {
    auto *image = new TaskBarEntry(window);

    taskBarEntries.put(window.getId(), image);
    windowIds.add(window.getId());

    updateTaskContainer(true);
}

void TaskBar::removeEntry(const ClientWindow &window) {
    if (taskBarEntries.containsKey(window.getId())) {
        taskBarEntries.remove(window.getId());
        windowIds.remove(window.getId());

        updateTaskContainer();
    }
}

void TaskBar::updateEntry(const ClientWindow &window) const {
    if (taskBarEntries.containsKey(window.getId())) {
        auto *image = taskBarEntries.get(window.getId());
        image->setImage(window.getIcon().scale(ICON_SIZE, ICON_SIZE));
    }
}

void TaskBar::setSize(const size_t width, const size_t height) {
    Container::setSize(width, height);
    updateTaskContainer();
}

void TaskBar::TaskBarSwitchButtonListener::onMousePressed() {
    switch (direction) {
        case PREVIOUS:
            if (taskBar.currentTaskBarIndex > 0) {
                taskBar.currentTaskBarIndex--;
                taskBar.updateTaskContainer();
            }
            break;
        case NEXT:
            const auto maxIndex = taskBar.windowIds.size() % taskBar.maxTaskBarEntries == 0 ?
                taskBar.windowIds.size() / taskBar.maxTaskBarEntries - 1 :
                taskBar.windowIds.size() / taskBar.maxTaskBarEntries;

            if (taskBar.currentTaskBarIndex < maxIndex) {
                taskBar.currentTaskBarIndex++;
                taskBar.updateTaskContainer();
            }
            break;
    }
}

void TaskBar::updateTaskContainer(const bool windowAdded) {
    if (maxTaskBarEntries == 0) {
        maxTaskBarEntries = centerContainer->getWidth() / (TaskBarEntry::SIZE + TASK_BAR_ICON_GAP);
        taskContainer->setLayout(new Lunar::GridLayout(1, maxTaskBarEntries, 0 ,8));
    }

    taskContainer->removeAllChildren();

    // If we are on the last page and the last entry has just been removed,
    // we need to decrement the currentTaskBarIndex to go to the previous page.
    while (windowIds.size() != 0 && currentTaskBarIndex * maxTaskBarEntries >= windowIds.size()) {
        currentTaskBarIndex--;
    }

    while (windowAdded && (currentTaskBarIndex + 1) * maxTaskBarEntries < windowIds.size()) {
        currentTaskBarIndex++;
    }

    currentIndexLabel->setText(Util::String::format("%u", currentTaskBarIndex + 1));

    if (windowIds.size() == 0) {
        maxIndexLabel->setText("1");
    } else {
        maxIndexLabel->setText(Util::String::format("%u", (windowIds.size() + maxTaskBarEntries - 1) / maxTaskBarEntries));
    }

    for (size_t i = 0; i < maxTaskBarEntries; i++) {
        const auto index = currentTaskBarIndex * maxTaskBarEntries + i;
        if (index >= windowIds.size()) {
            break;
        }

        taskContainer->addChild(taskBarEntries.get(windowIds.get(index)));
    }
}
