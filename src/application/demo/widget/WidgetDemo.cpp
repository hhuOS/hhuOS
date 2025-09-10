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
 */

#include "WidgetDemo.h"

#include "graphic/Ansi.h"
#include "graphic/Ansi.h"
#include "graphic/BufferedLinearFrameBuffer.h"
#include "graphic/font/Terminal8x8.h"
#include "graphic/widget/Button.h"
#include "graphic/widget/Container.h"
#include "graphic/widget/Label.h"
#include "io/key/MouseDecoder.h"
#include "io/stream/FileInputStream.h"

class ClickCountListener final : public Util::Graphic::ActionListener {

public:

    explicit ClickCountListener(Util::Graphic::Label &label) : label(label) {}

    void onMouseClick() override {
        label.setText(Util::String::format("Pressed: %u", ++count));
    }

private:

    Util::Graphic::Label &label;
    size_t count = 0;
};

WidgetDemo::WidgetDemo(Util::Graphic::LinearFrameBuffer &lfb) :
    WidgetApplication(lfb, 100, 100, 320, 240) {}

void WidgetDemo::run() {
    Util::Graphic::Ansi::prepareGraphicalApplication(false);

    auto testLabel = Util::Graphic::Label("This is a test!", 200, Util::Graphic::Fonts::TERMINAL_8x8);
    auto lineBreakTestLabel = Util::Graphic::Label("This\na test\nwith linebreaks!", 200, Util::Graphic::Fonts::TERMINAL_8x8);

    auto button = Util::Graphic::Button("Button");
    auto pressedLabel = Util::Graphic::Label("Pressed: 0", 200, Util::Graphic::Fonts::TERMINAL_8x8);

    button.addActionListener(new ClickCountListener(pressedLabel));

    addWidget(testLabel, 10, 10);
    addWidget(lineBreakTestLabel, 10, 10 + testLabel.getHeight() + 10);

    addWidget(button, 10, 100);
    addWidget(pressedLabel, 10, 100 + button.getHeight() + 10);

    while (true) {
        update();
    }
}
