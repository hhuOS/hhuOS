#ifndef HHUOS_WAV_H
#define HHUOS_WAV_H

#include <lib/file/File.h>
#include <lib/sound/Pcm.h>

class Wav : public Pcm {

private:

    enum WavFormat : uint16_t {
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

    struct FormatChunk {
        char formatSignature[4];
        uint32_t chunkSize;
        WavFormat wavFormat;
        uint16_t numChannels;
        uint32_t samplesPerSecond;
        uint32_t bytesPerSecond;
        uint16_t frameSize;
        uint16_t bitsPerSample;
    } __attribute__ ((packed));

    struct DataChunk {
        char dataSignature[4];
        uint32_t chunkSize;
    } __attribute__ ((packed));

    struct RiffChunk {
        char riffSignature[4];
        uint32_t chunkSize;
        char waveSignature[4];
        FormatChunk formatChunk;
        DataChunk dataChunk;
    } __attribute__ ((packed));

private:

    char *rawData = nullptr;

private:

    void processData();

public:

    explicit Wav(File *file);

    Wav(const Wav &copy) = delete;

    ~Wav();

};

#endif
