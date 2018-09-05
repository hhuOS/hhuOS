#ifndef HHUOS_PCMAUDIODEVICE_H
#define HHUOS_PCMAUDIODEVICE_H

#include <lib/String.h>

class PcmAudioDevice {

protected:

    PcmAudioDevice() = default;

public:

    PcmAudioDevice(const PcmAudioDevice &copy) = delete;

    ~PcmAudioDevice() = default;

    virtual String getVendorName() = 0;

    virtual String getDeviceName() = 0;

    virtual void setup() = 0;

    virtual void turnSpeakerOn() = 0;

    virtual void turnSpeakerOff() = 0;

    virtual void playSample(uint8_t sample) = 0;

    virtual void playSample(uint16_t sample) = 0;
};

#endif
