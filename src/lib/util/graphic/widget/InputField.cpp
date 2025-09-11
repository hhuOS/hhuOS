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

#include "InputField.h"

#include "graphic/widget/Theme.h"

namespace Util::Graphic {

InputField::InputField(const size_t width, const Font &font) : width(width), font(font) {
    addActionListener(new KeyInputListener(*this));
}

const String& InputField::getText() const {
    return text;
}

size_t InputField::getWidth() const {
    return width;
}

size_t InputField::getHeight() const {
    return font.getCharHeight() + PADDING_Y * 2;
}

void InputField::draw(const LinearFrameBuffer &lfb) {
    const auto &style = Theme::CURRENT_THEME.inputField().getStyle(*this);

    const auto height = getHeight();
    const auto posX = getPosX();
    const auto posY = getPosY();

    lfb.fillRectangle(posX, posY, width, height, style.widgetColor);
    lfb.drawRectangle(posX, posY, width, height, style.borderColor);

    const auto maxChars = (width - PADDING_X * 2) / font.getCharWidth();
    const auto visibleText = text.substring(text.length() > maxChars ? text.length() - maxChars : 0,
        text.length());

    lfb.drawString(font, posX + PADDING_X, posY + PADDING_Y, static_cast<const char*>(visibleText),
        style.textColor, style.textBackgroundColor);

    if (isFocused()) {
        const auto caretX = posX + 2 + visibleText.length() * font.getCharWidth();
        lfb.drawLine(caretX, posY + PADDING_Y, caretX, posY + font.getCharHeight(), style.accentColor);
    }

    Widget::draw(lfb);
}

InputField::KeyInputListener::KeyInputListener(InputField &inputField) : inputField(inputField) {}

void InputField::KeyInputListener::onKeyTyped(const Io::Key &key) {
    if (key.getAscii() == '\b') {
        inputField.text = inputField.text.substring(0, inputField.text.length() - 1);
        inputField.requireRedraw();
    } else if (CharacterTypes::isPrintable(key.getAscii())) {
        inputField.text += key.getAscii();
        inputField.requireRedraw();
    }
}

}
