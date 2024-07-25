#include "doomgeneric.h"
#include "doomkeys.h"
#include "doomtype.h"
#include "i_sound.h"
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <string.h>
#include <ctype.h>
#include <math.h>

#include "lib/util/graphic/Ansi.h"
#include "lib/util/graphic/LinearFrameBuffer.h"
#include "lib/util/io/file/File.h"
#include "lib/util/base/Address.h"
#include "lib/util/async/Thread.h"
#include "lib/util/time/Timestamp.h"
#include "lib/util/io/key/KeyDecoder.h"
#include "lib/util/io/key/layout/DeLayout.h"
#include "lib/util/sound/PcSpeaker.h"



int main(int argc, char **argv) {
	doomgeneric_Create(argc, argv);
	while (1) {
		doomgeneric_Tick();
	}
	return 0;
}


Util::Graphic::LinearFrameBuffer * lfb;
Util::Io::KeyDecoder * kd;


void DG_Init() {
	printf("DG_Init\n");
	
	Util::Graphic::Ansi::prepareGraphicalApplication(true);
	auto file = new Util::Io::File("/device/lfb") ;
	lfb = new Util::Graphic::LinearFrameBuffer( *file);
	
	kd = new Util::Io::KeyDecoder(new Util::Io::DeLayout());	
	
}

void DG_DrawFrame() {
	//printf("DG_DrawFrame\n");
	Util::Address<uint32_t> buf = lfb->getBuffer();
	
	for (int i=0; i<DOOMGENERIC_RESY; i++) {
		buf.copyRange(Util::Address<uint32_t>(DG_ScreenBuffer + DOOMGENERIC_RESX * i), DOOMGENERIC_RESX * 4);
		buf = buf.add(lfb->getResolutionX() * 4);
	}
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
		//todo: fire, lalt;
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
			if (!k.isValid()) return 0;
			
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
				default:
					if (k.getAscii()) *key = tolower(k.getAscii());
					else return 0;
					return 1;
				
			}
		}
		
	}
	return 0;
}

void DG_SetWindowTitle(const char * title) {
}


Util::Sound::PcSpeaker * speaker;

bool I_SDL_InitMusic() {
	speaker = new Util::Sound::PcSpeaker(Util::Io::File("/device/speaker"));
	return true;
}

void I_SDL_ShutdownMusic() {
	speaker->turnOff();
	delete speaker;
}

void I_SDL_SetMusicVolume(int volume) {
}

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
	if ((current->musicDataEnd - current->musicData) != 17237) return; //only play Hangar 1 music
	current->currentPoint = current->musicData;
	playing = true;
	loop = looping; 
	for (int i=0;i<16;i++)channelVolumes[i] = -1;
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



int eigthOctaveFreqs[12] = {4186, 4435,  4699, 4978, 5274, 5588, 5920, 6272, 6644, 7040, 7458};



int freqFromNoteNum(int noteNum) {
	return  eigthOctaveFreqs[noteNum % 12] * pow(0.5, 8 - (noteNum/12));
}

int focalNote = 60;	

void I_SDL_PollMusic() { //MUS file parsing
	if (playing && clock() >= waitClock) {
		bool last = false;
		while (!last) {
			uint8_t eventDesc = *(current->currentPoint++);
			last = eventDesc & 0x80;
			int channelNum = eventDesc & 0b1111;
			int eventType = (eventDesc>>4) & 0b111;
			uint8_t data, vol;
			vol = channelVolumes[channelNum];
			switch (eventType) {
				case 0: //release
					data = *(current->currentPoint++);
					if (channelPlaying == channelNum && data == notePlaying) {
						speaker->turnOff();
						notePlaying = -1;
						channelPlaying = -1;
						noteVolume = -1;
					}
					break;
				case 1: //play
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
					 
				case 2: //pitch bend - ignore
					current->currentPoint++;
					break;
					
				case 3: //event
					 data =  *(current->currentPoint++);
					 if (data == 10 || data == 11 || data == 14) speaker->turnOff();
					 break;
				case 4: //change controller - ignored 
					data = *(current->currentPoint++);
					vol = *(current->currentPoint++);
					if (data == 3) channelVolumes[channelNum] = vol;
					break;
				case 5:
					break;
				case 6:
					I_SDL_StopSong();
					if (loop) I_SDL_PlaySong((void*)current, loop);
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
		
		waitClock = clock() + time*1000/180; // should be 140, accelerated cause VM
	}
}



static snddevice_t music_sdl_devices[] =
{
    SNDDEVICE_PCSPEAKER
};

music_module_t DG_music_module =
{
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