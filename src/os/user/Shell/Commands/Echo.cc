#include "Echo.h"

Echo::Echo(Shell &shell) : Command(shell) {

};

void Echo::execute(Util::Array<String> &args, OutputStream &outputStream) {
    Util::ArrayList<String> strings;

    for(uint32_t i = 1; i < args.length(); i++) {
        if(!args[i].beginsWith("-") || args[i] == "-") {
            strings.add(args[i]);
        } else if(args[i] == "-h" || args[i] == "--help") {
            outputStream << "Writes it's arguments to the standard output stream." << endl << endl;
            outputStream << "Usage: " << args[0] << " [OPTION]... [ARGUMENT]..." << endl << endl;
            outputStream << "Options:" << endl;
            outputStream << "  -h, --help: Show this help-message." << endl;
            return;
        } else {
            stderr << args[0] << ": Invalid option '" << args[i] << "'!" << endl;
            return;
        }
    }

    for(const String &string : strings) {
        outputStream << string << " ";
    }

    outputStream << endl;
}