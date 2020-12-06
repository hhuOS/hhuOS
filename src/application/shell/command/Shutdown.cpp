#include "lib/file/FileStatus.h"
#include "Shutdown.h"
#include "device/cpu/Cpu.h"

Shutdown::Shutdown(Shell &shell) : Command(shell) {

};


/* The excecute portion of the command is implemented here */
void Shutdown::execute(Util::Array<String> &args) {
    Util::ArgumentParser parser(getHelpText(), 1);

    /* Parsing the arguments */
    if(!parser.parse(args)) {
        stderr << args[0] << ": " << parser.getErrorString() << endl;
        return;
    }
    if(parser.getUnnamedArguments().length() > 0) {
        stderr << args[0] << " " << parser.getEnteredCommand() << ": Invalid command" << endl;
        return;
    } else{
        /*
            This shutdown technique is QEMU specific. Firstly, we initialise the IO port with 0x501 value.
            Then we disable all interrupts of the CPU
            Finally, on writing any value to the IO port, QEMU will shutdown
        */
        IoPort dataPort(0x501);
        Cpu::disableInterrupts();
        dataPort.outb(0x45);
        return;
    }
}

/* Function for the help flags (-h or --help) */
const String Shutdown::getHelpText() {
    return "Shuts down the OS\n\n"
           "Usage: shutdown \n\n"
           "Options:\n"
           "  -h, --help: Show this help-message.";
}