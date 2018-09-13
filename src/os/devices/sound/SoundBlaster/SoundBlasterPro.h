#ifndef HHUOS_SOUNDBLASTERPRO_H
#define HHUOS_SOUNDBLASTERPRO_H

#include "SoundBlaster.h"

class SoundBlasterPro : public SoundBlaster, public InterruptHandler {

private:

    IOport mixerAddressPort;
    IOport mixerDataPort;

    bool receivedInterrupt = false;

    bool stopPlaying = false;

private:

    void prepareDma(uint16_t addressOffset, uint32_t bufferSize, bool autoInitialize = true);

    void setSamplingRate(uint16_t samplingRate, bool stereo);

    void setBufferSize(uint32_t bufferSize);

    void stopAutoInitialize();

    void enableLowPassFilter();

    void disableLowPassFilter();

    void enableStereo();

    void disableStereo();

    void ackInterrupt();

public:

    explicit SoundBlasterPro(uint16_t baseAddress);

    ~SoundBlasterPro() override = default;

    void playPcmData(const Pcm &pcm) override;

    void stopPlayback() override;

    void plugin();

    void trigger() override;
};

#endif
