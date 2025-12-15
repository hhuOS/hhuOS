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

#include "util/graphic/Ansi.h"
#include "util/graphic/BufferedLinearFrameBuffer.h"
#include "util/graphic/font/Terminal8x16.h"
#include "lunar/Button.h"
#include "lunar/CheckBox.h"
#include "lunar/Container.h"
#include "lunar/FreeLayout.h"
#include "lunar/GridLayout.h"
#include "lunar/HorizontalLayout.h"
#include "lunar/VerticalLayout.h"
#include "lunar/InputField.h"
#include "lunar/Label.h"
#include "lunar/RadioButton.h"
#include "lunar/RadioButtonGroup.h"
#include "util/io/key/MouseDecoder.h"
#include "util/io/stream/FileInputStream.h"
#include "lunar/BorderLayout.h"

class ClickCountListener final : public Lunar::ActionListener {

public:

    explicit ClickCountListener(Lunar::Label &label) : label(label) {}

    void onMouseClicked() override {
        label.setText(Util::String::format("Pressed: %u", ++count));
    }

    size_t getCount() const {
        return count;
    }

private:

    Lunar::Label &label;
    size_t count = 0;
};

class ExitListener final : public Lunar::ActionListener {

public:

    explicit ExitListener(bool &isRunning) : isRunning(isRunning) {}

    void onMouseClicked() override {
        isRunning = false;
    }

private:

    bool &isRunning;
};

class PackListener final : public Lunar::ActionListener {

public:

    explicit PackListener(WidgetApplication &rootContainer, Lunar::Button &packButton) :
        rootContainer(rootContainer), packButton(packButton) {}

    void onMouseClicked() override {
        if (packed) {
            rootContainer.setSize(320, 240);
            packButton.setText("Pack");
            packed = false;
        } else {
            packButton.setText("Reset Size");
            rootContainer.pack();
            packed = true;
        }
    }

private:

    bool packed = false;
    WidgetApplication &rootContainer;
    Lunar::Button &packButton;
};

WidgetDemo::WidgetDemo(Util::Graphic::LinearFrameBuffer &lfb) : WidgetApplication(lfb, 320, 240) {}

void WidgetDemo::run() {
    Util::Graphic::Ansi::prepareGraphicalApplication(true);

    setLayout(new Lunar::BorderLayout());

    // Add a label to the top
    auto *northContainer = new Container();
    northContainer->setLayout(new Lunar::HorizontalLayout());

    auto *northLabel = new Lunar::Label("Widget Demo", Util::Graphic::Fonts::TERMINAL_8x16);
    northContainer->addChild(northLabel);

    addChild(northContainer, Util::Array<size_t>{Lunar::BorderLayout::NORTH});

    // Add exit and pack buttons to the south
    auto *southContainer = new Container();
    southContainer->setLayout(new Lunar::HorizontalLayout(10));

    bool isRunning = true;
    auto *exitButton = new Lunar::Button("Exit");
    exitButton->addActionListener(new ExitListener(isRunning));

    auto packButton = new Lunar::Button("Pack");
    packButton->addActionListener(new PackListener(*this, *packButton));

    southContainer->addChild(exitButton);
    southContainer->addChild(packButton);

    addChild(southContainer, Util::Array<size_t>{Lunar::BorderLayout::SOUTH});

    // Add a container in the center with a grid layout
    auto *centerContainer = new Container();
    centerContainer->setLayout(new Lunar::GridLayout(2, 2));

    auto *topLeftContainer = new Container();
    auto *bottomLeftContainer = new Container();
    auto *topRightContainer = new Container();
    auto *bottomRightContainer = new Container();

    topLeftContainer->setLayout(new Lunar::VerticalLayout(10));
    bottomLeftContainer->setLayout(new Lunar::VerticalLayout(10));
    topRightContainer->setLayout(new Lunar::VerticalLayout(10));
    bottomRightContainer->setLayout(new Lunar::VerticalLayout(10));

    centerContainer->addChild(topLeftContainer);
    centerContainer->addChild(topRightContainer);
    centerContainer->addChild(bottomLeftContainer);
    centerContainer->addChild(bottomRightContainer);

    addChild(centerContainer, Util::Array<size_t>{Lunar::BorderLayout::CENTER});

    // Test labels
    auto *testLabel = new Lunar::Label("This is a test!");
    auto *lineBreakTestLabel = new Lunar::Label("This is\na test\nwith linebreaks!");

    topLeftContainer->addChild(testLabel);
    topLeftContainer->addChild(lineBreakTestLabel);

    // Test button
    auto *button = new Lunar::Button("Button");
    auto *pressedLabel = new Lunar::Label("Pressed: 0");

    auto *clickListener = new ClickCountListener(*pressedLabel);
    button->addActionListener(clickListener);

    bottomLeftContainer->addChild(button);
    bottomLeftContainer->addChild(pressedLabel);

    // Test checkbox
    auto *checkbox = new Lunar::CheckBox("Checkbox", Util::Graphic::Fonts::TERMINAL_8x16);
    bottomLeftContainer->addChild(checkbox);

    // Test radio buttons
    auto *radioGroup = new Lunar::RadioButtonGroup();
    auto *radio1 = new Lunar::RadioButton("Option 1", Util::Graphic::Fonts::TERMINAL_8x16);
    auto *radio2 = new Lunar::RadioButton("Option 2", Util::Graphic::Fonts::TERMINAL_8x16);
    auto *radio3 = new Lunar::RadioButton("Option 3", Util::Graphic::Fonts::TERMINAL_8x16);

    radioGroup->add(*radio1);
    radioGroup->add(*radio2);
    radioGroup->add(*radio3);

    topRightContainer->addChild(radio1);
    topRightContainer->addChild(radio2);
    topRightContainer->addChild(radio3);

    // Test input field
    auto *inputLabel = new Lunar::Label("Input Field:", 150);
    auto *inputField = new Lunar::InputField(100, Util::Graphic::Fonts::TERMINAL_8x16);

    bottomRightContainer->addChild(inputLabel);
    bottomRightContainer->addChild(inputField);

    while (isRunning) {
        update();
    }

    Util::System::out << "Button pressed " << clickListener->getCount() << " times" << Util::Io::PrintStream::ln
        << "Checkbox is " << (checkbox->isChecked() ? "checked" : "not checked") << Util::Io::PrintStream::ln
        << "Selected radio button: " << (radioGroup->getSelectedButton() == nullptr ?
            "None" : radioGroup->getSelectedButton()->getText()) << Util::Io::PrintStream::ln
        << "Input field text: " << static_cast<const char*>(inputField->getText()) << Util::Io::PrintStream::ln
        << Util::Io::PrintStream::flush;

    Util::Graphic::Ansi::cleanupGraphicalApplication();
}
