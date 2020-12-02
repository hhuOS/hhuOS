#include "lib/libc/printf.h"
#include "Ps.h"
#include "kernel/thread/Scheduler.h"
#include "kernel/thread/Thread.h"

Ps::Ps(Shell &shell) : Command(shell) {

};

void Ps::execute(Util::Array<String> &args) {
    Util::ArgumentParser parser(getHelpText(), 1);
    if(!parser.parse(args)) {
        stderr << args[0] << ": " << parser.getErrorString() << endl;
        return;
    }
    printf("Total Processes : %d\n",Kernel::Scheduler::getInstance().getThreadCount());
    printf("-------------------------------\n");
    printf("PID   Process Name\n");
    printf("-------------------------------\n");
    for (const auto &queue : Kernel::Scheduler::getInstance().readyQueues) {
        for(auto x : queue){
            printf("%02d    %s\n",x->getId(),(char*)x->getName());
        }
    }       
}

const String Ps::getHelpText() {
    return "Shows all the background processes/threads running.\n\n"
           "Usage: ps [OPTION]...\n\n"
           "Options:\n"
           "  -h, --help: Shows the help-message.";
}