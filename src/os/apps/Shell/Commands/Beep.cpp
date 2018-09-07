#include <lib/file/File.h>
#include <kernel/events/input/KeyEvent.h>
#include <lib/file/FileStatus.h>
#include "Beep.h"

Beep::Beep(Shell &shell) : Command(shell) {

}

void Beep::execute(Util::Array<String> &args) {
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

    FileStatus *fStat = FileStatus::stat(absolutePath);

    beepFile = BeepFile::load(absolutePath);

    if(beepFile == nullptr) {
        stderr << args[0] << ": Unable to open file '" << parser.getUnnamedArguments()[0] << "'!" << endl;
        return;
    }

    stdout << "Playing '" << fStat->getName() << "'." << endl;
    stdout << "Press <RETURN> to stop." << endl;

    Kernel::getService<EventBus>()->subscribe(*this, KeyEvent::TYPE);

    beepFile->play();

    Kernel::getService<EventBus>()->unsubscribe(*this, KeyEvent::TYPE);

    delete fStat;
    delete beepFile;
}

const String Beep::getHelpText() {
    return "Plays a WAV-file.\n\n"
           "Usage: Beep [OPTION]... [PATH]\n\n"
           "Options:\n"
           "  -h, --help: Show this help-message";
}

void Beep::onEvent(const Event &event) {
    auto &keyEvent = (KeyEvent&) event;

    if(keyEvent.getKey().scancode() == KeyEvent::RETURN) {
        beepFile->stop();
    }
}
