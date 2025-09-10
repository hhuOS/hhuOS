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

#ifndef HHUOS_LIB_UTIL_GRAPHIC_WIDGET_BUTTON_H
#define HHUOS_LIB_UTIL_GRAPHIC_WIDGET_BUTTON_H

#include <stddef.h>

#include "base/String.h"
#include "graphic/font/Terminal8x8.h"
#include "graphic/widget/Style.h"
#include "graphic/widget/Widget.h"

namespace Util::Graphic {

class Button final : public Widget {

public:

    explicit Button(const String &text, const Font &font = Fonts::TERMINAL_8x8);

    [[nodiscard]] size_t getWidth() const override;

    [[nodiscard]] size_t getHeight() const override;

    void setText(const String &text);

    void draw(LinearFrameBuffer &lfb) override;

private:

    class MouseHoverListener final : public ActionListener {

    public:

        explicit MouseHoverListener(Button &button);

        void onMouseEnter() override;

        void onMouseLeave() override;

        void onMousePress() override;

        void onMouseRelease() override;

    private:

        Button &button;
    };

    String text;
    const Font &font;
    Style style = DefaultTheme::button();

    bool hovered = false;
    bool pressed = false;
};

}

#endif