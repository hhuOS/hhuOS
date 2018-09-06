#include <devices/sound/SoundBlaster.h>
#include "SoundService.h"

Logger &SoundService::log = Logger::get("SOUND");

SoundService::SoundService() {
    log.info("Found audio device: PC Speaker");
    pcSpeaker = new PcSpeaker();

    if(SoundBlaster::isAvailable()) {
        log.info("Found audio device: SoundBlaster");

        auto *soundBlaster = new SoundBlaster();
        soundBlaster->setup();

        setPcmAudioDevice(soundBlaster);
    }
}

bool SoundService::isPcmAudioAvailable() {
    return pcmAudioDevice != nullptr;
}

PcSpeaker* SoundService::getPcSpeaker() {
    return pcSpeaker;
}

PcmAudioDevice *SoundService::getPcmAudioDevice() {
    return pcmAudioDevice;
}

void SoundService::setPcmAudioDevice(PcmAudioDevice *newDevice) {
    pcmAudioDevice = newDevice;

    log.info("PCM Audio Device is now set to '%s' by '%s'", (const char*) pcmAudioDevice->getDeviceName(),
             (const char*) pcmAudioDevice->getVendorName());
}
