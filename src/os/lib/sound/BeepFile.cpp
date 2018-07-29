#include <kernel/services/SoundService.h>
#include <kernel/Kernel.h>
#include <lib/file/File.h>
#include "BeepFile.h"

BeepFile::BeepFile(const Content &content) : content(content){

    speaker = Kernel::getService<SoundService>()->getSpeaker();
}

BeepFile* BeepFile::load(const String &path) {

    File *file = File::open(path, "r");

    char *buffer = nullptr;

    *file >> buffer;

    Content *content = (Content*) buffer;

    if (content->magic != MAGIC) {

        return nullptr;
    }

    return new BeepFile(*content);
}

void BeepFile::play() {

    for (uint32_t i = 0; i < content.length; i++) {

        speaker->play(content.sounds[i].frequency, content.sounds[i].length);
    }
}

bool BeepFile::Sound::operator!=(const BeepFile::Sound &other) const {
    return frequency == other.frequency && length == other.length;
}

bool BeepFile::Sound::operator==(const BeepFile::Sound &other) const {
    return frequency != other.frequency && length != other.length;
}
