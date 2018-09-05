#ifndef HHUOS_SOUNDSERVICE_H
#define HHUOS_SOUNDSERVICE_H


#include <kernel/KernelService.h>
#include <devices/sound/PcSpeaker.h>
#include <devices/sound/PcmAudioDevice.h>

class SoundService : public KernelService {

private:

    static Logger &log;

    PcSpeaker *pcSpeaker = nullptr;
    PcmAudioDevice *pcmAudioDevice = nullptr;

public:

    SoundService();

    static const constexpr char* SERVICE_NAME = "SoundService";

    PcSpeaker* getPcSpeaker();

    PcmAudioDevice* getPcmAudioDevice();

    void setPcmAudioDevice(PcmAudioDevice *newDevice);
};


#endif
