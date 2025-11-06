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
 * The game engine is based on a bachelor's thesis, written by Malte Sehmer.
 * The original source code can be found here: https://github.com/Malte2036/hhuOS
 *
 * It has been enhanced with 3D-capabilities during a bachelor's thesis by Richard Josef Schweitzer
 * The original source code can be found here: https://git.hhu.de/bsinfo/thesis/ba-risch114
 *
 * The 3D-rendering has been rewritten using OpenGL (TinyGL) during a bachelor's thesis by Kevin Weber
 * The original source code can be found here: https://git.hhu.de/bsinfo/thesis/ba-keweb100
 *
 * The 2D particle system is based on a bachelor's thesis, written by Abdulbasir Gümüs.
 * The original source code can be found here: https://git.hhu.de/bsinfo/thesis/ba-abgue101
 */

#ifndef HHUOS_LIB_PULSAR_TEXTSCREEN_H
#define HHUOS_LIB_PULSAR_TEXTSCREEN_H

#include "2d/Scene.h"

namespace Pulsar {

/// A simple 2D scene that displays a static text centered on the screen.
/// The text can have multiple lines, which are separated by newline characters ('\n').
/// Key presses can be handled via a callback function.
/// This class is useful for displaying intro and outro screens or instructions.
class TextScreen final : public D2::Scene {

public:
    /// Creates a new TextScreen instance with the given text and optional parameters.
    /// The callback function is invoked whenever a key is pressed.
    explicit TextScreen(const Util::String &text,
        void(*onKeyPressed)(const Util::Io::Key &key) = nullptr,
        const Util::Graphic::Color &fontColor = Util::Graphic::Colors::WHITE,
        const Util::Graphic::Color &backgroundColor = Util::Graphic::Colors::BLACK);

    /// Initialize the scene.
    /// For this simple scene, this method does nothing.
    void initialize() override;

    /// Initialize the background by rendering the text centered on the screen.
    bool initializeBackground(Graphics &graphics) override;

    /// Handle key press events by invoking the callback function if provided.
    void keyPressed(const Util::Io::Key &key) override;

private:

    const Util::String text;
    void(*onKeyPressed)(const Util::Io::Key &key);
    const Util::Graphic::Color fontColor;
    const Util::Graphic::Color backgroundColor;
};

}

#endif