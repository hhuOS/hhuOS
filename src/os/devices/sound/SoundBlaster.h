#ifndef HHUOS_SOUNDBLASTER16_H
#define HHUOS_SOUNDBLASTER16_H

#include <cstdint>
#include <kernel/IOport.h>
#include <kernel/services/TimeService.h>
#include <kernel/interrupts/Pic.h>
#include <devices/Isa.h>
#include <stdint-gcc.h>
#include "PcmAudioDevice.h"

class SoundBlaster : public PcmAudioDevice, public IODevice {

private:

    struct FeatureSet {
        bool mono8BitSingleCycle = false;
        bool mono8BitAutoInitialize = false;
        bool mono8BitHighSpeedSingleCycle = false;
        bool mono8BitHighSpeedAutoInitialize = false;
        bool stereo8BitHighSpeedSingleCycle = false;
        bool stereo8BitHighSpeedAutoInitialize = false;
        bool mono8Bit16BitSingleCycle = false;
        bool mono8Bit16BitAutoInitialize = false;
        bool stereo8Bit16BitSingleCycle = false;
        bool stereo8Bit16BitAutoInitialize = false;

        FeatureSet() = default;

        FeatureSet(uint8_t majorVersion, uint8_t minorVersion) {
            if(majorVersion < 2) {
                mono8BitSingleCycle = true;
            } else if(majorVersion < 3) {
                mono8BitSingleCycle = true;
                mono8BitAutoInitialize = true;

                if(minorVersion > 0) {
                    mono8BitHighSpeedSingleCycle = true;
                    mono8BitHighSpeedAutoInitialize = true;
                }
            } else if(majorVersion < 4) {
                mono8BitSingleCycle = true;
                mono8BitAutoInitialize = true;
                mono8BitHighSpeedSingleCycle = true;
                mono8BitHighSpeedAutoInitialize = true;
                stereo8BitHighSpeedSingleCycle = true;
                stereo8BitHighSpeedAutoInitialize = true;
            } else {
                mono8BitSingleCycle = true;
                mono8BitAutoInitialize = true;
                mono8Bit16BitSingleCycle = true;
                mono8Bit16BitAutoInitialize = true;
                stereo8Bit16BitSingleCycle = true;
                stereo8Bit16BitAutoInitialize = true;
            }
        }

        bool checkPcmCompatibility(const Pcm &pcm) {
            if(pcm.getAudioFormat() != Pcm::PCM) {
                return false;
            }

            if(pcm.getSamplesPerSecond() > 44100) {
                return false;
            }

            if(pcm.getSamplesPerSecond() < 4000) {
                return false;
            }

            if(mono8Bit16BitSingleCycle && pcm.getSamplesPerSecond() < 5000) {
                return false;
            }

            if(pcm.getNumChannels() > 2) {
                return false;
            }

            if(pcm.getBitsPerSample() != 8 && pcm.getBitsPerSample() != 16) {
                return false;
            }

            if(pcm.getBitsPerSample() == 16) {
                if(!mono8Bit16BitSingleCycle) {
                    return false;
                }
            }

            if(pcm.getBitsPerSample() == 8) {
                if(pcm.getSamplesPerSecond() > 23000 && !mono8BitHighSpeedSingleCycle && !mono8Bit16BitSingleCycle) {
                    return false;
                }

                if(pcm.getNumChannels() == 2) {
                    if(!stereo8BitHighSpeedSingleCycle && !stereo8Bit16BitSingleCycle) {
                        return false;
                    }

                    if(stereo8BitHighSpeedSingleCycle) {
                        if(pcm.getSamplesPerSecond() != 11025 && pcm.getSamplesPerSecond() != 22050) {
                            return false;
                        }
                    }
                }
            }

            return true;
        }
    };

    enum SetSampleRateCommand : uint8_t {
        useTimeConstant = 0x40,
        useSamplingRate = 0x41
    };

    enum SetBufferSizeCommand : uint8_t {
        mono8BitNormalSpeedSingleCycle = 0x14,
        monoStereo8Bit = 0x48,
        monoStereo8BitDspVersion4 = 0xc0,
        monoStereo16BitDspVersion4 = 0xb0,
    };

    enum SetBufferSizeMode : uint8_t {
        mono8Bit = 0x00,
        stereo8Bit = 0x20,
        mono16Bit = 0x10,
        stereo16Bit = 0x30,
        unused = 0xff
    };

private:

    static Logger &log;

    uint16_t baseAddress;

    uint8_t majorVersion;
    uint8_t minorVersion;

    bool receivedInterrupt = true;
    bool stopPlaying = false;

    void *dmaMemory = nullptr;

    Spinlock soundLock;

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

    void dspSetSampleRate(uint16_t numChannels, uint32_t samplesPerSecond, SetSampleRateCommand command);

    void dspSetBufferSize(uint16_t dataSize, SetBufferSizeCommand command, SetBufferSizeMode mode = unused,
                              bool useHighSpeed = false);

    void dspEnableStereoMode();

    void dspDisableStereoMode();

    void dspEnableLowPassFilter();

    void dspDisableLowPassFilter();

    void play8BitPcmSingleCycle(const Pcm &pcm);

    void play16BitPcmSingleCycle(const Pcm &pcm);

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

    void stopPlayback() override;

    bool checkForData() override;

    void trigger() override;
};

#endif
