#include <kernel/Kernel.h>
#include <kernel/services/SoundService.h>
#include "Sound.h"

Sound::Sound() : Thread("Sound") {
    speaker = Kernel::getService<SoundService>()->getSpeaker();
    timeService = Kernel::getService<TimeService>();
}

void Sound::run () {
    while(true) {
        speaker->demo1();

        speaker->off();
        timeService->msleep(1000);

        speaker->demo2();

        speaker->off();
        timeService->msleep(1000);

    }
}

Sound::~Sound() {
    speaker->off();
}
