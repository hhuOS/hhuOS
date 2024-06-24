#include <cstdint>
#include "lib/util/io/file/File.h"
#include "lib/util/io/stream/FileOutputStream.h"
#include "lib/util/io/stream/FileInputStream.h"
#include "lib/util/base/System.h"
#include "lib/util/sound/WaveFile.h"
#include "lib/util/async/Thread.h"
#include "lib/util/time/Timestamp.h"
#include "lib/util/graphic/Ansi.h"
#include "lib/util/async/FunctionPointerRunnable.h"
#include "lib/util/io/stream/FileInputStream.h"
#include "lib/util/io/key/KeyDecoder.h"
#include "lib/util/math/Math.h"
#include "lib/util/usb/sound/SoundControlHelper.h"
#include "lib/util/usb/io_control/AudioControl.h"
#include "lib/util/usb/input/InputEvents.h"
#include "lib/util/base/ArgumentParser.h"

void print_process(uint32_t data_size, uint32_t remaining, uint16_t max_col){
    uint32_t width = (data_size / (max_col - 2));
    uint32_t fill  = (data_size - remaining) / width;
    Util::System::out << "[";
    for(uint32_t i = 0; i < fill; i++){
        Util::System::out << "#";
    }
    for(uint32_t i = 0; i < (max_col-2)-fill; i++){
        Util::System::out << ".";
    }
    Util::System::out << "]" << Util::Io::PrintStream::flush;
}

bool running = true;
bool exit    = false;

