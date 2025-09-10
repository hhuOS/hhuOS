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
#include "graphic/BufferedLinearFrameBuffer.h"
#include "graphic/font/Terminal8x16.h"
#include "graphic/widget/Button.h"
#include "graphic/widget/CheckBox.h"
#include "graphic/widget/Container.h"
#include "graphic/widget/FreeLayout.h"
#include "graphic/widget/InputField.h"
#include "graphic/widget/Label.h"
#include "graphic/widget/RadioButton.h"
#include "graphic/widget/RadioButtonGroup.h"
#include "io/key/MouseDecoder.h"
#include "io/stream/FileInputStream.h"

class ClickCountListener final : public Util::Graphic::ActionListener {

public:

    explicit ClickCountListener(Util::Graphic::Label &label) : label(label) {}

    void onMouseClicked() override {
        label.setText(Util::String::format("Pressed: %u", ++count));
    }

    [[nodiscard]] size_t getCount() const { return count; }

private:

    Util::Graphic::Label &label;
    size_t count = 0;
};

class ExitListener final : public Util::Graphic::ActionListener {

public:

    explicit ExitListener(bool &isRunning) : isRunning(isRunning) {};

    void onMouseClicked() override {
        isRunning = false;
    }

private:

    bool &isRunning;
};

WidgetDemo::WidgetDemo(Util::Graphic::LinearFrameBuffer &lfb) :
    WidgetApplication(lfb, 100, 100, 320, 240) {}

void WidgetDemo::run() {
    Util::Graphic::Ansi::prepareGraphicalApplication(true);

    setLayout(new Util::Graphic::FreeLayout());

    // Test widgets
    auto testLabel = Util::Graphic::Label("This is a test!", 150);
    auto lineBreakTestLabel = Util::Graphic::Label("This\na test\nwith linebreaks!", 150);

    addWidget(testLabel, {10, 10});
    addWidget(lineBreakTestLabel, {10, 10 + testLabel.getHeight() + 10});

    // Test button
    auto button = Util::Graphic::Button("Button");
    auto pressedLabel = Util::Graphic::Label("Pressed: 0", 150);

    auto *clickListener = new ClickCountListener(pressedLabel);
    button.addActionListener(clickListener);

    addWidget(button, {10, 100});
    addWidget(pressedLabel, {10, 100 + button.getHeight() + 10});

    // Test checkbox
    auto checkbox = Util::Graphic::CheckBox("Checkbox", Util::Graphic::Fonts::TERMINAL_8x16);
    addWidget(checkbox, {10, 170});

    // Test radio buttons
    auto radioGroup = Util::Graphic::RadioButtonGroup();
    auto radio1 = Util::Graphic::RadioButton("Option 1", Util::Graphic::Fonts::TERMINAL_8x16);
    auto radio2 = Util::Graphic::RadioButton("Option 2", Util::Graphic::Fonts::TERMINAL_8x16);
    auto radio3 = Util::Graphic::RadioButton("Option 3", Util::Graphic::Fonts::TERMINAL_8x16);

    radioGroup.add(radio1);
    radioGroup.add(radio2);
    radioGroup.add(radio3);

    addWidget(radio1, {170, 10});
    addWidget(radio2, {170, 40});
    addWidget(radio3, {170, 70});

    // Test input field
    auto inputField = Util::Graphic::InputField(100, Util::Graphic::Fonts::TERMINAL_8x16);
    addWidget(inputField, {170, 120});

    // Exit button
    bool isRunning = true;
    auto exitButton = Util::Graphic::Button("Exit");
    exitButton.addActionListener(new ExitListener(isRunning));

    addWidget(exitButton, {10, 210});

    while (isRunning) {
        update();
    }

    Util::System::out << "Button pressed " << clickListener->getCount() << " times" << Util::Io::PrintStream::ln
        << "Checkbox is " << (checkbox.isChecked() ? "checked" : "not checked") << Util::Io::PrintStream::ln
        << "Selected radio button: " << (radioGroup.getSelectedButton() == nullptr ?
            "None" : radioGroup.getSelectedButton()->getText()) << Util::Io::PrintStream::ln
        << "Input field text: " << static_cast<const char*>(inputField.getText()) << Util::Io::PrintStream::ln
        << Util::Io::PrintStream::flush;

    Util::Graphic::Ansi::cleanupGraphicalApplication();
}
