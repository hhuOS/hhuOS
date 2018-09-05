#ifndef HHUOS_SOUNDBLASTER16_H
#define HHUOS_SOUNDBLASTER16_H

#include <cstdint>
#include <kernel/IOport.h>
#include <kernel/services/TimeService.h>
#include "PcmAudioDevice.h"

class SoundBlaster : public PcmAudioDevice {

private:

    static Logger &log;

    uint16_t baseAddress;

    uint8_t majorVersion;
    uint8_t minorVersion;

    IOport resetPort;
    IOport readDataPort;
    IOport writeDataPort;
    IOport readBufferStatusPort;

    TimeService *timeService = nullptr;

    static const constexpr uint16_t FIRST_BASE_ADDRESS = 0x210;
    static const constexpr uint32_t RESET_TIMEOUT = 10;

    static const constexpr char *VENDOR_NAME = "Creative Technology";

    static const constexpr char *SOUND_BLASTER_1 = "Sound Blaster 1.0";
    static const constexpr char *SOUND_BLASTER_2 = "Sound Blaster 2.0";
    static const constexpr char *SOUND_BLASTER_PRO = "Sound Blaster Pro";
    static const constexpr char *SOUND_BLASTER_16 = "Sound Blaster 16";
    static const constexpr char *SOUND_BLASTER_AWE32 = "Sound Blaster AWE32";

private:

    explicit SoundBlaster(uint16_t baseAddress);

    static bool checkPort(uint16_t baseAddress);

    static uint16_t getBasePort();

    bool reset();

    uint8_t readFromDSP();

    void writeToDSP(uint8_t value);

    void writeToDAC(uint8_t value);

public:

    static bool isAvailable();

    SoundBlaster();

    ~SoundBlaster() = default;

    void turnSpeakerOn();

    void turnSpeakerOff();

    String getVendorName() override;

    String getDeviceName() override;

    void setup() override;

    void playPcmData(const Pcm &pcm) override;
};

#endif
