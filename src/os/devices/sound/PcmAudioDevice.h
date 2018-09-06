#ifndef HHUOS_PCMAUDIODEVICE_H
#define HHUOS_PCMAUDIODEVICE_H

#include <lib/String.h>
#include <lib/sound/Pcm.h>

class PcmAudioDevice {

protected:

    PcmAudioDevice() = default;

public:

    PcmAudioDevice(const PcmAudioDevice &copy) = delete;

    ~PcmAudioDevice() = default;

    virtual String getVendorName() = 0;

    virtual String getDeviceName() = 0;

    virtual void setup() = 0;

    virtual void playPcmData(const Pcm &pcm) = 0;

    virtual void stopPlayback() = 0;
};

#endif
