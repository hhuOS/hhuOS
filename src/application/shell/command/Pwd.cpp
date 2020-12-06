#include "lib/file/FileStatus.h"
#include "Pwd.h"

Pwd::Pwd(Shell &shell) : Command(shell) {

};

/*Driver function of pwd command*/
void Pwd::execute(Util::Array<String> &args) {

    /*Parse the help message for --help flag*/
    Util::ArgumentParser parser(getHelpText(), 1);
    /*Error if could not parse the input command*/
    if(!parser.parse(args)) {
        stderr << args[0] << ": " << parser.getErrorString() << endl;
        return;
    }
    /*Error if arguments except for help flag provided by the user*/
    if(parser.getUnnamedArguments().length() > 0) {
        stderr << args[0] << " " << parser.getEnteredCommand() << ": Invalid command" << endl;
        return;
    } else{  /*Gets the pointer to the current working directory from shell and fetch path in required format*/
        String path = shell.getCurrentWorkingDirectory().getAbsolutePath();
        path = path.length()==0 ? "/" : path;
        stdout << path << endl;
    }
}
/*Help message for the command*/
const String Pwd::getHelpText() {
    return "Displays the whole path of the current working directory.\n\n"
           "Usage: pwd \n\n"
           "Options:\n"
           "  -h, --help: Show this help-message.";
}