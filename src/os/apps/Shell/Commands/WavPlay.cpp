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

#include <lib/file/wav/Wav.h>
#include <kernel/services/SoundService.h>
#include <kernel/events/input/KeyEvent.h>
#include <kernel/threads/WorkerThread.h>
#include <kernel/threads/Scheduler.h>
#include "WavPlay.h"

bool isRunning = false;

uint32_t playback(File* const& file) {
    Wav wav(file);
    Kernel::getService<SoundService>()->getPcmAudioDevice()->playPcmData(wav);

    isRunning = false;

    return 0;
}

WavPlay::WavPlay(Shell &shell) : Command(shell) {

}

void WavPlay::execute(Util::Array<String> &args) {
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

    File *file = File::open(absolutePath, "r");

    if(file == nullptr) {
        stderr << args[0] << ": Unable to open file '" << parser.getUnnamedArguments()[0] << "'!" << endl;
        return;
    }

    auto *soundService = Kernel::getService<SoundService>();

    if(!soundService->isPcmAudioAvailable()) {
        stderr << args[0] << ": No device for PCM playback available!" << endl;
        return;
    }

    stdout << "Playing '" << file->getName() << "'." << endl;
    stdout << "Press <RETURN> to stop." << endl;

    Kernel::getService<EventBus>()->subscribe(*this, KeyEvent::TYPE);

    auto *thread = new WorkerThread<File*, uint32_t>(playback, file, nullptr);

    isRunning = true;

    thread->start();

    while(isRunning);

    Kernel::getService<EventBus>()->unsubscribe(*this, KeyEvent::TYPE);
}

const String WavPlay::getHelpText() {
    return "Plays a WAV-file.\n\n"
           "Usage: wavplay [OPTION]... [PATH]\n\n"
           "Options:\n"
           "  -h, --help: Show this help-message";
}

void WavPlay::onEvent(const Event &event) {
    auto &keyEvent = (KeyEvent&) event;

    if(keyEvent.getKey().scancode() == KeyEvent::RETURN) {
        Kernel::getService<SoundService>()->getPcmAudioDevice()->stopPlayback();
    }
}
