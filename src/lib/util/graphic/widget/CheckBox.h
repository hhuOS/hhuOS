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

#ifndef HHUOS_LIB_UTIL_GRAPHIC_WIDGET_CHECKBOX_H
#define HHUOS_LIB_UTIL_GRAPHIC_WIDGET_CHECKBOX_H

#include "base/String.h"
#include "graphic/font/Terminal8x8.h"
#include "graphic/widget/Widget.h"

namespace Util::Graphic {

class CheckBox final : public Widget {

public:

    explicit CheckBox(const String &text, const Font &font = Fonts::TERMINAL_8x8);

    void toggle();

    void setText(const String &text);

    [[nodiscard]] const String& getText() const;

    [[nodiscard]] bool isChecked() const;

    [[nodiscard]] size_t getWidth() const override;

    [[nodiscard]] size_t getHeight() const override;

    void draw(const LinearFrameBuffer &lfb) override;

private:

    class ClickListener final : public ActionListener {

    public:

        explicit ClickListener(CheckBox &box);

        void onMouseClicked() override;

    private:

        CheckBox &box;
    };

    String text;
    const Font &font;

    bool checked = false;

    bool hovered = false;
    bool pressed = false;

    static constexpr size_t PADDING_X = 2;
    static constexpr size_t PADDING_Y = 2;
    static constexpr size_t GAP_X = 6;
};

}

#endif