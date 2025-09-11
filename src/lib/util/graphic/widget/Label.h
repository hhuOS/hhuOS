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

#ifndef HHUOS_LIB_UTIL_GRAPHIC_WIDGET_LABEL_H
#define HHUOS_LIB_UTIL_GRAPHIC_WIDGET_LABEL_H

#include <stddef.h>

#include "base/String.h"
#include "graphic/font/Terminal8x8.h"
#include "graphic/LinearFrameBuffer.h"
#include "graphic/widget/Style.h"
#include "graphic/widget/Widget.h"

namespace Util::Graphic {

class Label final : public Widget {

public:

    Label(const String &text, size_t maxWidth, const Font &font = Fonts::TERMINAL_8x8);

    void setText(const String &text);

    [[nodiscard]] String getText() const;

    [[nodiscard]] size_t getWidth() const override;

    [[nodiscard]] size_t getHeight() const override;

    void draw(const LinearFrameBuffer &lfb) override;

private:

    void calculateLines(const String &text);

    Array<String> lines;
    size_t maxWidth;
    const Font &font;
    const Style style = DefaultTheme::label();
};

} // namespace Util

#endif
