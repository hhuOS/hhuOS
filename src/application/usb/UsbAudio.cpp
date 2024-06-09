#include "UsbAudio.h"
#include <cstdint>
#include "lib/util/io/file/File.h"
#include "lib/util/io/stream/FileOutputStream.h"
#include "lib/util/io/stream/FileInputStream.h"
#include "lib/util/base/System.h"
#include "lib/util/sound/WaveFile.h"
#include "lib/util/async/Thread.h"
#include "lib/util/time/Timestamp.h"

int32_t main(int32_t argc, char* argv[]){
    Util::Io::File file = Util::Io::File("/device/audio0");
    Util::Io::File audio_file = Util::Io::File("/initrd/pcm/yorushika.pcm");
    Util::Io::File wave_file = Util::Io::File("/initrd/wav/yorushika.wav");
    if(!file.exists()){
        Util::System::error << "Device not present !" << Util::Io::PrintStream::endl << Util::Io::PrintStream::flush;
        return -1;
    }
    if(!audio_file.exists()){
        Util::System::error << "Audio file not present !" << Util::Io::PrintStream::endl << Util::Io::PrintStream::flush;
    }

    Util::Sound::WaveFile wave = Util::Sound::WaveFile(wave_file);
    
    uint32_t freq, frame_size, sub_frame_size, bit_depth, total_freq;
    file.control(0x01, {0x0E, (uint32_t)&freq});
    file.control(0x02, {0x0E, (uint32_t)&frame_size});
    file.control(0x03, {0x0E, (uint32_t)&sub_frame_size});
    file.control(0x04, {0x0E, (uint32_t)&bit_depth});
    file.control(0x05, {0x0E, (uint32_t)&total_freq});
    
    Util::System::out << freq << Util::Io::PrintStream::endl << Util::Io::PrintStream::flush;
    Util::System::out << frame_size << Util::Io::PrintStream::endl << Util::Io::PrintStream::flush;
    Util::System::out << sub_frame_size << Util::Io::PrintStream::endl << Util::Io::PrintStream::flush;
    Util::System::out << bit_depth << Util::Io::PrintStream::endl << Util::Io::PrintStream::flush;
    Util::System::out << total_freq << Util::Io::PrintStream::endl << Util::Io::PrintStream::flush;

    Util::System::out << wave.getBitsPerSample() << Util::Io::PrintStream::endl << Util::Io::PrintStream::flush;
    Util::System::out << wave.getNumChannels() << Util::Io::PrintStream::endl << Util::Io::PrintStream::flush;
    Util::System::out << wave.getSamplesPerSecond() << Util::Io::PrintStream::endl << Util::Io::PrintStream::flush;

    Util::Io::FileOutputStream audioOutputStream = Util::Io::FileOutputStream(file);
    //Util::Io::FileInputStream audioInputStream = Util::Io::FileInputStream(audio_file);
    uint32_t total_bytes_per_second = freq * sub_frame_size;
    uint32_t remaining = wave.getDataSize();
    uint32_t sample_count = wave.getSampleCount();
    Util::System::out << remaining << Util::Io::PrintStream::endl << Util::Io::PrintStream::flush;
    Util::System::out << sample_count << Util::Io::PrintStream::endl << Util::Io::PrintStream::flush;
    uint8_t* file_buffer = new uint8_t[total_bytes_per_second];
    while(remaining > 0){
        uint32_t toWrite = remaining >= total_bytes_per_second ? total_bytes_per_second : remaining;
        wave.read(file_buffer, 0, toWrite);
        audioOutputStream.write(file_buffer, 0, toWrite);
        remaining -= toWrite;
    }

    return 1;
}
