#ifndef HHUOS_SOUNDBLASTER1_H
#define HHUOS_SOUNDBLASTER1_H

#include "SoundBlaster.h"

class SoundBlaster1 : public SoundBlaster, public InterruptHandler {

private:

    bool receivedInterrupt = false;

    bool stopPlaying = false;

private:

    void prepareDma(uint16_t addressOffset, uint32_t bufferSize);

    void setSamplingRate(uint16_t samplingRate);

    void setBufferSize(uint32_t bufferSize);

    void ackInterrupt();

public:

    explicit SoundBlaster1(uint16_t baseAddress);

    ~SoundBlaster1() override = default;

    void playPcmData(const Pcm &pcm) override;

    void stopPlayback() override;

    void plugin();

    void trigger() override;
};

#endif
