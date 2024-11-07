#include <stdio.h>
#include <time.h>
#include <string.h>
#include <ctype.h>
#include <math.h>

#include "doomgeneric/doomgeneric/doomtype.h"
#include "doomgeneric/doomgeneric/i_video.h"
#include "doomgeneric/doomgeneric/doomgeneric.h"
#include "doomgeneric/doomgeneric/doomkeys.h"
#include "doomgeneric/doomgeneric/doomtype.h"
#include "doomgeneric/doomgeneric/i_sound.h"

#include "lib/util/graphic/Ansi.h"
#include "lib/util/graphic/LinearFrameBuffer.h"
#include "lib/util/io/file/File.h"
#include "lib/util/base/Address.h"
#include "lib/util/async/Thread.h"
#include "lib/util/time/Timestamp.h"
#include "lib/util/io/key/KeyDecoder.h"
#include "lib/util/io/key/layout/DeLayout.h"
#include "lib/util/sound/PcSpeaker.h"
#include "lib/util/graphic/BufferedLinearFrameBuffer.h"

uint32_t palette[256];
Util::Graphic::LinearFrameBuffer *lfb;
Util::Graphic::BufferedLinearFrameBuffer *bufferedlfb;
Util::Io::KeyDecoder *kd;

void (*drawFrame)();

void DG_DrawFrame32() {
    if (bufferedlfb == nullptr) {
        return;
    }

    auto screenBuffer = reinterpret_cast<uint32_t*>(bufferedlfb->getBuffer().get());

    for (uint32_t i = 0; i < DOOMGENERIC_RESX * DOOMGENERIC_RESY; i++) {
        auto pixel = DG_ScreenBuffer[i];
        auto color = palette[pixel];

        screenBuffer[i] = color;
    }
}

void DG_DrawFrame24() {
    if (bufferedlfb == nullptr) {
        return;
    }

    auto screenBuffer = reinterpret_cast<uint8_t*>(bufferedlfb->getBuffer().get());

    for (uint32_t i = 0; i < DOOMGENERIC_RESX * DOOMGENERIC_RESY; i++) {
        auto pixel = DG_ScreenBuffer[i];
        auto color = palette[pixel];

        screenBuffer[i * 3] = color & 0xff;
        screenBuffer[i * 3 + 1] = (color >> 8) & 0xff;
        screenBuffer[i * 3 + 2] = (color >> 16) & 0xff;
    }
}

void DG_DrawFrame16() {
    if (bufferedlfb == nullptr) {
        return;
    }

    auto screenBuffer = reinterpret_cast<uint16_t*>(bufferedlfb->getBuffer().get());

    for (uint32_t i = 0; i < DOOMGENERIC_RESX * DOOMGENERIC_RESY; i++) {
        auto pixel = DG_ScreenBuffer[i];
        auto color = palette[pixel];

        screenBuffer[i] = color;
    }
}

int32_t main(int argc, char **argv) {
    if (!Util::Io::File::changeDirectory("/user/doom")) {
        Util::System::error << "doomgeneric: '/user/doom' not found!" << Util::Io::PrintStream::endl << Util::Io::PrintStream::flush;
        return -1;
    }

	doomgeneric_Create(argc, argv);

    // Prepare graphics
    Util::Graphic::Ansi::prepareGraphicalApplication(true);
    auto lfbFile = new Util::Io::File("/device/lfb");

    // If '-res' is given, try to change display resolution
    for (int32_t i = 0; i < argc; i++) {
        if (!strcmp(argv[i], "-res") && i < argc - 1) {
            auto split1 = Util::String(argv[i + 1]).split("x");
            auto split2 = split1[1].split("@");

            uint32_t resolutionX = Util::String::parseInt(split1[0]);
            uint32_t resolutionY = Util::String::parseInt(split2[0]);
            uint32_t colorDepth = split2.length() > 1 ? Util::String::parseInt(split2[1]) : 32;

            lfbFile->controlFile(Util::Graphic::LinearFrameBuffer::SET_RESOLUTION, Util::Array<uint32_t>({resolutionX, resolutionY, colorDepth}));
            break;
        }
    }

    // Use double buffering to improve tearing and automatically scale the image to the screen
    lfb = new Util::Graphic::LinearFrameBuffer(*lfbFile);
    bufferedlfb = new Util::Graphic::BufferedLinearFrameBuffer(*lfb, static_cast<uint16_t>(DOOMGENERIC_RESX), static_cast<uint16_t>(DOOMGENERIC_RESY));

    // Generate color palette
    for (uint32_t i = 0; i < 256; i++) {
        auto color = Util::Graphic::Color(colors[i].r, colors[i].g, colors[i].b, colors[i].a);
        palette[i] = color.getColorForDepth(lfb->getColorDepth());
    }

    // Set draw method based on color depth
    switch (lfb->getColorDepth()) {
        case 32:
            drawFrame = &DG_DrawFrame32;
            break;
        case 24:
            drawFrame = &DG_DrawFrame24;
            break;
        case 15:
        case 16:
            drawFrame = &DG_DrawFrame16;
            break;
        default:
            Util::Exception::throwException(Util::Exception::UNSUPPORTED_OPERATION, "Unsupported color depth!");
    }

    lfb->clear();

    // Run game loop
    auto oldTime = clock();
    while (true) {
        auto newTime = clock();
        if (oldTime == newTime) {
            Util::Async::Thread::yield();
        } else {
            doomgeneric_Tick();
            oldTime = newTime;
        }
    }
}

