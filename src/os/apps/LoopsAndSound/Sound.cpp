#include <kernel/Kernel.h>
#include <kernel/services/SoundService.h>
#include <lib/file/beep/BeepFile.h>
#include "Sound.h"

Sound::Sound() : Thread("Sound") {
    speaker = Kernel::getService<SoundService>()->getPcSpeaker();
    timeService = Kernel::getService<TimeService>();
}

void Sound::run () {
    while(isRunning) {

        BeepFile::load("/initrd/music/tetris.beep")->play();

        speaker->off();
        timeService->msleep(1000);

        BeepFile::load("/initrd/music/mario.beep")->play();

        speaker->off();
        timeService->msleep(1000);

    }
}

Sound::~Sound() {
    speaker->off();
}
