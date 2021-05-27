//
// Created by hannes on 20.04.21.
//

#include <lib/libc/printf.h>
#include "Ip.h"

Ip::Ip(Shell &shell) : Command(shell) {

}

void Ip::execute(Util::Array<String> &args) {
    Util::ArgumentParser parser(getHelpText(), 1);

    parser.addSwitch("link", "l");

    if (!parser.parse(args)) {
        stderr << args[0] << ": " << parser.getErrorString() << endl;
        return;
    }

    auto *networkService = Kernel::System::getService<Kernel::NetworkService>();

    if (networkService->getDeviceCount() == 0) {
        stderr << args[0] << ": No network devices available!" << endl;
        return;
    }

    if (parser.checkSwitch("link")) {
        link(networkService);
    }
}

void Ip::link(Kernel::NetworkService *networkService) {
    if (networkService == nullptr) {
        printf("No valid network service given! Exit");
        return;
    }

    stdout << "Print available network links" << endl;

    auto * linkAttributes=new Util::ArrayList<String>();
    networkService->collectLinkAttributes(linkAttributes);

    for (uint32_t i=0; i < linkAttributes->size(); i++) {
        stdout << "Device " << i << ": "  << linkAttributes->get(i) << endl;
    }
}

const String Ip::getHelpText() {
    return "Utility for reading and setting attributes for IP network interfaces\n\n"
           "Usage: ip [OPTIONS]\n"
           "Options:\n"
           "   -l, --link: List all available network links and their attributes\n"
           "   -h, --help: Show this help-message";
}
