#ifndef HHUOS_PCMDATA_H
#define HHUOS_PCMDATA_H

#include <cstdint>

class Pcm {

public:

    enum AudioFormat : uint16_t {
        PCM = 0x0001,
        MS_ADPCM = 0x0002,
        IEEE_FLOAT = 0x0003,
        IBM_CVSD = 0x0005,
        ALAW = 0x0006,
        MULAW = 0x0007,
        OKI_ADPCM = 0x0010,
        DVI_IMA_ADPCM = 0x0011,
        MEDIASPACE_ADPCM = 0x0012,
        SIERRA_ADPCM = 0x0013,
        G723_ADPCM = 0x0014,
        DIGISTD = 0x0015,
        DIGIFX = 0x0016,
        DIALOGIC_OKI_ADPCM = 0x0017,
        YAMAHA_ADPCM = 0x0020,
        SONARC = 0x0021,
        DSPGROUP_TRUESPEECH = 0x0022,
        ECHOSC1 = 0x0023,
        AUDIOFILE_AF36 = 0x0024,
        APTX = 0x0025,
        AUDIOFILE_AF10 = 0x0026,
        DOLBY_AC2 = 0x0030,
        GSM610 = 0x0031,
        ANTEX_ADPCME = 0x0032,
        CONTROL_RES_VQLPC_1 = 0x0034,
        CONTROL_RES_VQLPC_2 = 0x0035,
        DIGIADPCM = 0x0036,
        CONTROL_RES_CR10 = 0x0037,
        NMS_VBXADPCM = 0x0038,
        CS_IMAADPCM = 0x0039,
        G721_ADPCM = 0x0040,
        MPEG_1_2 = 0x0050,
        MPEG_3 = 0x0055,
        XBOX_ADPCM = 0x0069,
        CREATIVE_ADPCM = 0x0200,
        CREATIVE_FASTSPEECH8 = 0x0202,
        CREATIVE_FASTSPEECH10 = 0x0203,
        FM_TOWNS_SND = 0x0300,
        OLIGSM = 0x1000,
        OLIADPCM = 0x1001,
        OLICELP = 0x1002,
        OLISBC = 0x1003,
        OLIOPR = 0x1004
    };

protected:

    AudioFormat audioFormat = PCM;
    uint16_t numChannels = 0;
    uint32_t samplesPerSecond = 0;
    uint32_t bytesPerSecond = 0;
    uint16_t bitsPerSample = 0;
    uint16_t frameSize = 0;
    uint32_t sampleCount = 0;

    uint8_t *pcmData = nullptr;

protected:

    Pcm() = default;

public:

    Pcm(uint16_t numChannels, uint32_t samplesPerSecond, uint32_t bytesPerSecond, uint16_t bitsPerSample, uint16_t frameSize, uint32_t sampleCount, uint8_t *pcmData);

    Pcm(const Pcm &copy) = delete;

    ~Pcm() = default;

    AudioFormat getAudioFormat() const;

    uint16_t getNumChannels() const;

    uint32_t getSamplesPerSecond() const;

    uint32_t getBytesPerSecond() const;

    uint16_t getBitsPerSample() const;

    uint16_t getFrameSize() const;

    uint32_t getSampleCount() const;

    uint8_t *getPcmData() const;
};

#endif
