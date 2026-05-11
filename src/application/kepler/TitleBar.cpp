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

#include "TitleBar.h"
#include "ClientWindow.h"
#include "lunar/BorderLayout.h"
#include "lunar/Button.h"
#include "lunar/HorizontalLayout.h"
#include "util/graphic/LinearFrameBufferView.h"

class CloseButtonHandler : public Lunar::ActionListener {

public:

    explicit CloseButtonHandler(ClientWindow &clientWindow) : clientWindow(clientWindow) {}

    void onMousePressed() override {
        const auto event = Kepler::Event::WindowClose();
        clientWindow.sendWindowCloseEvent(event);
    }

private:

    ClientWindow &clientWindow;
};

TitleBar::TitleBar(ClientWindow &clientWindow, const Util::String &title) : clientWindow(clientWindow) {
    rootContainer.setLayout(new Lunar::BorderLayout());

    titleLabel = new Lunar::Label(title);
    titleContainer->setLayout(new Lunar::HorizontalLayout());
    titleContainer->addChild(titleLabel);

    closeButton->setOverrideStyle(CLOSE_BUTTON_STYLE);
    closeButton->addActionListener(new CloseButtonHandler(clientWindow));

    buttonContainer->setLayout(new Lunar::HorizontalLayout(5));
    buttonContainer->addChild(minimizeButton);
    buttonContainer->addChild(closeButton);

    rootContainer.addChild(titleContainer, Util::Array<size_t>{Lunar::BorderLayout::CENTER});
    rootContainer.addChild(buttonContainer, Util::Array<size_t>{Lunar::BorderLayout::EAST});

    height = rootContainer.getPreferredHeight() + 1;
}

void TitleBar::draw(const Util::Graphic::LinearFrameBuffer &lfb, const int32_t posX, const int32_t posY, const uint16_t width, const bool focused) {
    const Util::Graphic::LinearFrameBufferView lfbView(lfb, posX, posY, width, height);

    if (focused) {
        rootContainer.setOverrideStyle(FOCUSED_TITLE_BAR_STYLE);
        titleContainer->setOverrideStyle(FOCUSED_TITLE_BAR_STYLE);
        buttonContainer->setOverrideStyle(FOCUSED_TITLE_BAR_STYLE);
    } else {
        rootContainer.resetStyle();
        titleContainer->resetStyle();
        buttonContainer->resetStyle();
    }

    rootContainer.setSize(width, height);
    rootContainer.requireRedraw();
    rootContainer.draw(lfbView);
}

void TitleBar::onMouseHover(const MouseEvent &event) {
    auto *hoveredChild = rootContainer.getChildAtPoint(event.contentPosX, event.contentPosY);

    if (hoveredChild != lastHoveredChild) {
        if (lastHoveredChild != nullptr) {
            lastHoveredChild->mouseExited();
        }
        if (hoveredChild != nullptr) {
            hoveredChild->mouseEntered();
        }
    }

    lastHoveredChild = hoveredChild;
}

void TitleBar::onMouseExit() {
    if (lastHoveredChild != nullptr) {
        lastHoveredChild->mouseExited();
    }

    lastHoveredChild = nullptr;
}

void TitleBar::onMouseClick(const MouseEvent &event) {
    auto *clickedChild = rootContainer.getChildAtPoint(event.contentPosX, event.contentPosY);
    if (clickedChild != lastPressedChild && lastPressedChild != nullptr) {
        lastPressedChild->setFocused(false);
    }

    if (clickedChild != nullptr) {
        clickedChild->setFocused(true);
        clickedChild->mousePressed();
    }

    lastPressedChild = clickedChild;
}

void TitleBar::onMouseRelease(const MouseEvent&) {
    if (lastPressedChild != nullptr) {
        lastPressedChild->mouseReleased();
        lastPressedChild->mouseClicked();
    }
}

bool TitleBar::needsRedraw() const {
    return rootContainer.requiresRedraw();
}

bool TitleBar::isMouseOnButton(const MouseEvent &event) {
    const auto *child = rootContainer.getChildAtPoint(event.contentPosX, event.contentPosY);
    return child == minimizeButton || child == closeButton;
}