void DG_Init() {
	kd = new Util::Io::KeyDecoder(new Util::Io::DeLayout());
}

void DG_DrawFrame() {
    if (bufferedlfb == nullptr) {
        return;
    }

    drawFrame();
    bufferedlfb->flush();
}

void DG_SleepMs(uint32_t ms) {
	Util::Async::Thread::sleep(Util::Time::Timestamp::ofMilliseconds(ms));
}

uint32_t DG_GetTicksMs() {
	return clock();
}

int DG_GetKey(int * pressed, unsigned char * key) {
	if (!stdin->isReadyToRead()) {
		return 0;
	} else {
		uint8_t scancode = fgetc(stdin);
		
		if ((scancode & ~0x80) == 0x1d) {
			*pressed = !(scancode & 0x80);
			*key = KEY_FIRE;
			return 1;
		}
		if ((scancode & ~0x80) == 0x38) {
			*pressed = !(scancode & 0x80);
			*key = KEY_LALT;
			return 1;
		}
		
		
		if (kd->parseScancode(scancode)) {
			auto k = kd->getCurrentKey();
			if (!k.isValid()) {
                return 0;
            }
			
			*pressed = k.isPressed() ? 1:0; 
			
			if (k.getScancode() >= 0x3b && k.getScancode() <= 0x44) { // handle F1-10
				*key = k.getScancode() + 0x80;
				return 1;
			}
			
			switch(k.getScancode()) {
				case Util::Io::Key::UP:
					*key = KEY_UPARROW;
					return 1;
				case Util::Io::Key::DOWN:
					*key = KEY_DOWNARROW;
					return 1;
				case Util::Io::Key::LEFT:
					*key = KEY_LEFTARROW;
					return 1;
				case Util::Io::Key::RIGHT:
					*key = KEY_RIGHTARROW;
					return 1;
				case Util::Io::Key::SPACE:
					*key = KEY_USE;
					return 1;
				case Util::Io::Key::ESC:
					*key = KEY_ESCAPE;
					return 1;
				case Util::Io::Key::ENTER:
					*key = KEY_ENTER;
					return 1;
				case Util::Io::Key::TAB:
					*key = KEY_TAB;
					return 1;
				case Util::Io::Key::BACKSPACE:
					*key = KEY_BACKSPACE;
					return 1;
                case Util::Io::Key::HOME:
                    *key = KEY_HOME;
                    return 1;
                case Util::Io::Key::END:
                    *key = KEY_END;
                    return 1;
                case Util::Io::Key::INSERT:
                    *key = KEY_INS;
                    return 1;
                case Util::Io::Key::DEL:
                    *key = KEY_DEL;
                    return 1;
                case Util::Io::Key::PAGE_UP:
                    *key = KEY_PGUP;
                    return 1;
                case Util::Io::Key::PAGE_DOWN:
                    *key = KEY_PGDN;
                    return 1;
                case Util::Io::Key::F1:
                    *key = KEY_F1;
                    return 1;
                case Util::Io::Key::F2:
                    *key = KEY_F2;
                    return 1;
                case Util::Io::Key::F3:
                    *key = KEY_F3;
                    return 1;
                case Util::Io::Key::F4:
                    *key = KEY_F4;
                    return 1;
                case Util::Io::Key::F5:
                    *key = KEY_F5;
                    return 1;
                case Util::Io::Key::F6:
                    *key = KEY_F6;
                    return 1;
                case Util::Io::Key::F7:
                    *key = KEY_F7;
                    return 1;
                case Util::Io::Key::F8:
                    *key = KEY_F8;
                    return 1;
                case Util::Io::Key::F9:
                    *key = KEY_F9;
                    return 1;
                case Util::Io::Key::F10:
                    *key = KEY_F10;
                    return 1;
                case Util::Io::Key::F11:
                    *key = KEY_F11;
                    return 1;
                case Util::Io::Key::F12:
                    *key = KEY_F12;
                    return 1;
				default:
					if (k.getAscii()) {
                        *key = tolower(k.getAscii());
                        return 1;
                    }

                    return 0;
			}
		}
		
	}
	return 0;
}

void DG_SetWindowTitle(const char * title) {}

Util::Sound::PcSpeaker *speaker;

bool I_SDL_InitMusic() {
	speaker = new Util::Sound::PcSpeaker(Util::Io::File("/device/speaker"));
	return true;
}

void I_SDL_ShutdownMusic() {
	speaker->turnOff();
	delete speaker;
}

void I_SDL_SetMusicVolume(int volume) {}

