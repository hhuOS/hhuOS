#ifndef HHUOS_SOUNDBLASTER_H
#define HHUOS_SOUNDBLASTER_H

#include <cstdint>
#include <kernel/IOport.h>
#include <kernel/services/TimeService.h>
#include <devices/sound/PcmAudioDevice.h>

class SoundBlaster : public PcmAudioDevice {

protected:

    static Logger &log;

    uint16_t baseAddress;

    uint8_t majorVersion;
    uint8_t minorVersion;

    Spinlock soundLock;

    void *dmaMemory = nullptr;

    IOport resetPort;
    IOport readDataPort;
    IOport writeDataPort;
    IOport readBufferStatusPort;

    TimeService *timeService = nullptr;

private:

    static const constexpr uint16_t FIRST_BASE_ADDRESS = 0x220;
    static const constexpr uint16_t LAST_BASE_ADDRESS = 0x280;
    static const constexpr uint32_t TIMEOUT = 10;

    static const constexpr char *VENDOR_NAME = "Creative Technology";

    static const constexpr char *SOUND_BLASTER_1 = "Sound Blaster 1.0";
    static const constexpr char *SOUND_BLASTER_2 = "Sound Blaster 2.0";
    static const constexpr char *SOUND_BLASTER_PRO = "Sound Blaster Pro";
    static const constexpr char *SOUND_BLASTER_16 = "Sound Blaster 16";
    static const constexpr char *SOUND_BLASTER_AWE32 = "Sound Blaster AWE32";

private:

    static bool checkPort(uint16_t baseAddress);

    static uint16_t getBasePort();

protected:

    explicit SoundBlaster(uint16_t baseAddress);

    bool reset();

    uint8_t readFromDSP();

    void writeToDSP(uint8_t value);

    uint8_t readFromADC();

    void writeToDAC(uint8_t value);

    void turnSpeakerOn();

    void turnSpeakerOff();

public:

    static bool isAvailable();

    static SoundBlaster *initialize();

    ~SoundBlaster() = default;

    String getVendorName() override;

    String getDeviceName() override;

    void playPcmData(const Pcm &pcm) override = 0;

    void stopPlayback() override = 0;

};

#endif
