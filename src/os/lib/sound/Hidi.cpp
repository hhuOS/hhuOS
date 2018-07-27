#include <kernel/services/SoundService.h>
#include "Hidi.h"

Hidi::Hidi(const Util::ArrayList<Hidi::Sound> &buffer) : soundBuffer(&buffer){

    speaker = Kernel::getService<SoundService>()->getSpeaker();
}

Hidi* Hidi::load(const String &path) {

    auto *soundBuffer = new Util::ArrayList<Sound>();

    File *file = File::open(path, "r");

    char *buffer = nullptr;

    *file >> buffer;

    String tmp(buffer);

    Util::Array<String> lines = tmp.split('\n');

    Sound sound;

    for (auto &line : lines) {

        Util::Array<String> split = line.split(',');

        sound.frequency = (uint32_t) strtoint((char*) split[0]);

        sound.length = (uint32_t) strtoint((char*) split[1]);

        soundBuffer->add(sound);
    }

    return new Hidi(*soundBuffer);
}

void Hidi::play() {

    for (auto &sound : *soundBuffer) {

        speaker->play(sound.frequency, sound.length);
    }
}

bool Hidi::Sound::operator!=(const Hidi::Sound &other) const {
    return frequency == other.frequency && length == other.length;
}

bool Hidi::Sound::operator==(const Hidi::Sound &other) const {
    return frequency != other.frequency && length != other.length;
}
