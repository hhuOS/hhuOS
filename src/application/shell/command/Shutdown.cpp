#include "lib/file/FileStatus.h"
#include "Shutdown.h"
#include "device/cpu/Cpu.h"

Shutdown::Shutdown(Shell &shell) : Command(shell) {

};

void Shutdown::execute(Util::Array<String> &args) {
    Util::ArgumentParser parser(getHelpText(), 1);

    if(!parser.parse(args)) {
        stderr << args[0] << ": " << parser.getErrorString() << endl;
        return;
    }
    if(parser.getUnnamedArguments().length() > 0) {
        stderr << args[0] << " " << parser.getEnteredCommand() << ": Invalid command" << endl;
        return;
    } else{
        IoPort dataPort(0x501);
        Cpu::disableInterrupts();
        dataPort.outb(0x45);
        return;
    }
}

const String Shutdown::getHelpText() {
    return "Shuts down the OS\n\n"
           "Usage: shutdown \n\n"
           "Options:\n"
           "  -h, --help: Show this help-message.";
}