struct MUSheader {
	char ID[4];
	uint16_t scoreLen;
	uint16_t scoreStart;
	uint16_t channels;
	uint16_t sec_channels;
	uint16_t instrCnt;
	uint16_t dummy;
};

struct Song {
	uint8_t * musicData;
	uint8_t * musicDataEnd;
	uint8_t * currentPoint;
};

void * I_SDL_RegisterSong(void * data, int len) {
	MUSheader * header = (MUSheader *) data;
	uint8_t * scoreStart = (uint8_t*)data + header->scoreStart;
	
	Song * newSong = new Song();
	newSong->musicData = new uint8_t[header->scoreLen];
	newSong->musicDataEnd = newSong->musicData + header->scoreLen;
	memcpy(newSong->musicData, scoreStart, header->scoreLen);
	
	newSong->currentPoint = newSong->musicData ;
	return (void*)newSong;
}

void I_SDL_UnRegisterSong(void* handle) { 
	Song * song = (Song*)handle;
	delete [] song->musicData;
	delete song;
}


bool playing = false;
bool loop = false;
Song * current;

void I_SDL_PauseSong() {
	playing = false;
}

void I_SDL_ResumeSong() {
	playing = true;
}

int channelVolumes[16] = {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1};

clock_t waitClock = 0;

int channelPlaying = -1;
int notePlaying = -1;
int noteVolume = -1;

void I_SDL_PlaySong(void * handle, bool looping) {
	current = (Song*) handle;
	if ((current->musicDataEnd - current->musicData) != 17237) {
        speaker->turnOff();
        return; //only play Hangar 1 music
    }

	current->currentPoint = current->musicData;
	playing = true;
	loop = looping;

	for (int i = 0; i < 16; i++)channelVolumes[i] = -1;

	channelPlaying = -1;
	notePlaying = -1;
	noteVolume = -1;
}

void I_SDL_StopSong() {
	playing = false;
	current->currentPoint = current->musicData;
}

bool I_SDL_MusicIsPlaying() {
	return playing;
}

int32_t eigthOctaveFreqs[12] = { 4186, 4435,  4699, 4978, 5274, 5588, 5920, 6272, 6644, 7040, 7458 };

int freqFromNoteNum(int noteNum) {
	return eigthOctaveFreqs[noteNum % 12] * pow(0.5, 8 - (noteNum / 12));
}

int focalNote = 60;	

void I_SDL_PollMusic() { // MUS file parsing
	if (playing && clock() >= waitClock) {
		bool last = false;
		while (!last) {
			uint8_t eventDesc = *(current->currentPoint++);
			last = eventDesc & 0x80;
			int32_t channelNum = eventDesc & 0b1111;
            int32_t eventType = (eventDesc>>4) & 0b111;
			uint8_t data, vol;
			vol = channelVolumes[channelNum];

			switch (eventType) {
				case 0: // release
					data = *(current->currentPoint++);
					if (channelPlaying == channelNum && data == notePlaying) {
						speaker->turnOff();
						notePlaying = -1;
						channelPlaying = -1;
						noteVolume = -1;
					}

					break;
				case 1: // play
					 data =  *(current->currentPoint++);
					 if (data & 0x80) vol = *(current->currentPoint++); //volume info;
					 data &= ~0x80;
					 if (channelNum == 15) break; //percussion channel
					 if (notePlaying == -1 || (abs(focalNote-data) < abs(focalNote - notePlaying) && data > 30)) {
						 channelPlaying = channelNum;
						 notePlaying = data;
						 noteVolume = vol;
						 speaker->play(freqFromNoteNum(data));
					 }

					 break;
				case 2: // pitch bend - ignore
					current->currentPoint++;
					break;
				case 3: // event
					 data =  *(current->currentPoint++);
					 if (data == 10 || data == 11 || data == 14) {
                         speaker->turnOff();
                     }

					 break;
				case 4: // change controller - ignored
					data = *(current->currentPoint++);
					vol = *(current->currentPoint++);
					if (data == 3) {
                        channelVolumes[channelNum] = vol;
                    }

					break;
				case 5:
					break;
				case 6:
					I_SDL_StopSong();
					if (loop) {
                        I_SDL_PlaySong((void*)current, loop);
                    }

					return;
				case 7:
					break;
			}
		}
		
		int time = 0;
		uint8_t byte;
		do {
			byte = *(current->currentPoint++);
			time = time * 128 + (byte & 127);
		} while (byte & 128);
		
		waitClock = clock() + time * 1000 / 180; // should be 140, accelerated cause VM
	}
}

static snddevice_t music_sdl_devices[] = {
    SNDDEVICE_PCSPEAKER
};

music_module_t DG_music_module = {
    music_sdl_devices,
    arrlen(music_sdl_devices),
    I_SDL_InitMusic,
    I_SDL_ShutdownMusic,
    I_SDL_SetMusicVolume,
    I_SDL_PauseSong,
    I_SDL_ResumeSong,
    I_SDL_RegisterSong,
    I_SDL_UnRegisterSong,
    I_SDL_PlaySong,
    I_SDL_StopSong,
    I_SDL_MusicIsPlaying,
    I_SDL_PollMusic,
};