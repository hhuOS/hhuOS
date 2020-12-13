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

#include "lib/libc/printf.h"
#include "Ps.h"
#include "kernel/thread/Scheduler.h"
#include "kernel/thread/Thread.h"

Ps::Ps(Shell &shell) : Command(shell) {

};


/*Driver code of ps command*/
void Ps::execute(Util::Array<String> &args) {
    /*Parses the help text for --help flag*/
    Util::ArgumentParser parser(getHelpText(), 1);
    if(!parser.parse(args)) {
        stderr << args[0] << ": " << parser.getErrorString() << endl;
        return;
    }

    /*Creates an instance of scheduler to access all the processes and counts the number of active processes*/
    printf("Total Processes : %d\n",Kernel::Scheduler::getInstance().getThreadCount());
    printf("-------------------------------\n");
    printf("PID   Process Name\n");
    printf("-------------------------------\n");
    Kernel::Scheduler::getInstance().getReadyQueue();     
}
/*Help message for the command*/
const String Ps::getHelpText() {
    return "Shows all the background processes/threads running.\n\n"
           "Usage: ps [OPTION]...\n\n"
           "Options:\n"
           "  -h, --help: Shows the help-message.";
}