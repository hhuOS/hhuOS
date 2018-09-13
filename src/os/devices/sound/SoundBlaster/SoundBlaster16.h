#ifndef HHUOS_SOUNDBLASTER16_H
#define HHUOS_SOUNDBLASTER16_H

#include "SoundBlaster.h"

class SoundBlaster16 : public SoundBlaster, public InterruptHandler {

private:

    IOport mixerAddressPort;
    IOport mixerDataPort;

    bool receivedInterrupt = false;

    bool stopPlaying = false;

private:

    void prepareDma(uint16_t addressOffset, uint32_t bufferSize, bool bits16, bool autoInitialize = true);

    void setSamplingRate(uint16_t samplingRate);

    void setBufferSize(uint32_t bufferSize, bool stereo, bool bits16);

    void stopAutoInitialize(bool bits16);

    void ackInterrupt();

public:

    explicit SoundBlaster16(uint16_t baseAddress);

    ~SoundBlaster16() override = default;

    void playPcmData(const Pcm &pcm) override;

    void stopPlayback() override;

    void plugin();

    void trigger() override;
};

#endif
