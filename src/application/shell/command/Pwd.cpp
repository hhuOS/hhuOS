#include "lib/file/FileStatus.h"
#include "Pwd.h"

String getEnteredCommand(Util::Array<String> unamedArguments){
    String command = "";
    for(auto it:unamedArguments) command += it + " ";
    return command;
}

Pwd::Pwd(Shell &shell) : Command(shell) {

};

void Pwd::execute(Util::Array<String> &args) {
    Util::ArgumentParser parser(getHelpText(), 1);

    if(!parser.parse(args)) {
        stderr << args[0] << ": " << parser.getErrorString() << endl;
        return;
    }
    if(parser.getUnnamedArguments().length() > 0) {
        stderr << args[0] << " " << getEnteredCommand(parser.getUnnamedArguments()) << ": Invalid command" << endl;
        return;
    } else{
        String path = shell.getCurrentWorkingDirectory().getAbsolutePath();
        path = path.length()==0 ? "/" : path;
        stdout << path << endl;
    }
}

const String Pwd::getHelpText() {
    return "Displays the whole path of the current working directory.\n\n"
           "Usage: pwd \n\n"
           "Options:\n"
           "  -h, --help: Show this help-message.";
}