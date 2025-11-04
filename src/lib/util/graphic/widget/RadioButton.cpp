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

#include "RadioButton.h"

#include "util/graphic/widget/RadioButtonGroup.h"

namespace Util::Graphic {

RadioButton::RadioButton(const String &text, const Font &font) : text(text.split("\n")[0]), font(font) {
    addActionListener(new ClickListener(*this));
}

void RadioButton::select() const {
    if (group == nullptr) {
        Panic::fire(Panic::ILLEGAL_STATE, "RadioButton: Not assigned to any group!");
    }

    group->select(groupIndex);
}

void RadioButton::setText(const String &text) {
    const auto oldText = RadioButton::text;
    RadioButton::text = text.split("\n")[0];

    requireRedraw();

    if (oldText.length() != RadioButton::text.length()) {
        reportPreferredSizeChange();
    }
}

const String& RadioButton::getText() const {
    return text;
}

bool RadioButton::isSelected() const {
    return selected;
}

size_t RadioButton::getPreferredWidth() const {
    return getPreferredHeight() + GAP_X + font.getCharWidth() * text.length();
}

size_t RadioButton::getPreferredHeight() const {
    return font.getCharHeight();
}

void RadioButton::setSize(size_t width, size_t height) {
    const auto preferredWidth = getPreferredWidth();
    const auto preferredHeight = getPreferredHeight();

    if (width > preferredWidth) {
        width = preferredWidth;
    }
    if (height > preferredHeight) {
        height = preferredHeight;
    }

    if (width != getWidth() || height != getHeight()) {
        Widget::setSize(width, height);
    }
}

void RadioButton::draw(const LinearFrameBuffer &lfb) {
    const auto &style = Theme::CURRENT_THEME.radioButton().getStyle(*this);
    const auto posX = getPosX();
    const auto posY = getPosY();
    const auto width = getWidth();
    const auto height = getHeight();

    // Draw radio button circle
    const auto textHeight = font.getCharHeight();
    const auto diameter = textHeight < height ? textHeight : height;
    const auto radius = diameter / 2;

    lfb.fillCircle(posX + radius, posY + radius, radius, style.widgetColor);
    lfb.drawCircle(posX + radius, posY + radius, radius, style.borderColor);

    if (selected) {
        lfb.fillCircle(posX + radius, posY + radius, radius / 2, style.accentColor);
    }

    if (height < textHeight) {
        // Not enough space to draw text
        Widget::draw(lfb);
        return;
    }

    // Calculate maximum text length that fits into the remaining space
    const auto maxTextWidth = width - diameter - GAP_X;
    const auto maxTextLength = maxTextWidth / font.getCharWidth();

    auto text = RadioButton::text;
    if (text.length() > maxTextLength) {
        // Not enough space to draw full text, truncate it and add "..."
        if (maxTextLength < 3) {
            // Not enough space to even draw "..."
            return;
        }

        text = text.substring(0, maxTextLength - 3) + "...";
    }

    lfb.drawString(font, posX + diameter + GAP_X, posY, static_cast<const char*>(text),
        style.textColor, style.textBackgroundColor);

    Widget::draw(lfb);
}

RadioButton::ClickListener::ClickListener(RadioButton &button) : button(button) {}

void RadioButton::ClickListener::onMouseClicked() {
    button.select();
}

}
