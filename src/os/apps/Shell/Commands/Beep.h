/*
 * Copyright (C) 2018 Burak Akguel, Christian Gesse, Fabian Ruhland, Filip Krakowski, Michael Schoettner
 * Heinrich-Heine University
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

#ifndef HHUOS_BEEP_H
#define HHUOS_BEEP_H

#include <lib/file/beep/BeepFile.h>
#include "Command.h"

class Beep : public Command, Receiver {

private:

    BeepFile *beepFile = nullptr;

    bool isRunning;

public:
    /**
     * Default-constructor.
     */
    Beep() = delete;

    /**
     * Copy-constructor.
     */
    Beep(const Beep &copy) = delete;

    /**
     * Constructor.
     *
     * @param shell The shell, that executes this command
     */
    explicit Beep(Shell &shell);

    /**
     * Destructor.
     */
    ~Beep() override = default;

    /**
     * Overriding function from Command.
     */
    void execute(Util::Array<String> &args) override;

    /**
     * Overriding function from Command.
     */
    const String getHelpText() override;

    /**
     * Overriding function from Receiver.
     */
    void onEvent(const Event &event) override;
};

#endif
