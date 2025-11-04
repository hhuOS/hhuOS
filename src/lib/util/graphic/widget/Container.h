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

#ifndef HHUOS_LIB_UTIL_GRAPHIC_WIDGET_CONTAINER_H
#define HHUOS_LIB_UTIL_GRAPHIC_WIDGET_CONTAINER_H

#include <stddef.h>

#include "util/collection/ArrayList.h"
#include "util/graphic/LinearFrameBuffer.h"
#include "util/graphic/widget/Layout.h"
#include "util/graphic/widget/Widget.h"

namespace Util::Graphic {

class Container : public Widget {

public:

    Container();

    ~Container() override;

    void setLayout(Layout *layout);

    void addChild(Widget &widget, const Array<size_t> &layoutArgs = Array<size_t>());

    [[nodiscard]] size_t getPreferredWidth() const override;

    [[nodiscard]] size_t getPreferredHeight() const override;

    [[nodiscard]] bool requiresRedraw() const override;

    [[nodiscard]] Widget* getChildAtPoint(size_t posX, size_t posY) override;

    void draw(const LinearFrameBuffer &lfb) override;

protected:

    Container(size_t width, size_t height);

private:

    void rearrangeChildren() override;

    friend class Widget;

    Layout *layout = nullptr;
    ArrayList<Layout::WidgetEntry> children;
};

}

#endif