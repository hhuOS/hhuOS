#ifndef HHUOS_SOUNDBLASTER16_H
#define HHUOS_SOUNDBLASTER16_H

#include <cstdint>
#include <kernel/IOport.h>
#include <kernel/services/TimeService.h>
#include <kernel/interrupts/Pic.h>
#include <devices/Isa.h>
#include "PcmAudioDevice.h"

class SoundBlaster : public PcmAudioDevice, public InterruptHandler {

private:

    struct FeatureSet {
        bool monoPcm8Bit = false;
        bool monoAdpcm8Bit = false;
        bool stereoPcm8Bit = false;
        bool monoPcm16Bit = false;
        bool stereoPcm16Bit = false;
        bool dmaSingleCycle = false;
        bool dmaAutoInitialize = false;
        bool dmaHighSpeedSingleCycle = false;
        bool dmaHighSpeedAutoInitialize = false;

        FeatureSet() = default;

        FeatureSet(uint8_t majorVersion, uint8_t minorVersion) {
            if(majorVersion < 2) {
                monoPcm8Bit = true;
                monoAdpcm8Bit = true;
                dmaSingleCycle = true;
            } else if(majorVersion < 3) {
                monoPcm8Bit = true;
                monoAdpcm8Bit = true;
                dmaSingleCycle = true;
                dmaAutoInitialize = true;

                if(minorVersion > 0) {
                    dmaHighSpeedSingleCycle = true;
                    dmaHighSpeedAutoInitialize = true;
                }
            } else if(majorVersion < 4) {
                monoPcm8Bit = true;
                monoAdpcm8Bit = true;
                stereoPcm16Bit = true;
                dmaSingleCycle = true;
                dmaAutoInitialize = true;
                dmaHighSpeedSingleCycle = true;
                dmaHighSpeedAutoInitialize = true;
            } else {
                monoPcm8Bit = true;
                monoAdpcm8Bit = true;
                stereoPcm16Bit = true;
                dmaSingleCycle = true;
                dmaAutoInitialize = true;
            }
        }
    };

private:

    static Logger &log;

    uint16_t baseAddress;

    uint8_t majorVersion;
    uint8_t minorVersion;

    bool receivedInterrupt;

    void *dmaMemory = nullptr;

    Spinlock cycleLock;

    FeatureSet featureSet;

    // Supported on all DSP-versions
    IOport resetPort;
    IOport readDataPort;
    IOport writeDataPort;
    IOport readBufferStatusPort;

    // Only supported on DSP-versions >= 3 (SoundBlaster Pro), or higher
    IOport mixerAddressPort;
    IOport mixerDataPort;

    TimeService *timeService = nullptr;

    static const constexpr uint16_t FIRST_BASE_ADDRESS = 0x210;
    static const constexpr uint16_t LAST_BASE_ADDRESS = 0x280;
    static const constexpr uint32_t TIMEOUT = 10;

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

    uint8_t readFromADC();

    void writeToDAC(uint8_t value);

    uint8_t ackInterrupt();

    void prepareDma(Isa::TransferMode transferMode, uint16_t dataSize);

    void dspSetSampleRate(uint16_t numChannels, uint32_t samplesPerSecond);

    void dspSetBufferSize(uint16_t dataSize);

    void play8BitPcm(const Pcm &pcm);

    void play16BitPcm(const Pcm &pcm);

    void plugin();

public:

    static bool isAvailable();

    SoundBlaster();

    ~SoundBlaster() override = default;

    void turnSpeakerOn();

    void turnSpeakerOff();

    String getVendorName() override;

    String getDeviceName() override;

    void setup() override;

    void playPcmData(const Pcm &pcm) override;

    void trigger() override;
};

#endif
