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
#include "graphic/widget/GridLayout.h"
#include "graphic/widget/HorizontalLayout.h"
#include "graphic/widget/VerticalLayout.h"
#include "graphic/widget/InputField.h"
#include "graphic/widget/Label.h"
#include "graphic/widget/RadioButton.h"
#include "graphic/widget/RadioButtonGroup.h"
#include "io/key/MouseDecoder.h"
#include "io/stream/FileInputStream.h"
#include "graphic/widget/BorderLayout.h"

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

WidgetDemo::WidgetDemo(Util::Graphic::LinearFrameBuffer &lfb) : WidgetApplication(lfb, 320, 240) {}

void WidgetDemo::run() {
    Util::Graphic::Ansi::prepareGraphicalApplication(true);

    setLayout(new Util::Graphic::BorderLayout());

    // Add a label to the top
    auto northLabel = Util::Graphic::Label("Widget Demo", Util::Graphic::Fonts::TERMINAL_8x16);
    addChild(northLabel, Util::Array<size_t>{Util::Graphic::BorderLayout::NORTH});

    // Add exit button to the south
    bool isRunning = true;
    auto exitButton = Util::Graphic::Button("Exit");
    exitButton.addActionListener(new ExitListener(isRunning));

    auto southContainer = Container();
    southContainer.setLayout(new Util::Graphic::HorizontalLayout());

    southContainer.addChild(exitButton);
    addChild(southContainer, Util::Array<size_t>{Util::Graphic::BorderLayout::SOUTH});

    // Add a container in the center with a grid layout
    auto centerContainer = Container();
    centerContainer.setLayout(new Util::Graphic::GridLayout(2, 2));

    auto topLeftContainer = Container();
    auto bottomLeftContainer = Container();
    auto topRightContainer = Container();
    auto bottomRightContainer = Container();

    topLeftContainer.setLayout(new Util::Graphic::VerticalLayout(10));
    bottomLeftContainer.setLayout(new Util::Graphic::VerticalLayout(10));
    topRightContainer.setLayout(new Util::Graphic::VerticalLayout(10));
    bottomRightContainer.setLayout(new Util::Graphic::VerticalLayout(10));

    centerContainer.addChild(topLeftContainer);
    centerContainer.addChild(topRightContainer);
    centerContainer.addChild(bottomLeftContainer);
    centerContainer.addChild(bottomRightContainer);

    addChild(centerContainer, Util::Array<size_t>{Util::Graphic::BorderLayout::CENTER});

    // Test labels
    auto testLabel = Util::Graphic::Label("This is a test!");
    auto lineBreakTestLabel = Util::Graphic::Label("This is\na test\nwith linebreaks!");

    topLeftContainer.addChild(testLabel);
    topLeftContainer.addChild(lineBreakTestLabel);

    // Test button
    auto button = Util::Graphic::Button("Button");
    auto pressedLabel = Util::Graphic::Label("Pressed: 0");

    auto *clickListener = new ClickCountListener(pressedLabel);
    button.addActionListener(clickListener);

    bottomLeftContainer.addChild(button);
    bottomLeftContainer.addChild(pressedLabel);

    // Test checkbox
    auto checkbox = Util::Graphic::CheckBox("Checkbox", Util::Graphic::Fonts::TERMINAL_8x16);
    bottomLeftContainer.addChild(checkbox);

    // Test radio buttons
    auto radioGroup = Util::Graphic::RadioButtonGroup();
    auto radio1 = Util::Graphic::RadioButton("Option 1", Util::Graphic::Fonts::TERMINAL_8x16);
    auto radio2 = Util::Graphic::RadioButton("Option 2", Util::Graphic::Fonts::TERMINAL_8x16);
    auto radio3 = Util::Graphic::RadioButton("Option 3", Util::Graphic::Fonts::TERMINAL_8x16);

    radioGroup.add(radio1);
    radioGroup.add(radio2);
    radioGroup.add(radio3);

    topRightContainer.addChild(radio1);
    topRightContainer.addChild(radio2);
    topRightContainer.addChild(radio3);

    // Test input field
    auto inputLabel = Util::Graphic::Label("Input Field:", 150);
    auto inputField = Util::Graphic::InputField(100, Util::Graphic::Fonts::TERMINAL_8x16);

    bottomRightContainer.addChild(inputLabel);
    bottomRightContainer.addChild(inputField);

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
