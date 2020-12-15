/*
 * Copyright (C) 2020 Namit Shah, Martand Javia & Harvish Jariwala
 * Ahmedabad University
 *
 * This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public
 * License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any
 * later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied
 * warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>
 */

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