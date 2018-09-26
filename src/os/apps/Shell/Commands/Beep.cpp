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

#include <lib/file/File.h>
#include <kernel/events/input/KeyEvent.h>
#include <lib/file/FileStatus.h>
#include "Beep.h"

Beep::Beep(Shell &shell) : Command(shell) {

}

void Beep::execute(Util::Array<String> &args) {
    ArgumentParser parser(getHelpText(), 1);

    if(!parser.parse(args)) {
        stderr << args[0] << ": " << parser.getErrorString() << endl;
        return;
    }

    if(parser.getUnnamedArguments().length() == 0) {
        stderr << args[0] << ": Missing operand!" << endl;
        return;
    }

    String absolutePath = calcAbsolutePath(parser.getUnnamedArguments()[0]);

    FileStatus *fStat = FileStatus::stat(absolutePath);

    beepFile = BeepFile::load(absolutePath);

    if(beepFile == nullptr) {
        stderr << args[0] << ": Unable to open file '" << parser.getUnnamedArguments()[0] << "'!" << endl;
        return;
    }

    stdout << "Playing '" << fStat->getName() << "'." << endl;
    stdout << "Press <RETURN> to stop." << endl;

    Kernel::getService<EventBus>()->subscribe(*this, KeyEvent::TYPE);

    beepFile->play();

    Kernel::getService<EventBus>()->unsubscribe(*this, KeyEvent::TYPE);

    delete fStat;
    delete beepFile;
}

const String Beep::getHelpText() {
    return "Plays a WAV-file.\n\n"
           "Usage: Beep [OPTION]... [PATH]\n\n"
           "Options:\n"
           "  -h, --help: Show this help-message";
}

void Beep::onEvent(const Event &event) {
    auto &keyEvent = (KeyEvent&) event;

    if(keyEvent.getKey().scancode() == KeyEvent::RETURN) {
        beepFile->stop();
    }
}
