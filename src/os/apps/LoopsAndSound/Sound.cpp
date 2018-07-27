#include <kernel/Kernel.h>
#include <kernel/services/SoundService.h>
#include <lib/sound/Hidi.h>
#include "Sound.h"

Sound::Sound() : Thread("Sound") {
    speaker = Kernel::getService<SoundService>()->getSpeaker();
    timeService = Kernel::getService<TimeService>();
}

void Sound::run () {
    while(true) {

        Hidi::load("/music/tetris.hidi")->play();

        speaker->off();
        timeService->msleep(1000);

        Hidi::load("/music/mario.hidi")->play();

        speaker->off();
        timeService->msleep(1000);

    }
}

Sound::~Sound() {
    speaker->off();
}
