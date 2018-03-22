#include "Clear.h"

Clear::Clear(Shell &shell) : Command(shell) {

};

void Clear::execute(Util::Array<String> &args, OutputStream &outputStream) {
    for(uint32_t i = 1; i < args.length(); i++) {
        if(args[i] == "-h" || args[i] == "--help") {
            outputStream << "Clears the screen." << endl << endl;
            outputStream << "Usage: " << args[0] << " [OPTION]..." << endl << endl;
            outputStream << "Options:" << endl;
            outputStream << "  -h, --help: Show this help-message." << endl;
            return;
        } else {
            stderr << args[0] << ": Invalid option '" << args[i] << "'!" << endl;
            return;
        }
    }

    auto *text = Kernel::getService<GraphicsService>()->getTextDriver();
    text->clear();
}