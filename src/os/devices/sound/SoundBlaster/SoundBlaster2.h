#ifndef HHUOS_SOUNDBLASTER2_H
#define HHUOS_SOUNDBLASTER2_H

#include "SoundBlaster.h"

class SoundBlaster2 : public SoundBlaster, public InterruptHandler {

private:

    bool receivedInterrupt = false;

    bool stopPlaying = false;

private:

    void prepareDma(uint16_t addressOffset, uint32_t bufferSize, bool autoInitialize = true);

    void setSamplingRate(uint16_t samplingRate);

    void setBufferSize(uint32_t bufferSize);

    void stopAutoInitialize();

    void ackInterrupt();

public:

    explicit SoundBlaster2(uint16_t baseAddress);

    ~SoundBlaster2() override = default;

    void playPcmData(const Pcm &pcm) override;

    void stopPlayback() override;

    void plugin();

    void trigger() override;
};

#endif
