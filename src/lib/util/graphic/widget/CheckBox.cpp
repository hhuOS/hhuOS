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

#include "CheckBox.h"

#include "graphic/widget/Theme.h"

namespace Util::Graphic {

CheckBox::CheckBox(const String &text, const Font &font) : text(text.split("\n")[0]), font(font) {
    addActionListener(new ClickListener(*this));
}

void CheckBox::toggle() {
    checked = !checked;
    requireRedraw();
}

void CheckBox::setText(const String &text) {
    const auto newText = text.split("\n")[0];
    if (newText.length() == CheckBox::text.length()) {
        requireRedraw();
    } else {
        reportSizeChange();
    }

    CheckBox::text = newText;
}

const String& CheckBox::getText() const {
    return text;
}

bool CheckBox::isChecked() const {
    return checked;
}

size_t CheckBox::getWidth() const {
    return getHeight() + 2 * PADDING_X + GAP_X + font.getCharWidth() * text.length();
}

size_t CheckBox::getHeight() const {
    return font.getCharHeight() + 2 * PADDING_Y;
}

void CheckBox::draw(const LinearFrameBuffer &lfb) {
    const auto &style = Theme::CURRENT_THEME.checkBox().getStyle(*this);

    const auto boxSize = font.getCharHeight();
    const auto posX = getPosX() + PADDING_X;
    const auto posY = getPosY() + PADDING_Y;

    // Draw box
    lfb.fillSquare(posX, posY, boxSize, style.widgetColor);
    lfb.drawSquare(posX, posY, boxSize, style.borderColor);

    // Draw checkmark (if checked)
    if (checked) {
        const auto inset = boxSize / 6;
        const auto leftX = posX + inset;
        const auto midY = posY + boxSize / 2;
        const auto midX = posX + boxSize / 2;
        const auto bottomY = posY + boxSize - inset;
        const auto rightX = posX + boxSize - inset;
        const auto topY = posY + inset;
        lfb.drawLine(leftX, midY, midX, bottomY, style.accentColor);
        lfb.drawLine(midX, bottomY, rightX, topY, style.accentColor);
    }

    // Calculate text position
    const auto textX = posX + boxSize + GAP_X;
    const auto textY = posY + (boxSize - font.getCharHeight()) / 2;

    // Draw text
    lfb.drawString(font, textX, textY, static_cast<const char*>(text),
        style.textColor, Colors::INVISIBLE);

    Widget::draw(lfb);
}

CheckBox::ClickListener::ClickListener(CheckBox &box) : box(box) {}

void CheckBox::ClickListener::onMouseClicked() {
    box.toggle();
}

}
