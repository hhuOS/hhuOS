//
// Created by hannes on 20.04.21.
//

#include "Ip.h"

Ip::Ip(Shell &shell) : Command(shell) {

}

void Ip::execute(Util::Array<String> &args) {
    Util::ArgumentParser parser(getHelpText(), 1);

    parser.addParameter("link", "l", false);

    if (!parser.parse(args)) {
        stderr << args[0] << ": " << parser.getErrorString() << endl;
        return;
    }

    auto *networkService = Kernel::System::getService<Kernel::NetworkService>();

    if(networkService->getDeviceCount() == 0) {
        stderr << args[0] << ": No network devices available!" << endl;
        return;
    }

    if(!parser.getNamedArgument("link").isEmpty()) {
        link(networkService);
    }
}

void Ip::link(Kernel::NetworkService* networkService) {
    stdout << "Print available network links\n" << endl;

    uint8_t macAddress=0;
    uint32_t count = networkService->getDeviceCount();

    for(uint8_t i=0;(uint32_t)i<count;i++){
        networkService->getDriver(i).getMacAddress(&macAddress);

        stdout
        << "Link No. " << i << ": MAC='"
        << macAddress
        << "'\n"
        << endl;
    }
}

const String Ip::getHelpText() {
    return "Utility for reading and setting attributes for IP network interfaces\n\n"
            "Usage: ip [COMMAND] [OPTIONS]\n"
            "Commands:\n"
            "   link: List all available network links and their attributes\n"
            "Options:\n"
            "   -h, --help: Show this help-message\n";
}