int32_t main(uint32_t argc, char* argv[]){
    Util::Io::File file = Util::Io::File("/device/audio0");
    if(!file.exists()){
        Util::System::error << "Device not present !" << Util::Io::PrintStream::endl << Util::Io::PrintStream::flush;
        return -1;
    }
    Util::ArgumentParser arg_parser = Util::ArgumentParser();
    arg_parser.setHelpText("Usage: playusb [FILE]\n");

    if(!arg_parser.parse(argc, argv)){
        Util::System::error << arg_parser.getErrorString() 
                << Util::Io::PrintStream::endl << Util::Io::PrintStream::flush;
        return -1;
    }
    auto arg = arg_parser.getUnnamedArguments();
    if(arg.length() == 0){
        Util::System::error << "No file passed as argument ..."
                << Util::Io::PrintStream::endl << Util::Io::PrintStream::flush;
        return -1;
    }
    Util::String file_string = arg[0];
    Util::Io::File f         = Util::Io::File(file_string);

    if(!f.exists()){
        Util::System::error << "File passed in as argument, does not exist ..."
                << Util::Io::PrintStream::endl << Util::Io::PrintStream::flush;
        return -1;
    }

    Util::Sound::WaveFile wave = Util::Sound::WaveFile(f);
    uint32_t wav_bit_depth = wave.getBitsPerSample();
    uint32_t wav_num_ch    = wave.getNumChannels();

    uint32_t freq, frame_size, sub_frame_size, bit_depth, total_freq, num_channels;
    file.control(GET_FREQ, {IN_TERMINAL_SELECT, (uint32_t)&freq});
    file.control(GET_FRAME_SIZE, {IN_TERMINAL_SELECT, (uint32_t)&frame_size});
    file.control(GET_SUB_FRAME_SIZE, {IN_TERMINAL_SELECT, (uint32_t)&sub_frame_size});
    file.control(GET_BIT_DEPTH, {IN_TERMINAL_SELECT, (uint32_t)&bit_depth});
    file.control(GET_TOTAL_FREQ, {IN_TERMINAL_SELECT, (uint32_t)&total_freq});
    file.control(GET_NUM_CHANNELS, {IN_TERMINAL_SELECT, (uint32_t)&num_channels});

    if(wav_bit_depth != bit_depth){
        Util::System::error << Util::String::format("Bit depth mismatch : passed in %u , but expected %u", 
        wav_bit_depth, bit_depth) << Util::Io::PrintStream::endl << Util::Io::PrintStream::flush;
        return -1;
    }
    if(wav_num_ch != num_channels){
        Util::System::error << Util::String::format("Channel mismatch : passed in %u , but expected %u",
        wav_num_ch, num_channels) << Util::Io::PrintStream::endl << Util::Io::PrintStream::flush;
        return -1;
    }

    //file.control(SET_FREQ, {IN_TERMINAL_SELECT, wave.getSamplesPerSecond()});
    file.control(OPEN, {IN_TERMINAL_SELECT});
    
    Util::System::out << Util::String::format("Frequency    : %u-HZ", freq) << Util::Io::PrintStream::endl;
    Util::System::out << Util::String::format("Frame size   : %u-kB", frame_size / 1000) << Util::Io::PrintStream::endl; 
    Util::System::out << Util::String::format("Sub-frame    : %u", sub_frame_size) << Util::Io::PrintStream::endl;
    Util::System::out << Util::String::format("Bit depth    : %u", bit_depth) << Util::Io::PrintStream::endl;
    Util::System::out << Util::String::format("Num channels : %u", num_channels) << Util::Io::PrintStream::endl 
        << Util::Io::PrintStream::endl << Util::Io::PrintStream::flush;

    uint32_t total_bytes_per_second = frame_size;
    uint32_t data_size = wave.getDataSize();
    uint32_t remaining = wave.getDataSize();
    //uint32_t sample_count = wave.getSampleCount();

    uint32_t seconds = data_size / total_bytes_per_second;
    uint32_t minutes = seconds / 60;

    Util::System::out << Util::String::format("Start playing song : %s # Duration : %02u-%02um", 
        (char*)f.getName(), minutes, seconds % 60) 
        << Util::Io::PrintStream::endl << Util::Io::PrintStream::flush;
    Util::System::out << "m : mute , u : unmute , +/- : vol up/down, e : exit , p : pause/play" 
        << Util::Io::PrintStream::endl << Util::Io::PrintStream::endl 
        << Util::Io::PrintStream::flush;

    Util::Io::FileOutputStream audioOutputStream = Util::Io::FileOutputStream(file);
    
    Util::Async::Thread::createThread("Key-Listener", new Util::Async::FunctionPointerRunnable([]{
        Util::Io::File file = Util::Io::File("/device/audio0");
        auto keyboardStream = Util::Io::FileInputStream("/device/keyboard0");
        auto keyDecoder = Util::Io::KeyDecoder();
        int16_t cur_sound, max_sound, min_sound, wVolume;

        file.control(GET_SOUND, {IN_TERMINAL_SELECT, (uint32_t)(uintptr_t)&cur_sound});
        //Util::System::out << Util::String::format("Cur volume : %d", cur_sound) << Util::Io::PrintStream::endl << Util::Io::PrintStream::flush;
        file.control(GET_SOUND_MIN, {IN_TERMINAL_SELECT, (uint32_t)(uintptr_t)&min_sound});
        //Util::System::out << Util::String::format("Min volume : %d", min_sound) << Util::Io::PrintStream::endl << Util::Io::PrintStream::flush;
        file.control(GET_SOUND_MAX, {IN_TERMINAL_SELECT, (uint32_t)(uintptr_t)&max_sound});
        //Util::System::out << Util::String::format("Max volume : %d", max_sound) << Util::Io::PrintStream::endl << Util::Io::PrintStream::flush;
        
        uint8_t  total_changes = 10;
        uint32_t diff          = Util::Math::absolute(min_sound - max_sound);
        uint16_t sound_step    = diff / total_changes;
        
        for(;;){
            int16_t modifiers = keyboardStream.read();
            int16_t keytype = keyboardStream.read();
            int16_t keycode = keyboardStream.read();
            keyDecoder.parseUsbCodes(modifiers, (uint8_t)Util::Io::KeyDecoder::KEY_MODIFIER);
            keyDecoder.parseUsbCodes(keytype, (uint8_t)Util::Io::KeyDecoder::KEY_TYPE);
            keyDecoder.parseUsbCodes(keycode, (uint8_t)Util::Io::KeyDecoder::KEY_CODE);

            auto key = keyDecoder.getCurrentKey().getScancode();
            if(keytype == KEY_PRESSED){
                if(key == KEY_M){
                file.control(MUTE, {IN_TERMINAL_SELECT});
                }
                else if(key == KEY_U){
                    file.control(UNMUTE, {IN_TERMINAL_SELECT});
                }
                else if(key == KEY_PLUS){
                    cur_sound += sound_step;
                    wVolume = cur_sound;
                    file.control(SET_SOUND, {IN_TERMINAL_SELECT, (uint32_t)wVolume});
                }
                else if(key == KEY_MINUS){
                    cur_sound -= sound_step;
                    wVolume = cur_sound;
                    file.control(SET_SOUND, {IN_TERMINAL_SELECT, (uint32_t)wVolume});
                }
                else if(key == KEY_P){
                    running ^= 1;
                }
                else if(key == KEY_E){
                    exit = true;
                }
            }
        }
    }));
    
    //Util::System::out << remaining << Util::Io::PrintStream::endl << Util::Io::PrintStream::flush;
    //Util::System::out << sample_count << Util::Io::PrintStream::endl << Util::Io::PrintStream::flush;
    uint8_t* file_buffer = new uint8_t[total_bytes_per_second];
    
    Util::Graphic::Ansi::disableCursor();
    uint16_t max_col = Util::Graphic::Ansi::getCursorLimits().column;

    while(!exit && remaining > 0){
        if(running){
            uint32_t toWrite = remaining >= total_bytes_per_second ? total_bytes_per_second : remaining;
            wave.read(file_buffer, 0, toWrite);
            audioOutputStream.write(file_buffer, 0, toWrite);
            remaining -= toWrite;
            Util::Graphic::Ansi::saveCursorPosition();
            print_process(data_size, remaining, max_col);
            Util::Graphic::Ansi::restoreCursorPosition();
        }
    }
    Util::Graphic::Ansi::clearLine();
    Util::Graphic::Ansi::moveCursorToBeginningOfNextLine(0);
    Util::Graphic::Ansi::enableCursor();

    file.control(CLOSE, {IN_TERMINAL_SELECT});

    return 1;
}
