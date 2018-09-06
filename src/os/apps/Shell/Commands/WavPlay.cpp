#include <lib/file/wav/Wav.h>
#include <kernel/services/SoundService.h>
#include <kernel/events/input/KeyEvent.h>
#include <kernel/threads/WorkerThread.h>
#include "WavPlay.h"

uint32_t playback(File* const& file) {
    Wav wav(file);
    Kernel::getService<SoundService>()->getPcmAudioDevice()->playPcmData(wav);

    return 0;
}

WavPlay::WavPlay(Shell &shell) : Command(shell) {

}

void WavPlay::execute(Util::Array<String> &args) {
    ArgumentParser parser(getHelpText(), 1);

    if(!parser.parse(args)) {
        stderr << args[0] << ": " << parser.getErrorString() << endl;
        return;
    }

    if(parser.getUnnamedArguments().length() == 0) {
        stderr << args[0] << ": Missing operand!" << endl;
        return;
    }

    String absolutePath = calcAbsolutePath(parser.getUnnamedArguments()[0]);

    File *file = File::open(absolutePath, "r");

    if(file == nullptr) {
        stderr << args[0] << ": Unable to open file '" << parser.getUnnamedArguments()[0] << "'!" << endl;
        return;
    }

    auto *soundService = Kernel::getService<SoundService>();

    if(!soundService->isPcmAudioAvailable()) {
        stderr << args[0] << ": No device for PCM playback available!" << endl;
        return;
    }

    stdout << "Playing '" << file->getName() << "'." << endl;
    stdout << "Press <RETURN> to stop." << endl;

    Kernel::getService<EventBus>()->subscribe(*this, KeyEvent::TYPE);

    auto *thread = new WorkerThread<File*, uint32_t>(playback, file, nullptr);

    isRunning = true;

    thread->start();

    while(isRunning);

    Kernel::getService<EventBus>()->unsubscribe(*this, KeyEvent::TYPE);
}

const String WavPlay::getHelpText() {
    return "Plays a WAV-file.\n\n"
           "Usage: wavplay [OPTION]... [PATH]\n\n"
           "Options:\n"
           "  -h, --help: Show this help-message";
}

void WavPlay::onEvent(const Event &event) {
    auto &keyEvent = (KeyEvent&) event;

    if(keyEvent.getKey().scancode() == KeyEvent::RETURN) {
        Kernel::getService<SoundService>()->getPcmAudioDevice()->stopPlayback();
        isRunning = false;
    }
}
