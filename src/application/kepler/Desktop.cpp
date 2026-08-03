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

#include "Desktop.h"

#include <util/async/Process.h>
#include <util/graphic/BitmapFile.h>
#include <util/graphic/font/Terminal8x8.h>
#include <lunar/Label.h>
#include <lunar/Image.h>
#include <lunar/BorderLayout.h>
#include <lunar/GridLayout.h>
#include <lunar/VerticalLayout.h>

#include "lunar/HorizontalLayout.h"

Desktop::Desktop(const uint16_t width, const uint16_t height) :
    RootContainer(width, height),
    rows(height / (DESKTOP_ENTRY_HEIGHT + DESKTOP_ENTRY_SPACING)),
    columns(width / (DESKTOP_ENTRY_WIDTH + DESKTOP_ENTRY_SPACING))
{
    background = Util::Graphic::BitmapFile::open("/user/kepler/background.bmp");
    if (background != nullptr) {
        if (background->getWidth() > width || background->getHeight() > height) {
            background = background->scale(width, height);
        }
    }

    RootContainer::setSize(width, height);
    setLayout(new Lunar::BorderLayout());

    desktopContainer->setLayout(new Lunar::GridLayout(rows, columns));

    addChild(desktopContainer, Util::Array<size_t>{Lunar::BorderLayout::CENTER});
    addChild(taskBar, Util::Array<size_t>{Lunar::BorderLayout::SOUTH});
}

Desktop::~Desktop() {
    delete background;
}

void Desktop::addEntry(const Util::String &name, const Util::String &executable, const Util::Array<Util::String> &args,
                       const Util::String &iconPath) const
{
    Util::Io::File iconFile(iconPath);
    if (!iconFile.exists() || iconFile.isDirectory()) {
        iconFile = Util::Io::File("/user/kepler/telescope.bmp");
    }

    auto *entry = new DesktopContainer();
    auto *entryLayout = new Lunar::VerticalLayout(4);
    auto *label = new Lunar::Label(name);
    auto *image = new Lunar::Image(iconFile.getCanonicalPath(), DesktopEntry::ICON_SIZE,
        DesktopEntry::ICON_SIZE);

    label->setOverrideStyle(LABEL_STYLE);

    entry->setLayout(entryLayout);
    entry->addChild(image);
    entry->addChild(label);

    entry->addActionListener(new IconListener(executable, name, args));
    label->addActionListener(new IconListener(executable, name, args));
    image->addActionListener(new IconListener(executable, name, args));

    desktopContainer->addChild(entry);
}

void Desktop::windowCreated(ClientWindow &window) {
    taskBar->addEntry(window);
}

void Desktop::windowClosed(const ClientWindow &window) {
    taskBar->removeEntry(window);
}

void Desktop::windowUpdated(const ClientWindow &window) {
    taskBar->updateEntry(window);
}

void Desktop::draw(const Util::Graphic::LinearFrameBuffer &lfb) {
    lfb.drawImage(*background, (lfb.getResolutionX() - background->getWidth()) / 2,
        (lfb.getResolutionY() - background->getHeight()) / 2);

    taskBar->requireRedraw();
    for (const auto &child : getChildren()) {
        child.widget->draw(lfb);
    }

    Widget::draw(lfb);
}

void Desktop::IconListener::onMouseClicked() {
    const auto nullFile = Util::Io::File("/device/null");
    Util::Async::Process::execute(executable, nullFile, nullFile, nullFile, name, args);
}

void Desktop::DesktopContainer::draw(const Util::Graphic::LinearFrameBuffer &lfb) {
    for (const auto &child : getChildren()) {
        child.widget->draw(lfb);
    }

    Widget::draw(lfb);
}

const Util::Graphic::Font &Desktop::FONT = Util::Graphic::Fonts::TERMINAL_8x8;

const int32_t Desktop::DESKTOP_ENTRY_WIDTH = FONT.getCharWidth() * 8;

const int32_t Desktop::DESKTOP_ENTRY_HEIGHT =
    DesktopEntry::ICON_SIZE + FONT.getCharHeight() + DESKTOP_ENTRY_TEXT_SPACING;
