// options : default just printing all devices
// -l print with level
// -e print with extended information
// -d print downstream
// -c number
// -r number

#include <cstdint>

#include "lib/util/base/ArgumentParser.h"
#include "lib/util/io/file/File.h"
#include "lib/util/io/stream/FileInputStream.h"
#include "lib/util/base/System.h"

void print_device_information(Util::Io::File& f, int16_t root_number,
                              uint8_t extended,  int16_t level){
    bool error;
    if(level >= 0){
        int l = Util::String::parseInt(Util::Io::FileInputStream(f.getCanonicalPath() + "/level").readLine(error));
        if(l != level) return;
    }
    if(root_number >= 0){
        int r = Util::String::parseInt(Util::Io::FileInputStream(f.getCanonicalPath() + "/root").readLine(error));
        if(r != root_number) return;
    }
    
    Util::String root = Util::Io::FileInputStream(f.getCanonicalPath() + "/root").readLine(error);
    Util::String dev_num = Util::Io::FileInputStream(f.getCanonicalPath() + "/dev_num").readLine(error);
    Util::String idVendor = Util::Io::FileInputStream(f.getCanonicalPath() + "/idVendor").readLine(error);
    Util::String idProduct = Util::Io::FileInputStream(f.getCanonicalPath() + "/idProduct").readLine(error);
    Util::String manufacturer = Util::Io::FileInputStream(f.getCanonicalPath() + "/manufacturer").readLine(error);
    Util::String product = Util::Io::FileInputStream(f.getCanonicalPath() + "/product").readLine(error);

    Util::System::out << "Root " + root + " : " << Util::Io::PrintStream::flush;
    Util::System::out << "Device " + dev_num + " : " << Util::Io::PrintStream::flush;
    Util::System::out << "ID " + idVendor + "-" << Util::Io::PrintStream::flush;
    Util::System::out << idProduct + " : " << Util::Io::PrintStream::flush;
    Util::System::out << manufacturer + " , "<< Util::Io::PrintStream::flush;
    Util::System::out << product << Util::Io::PrintStream::flush;
    if(extended){
        Util::String le = Util::Io::FileInputStream(f.getCanonicalPath() + "/level").readLine(error);
        int speed = Util::String::parseInt(Util::Io::FileInputStream(f.getCanonicalPath() + "/speed").readLine(error));
        Util::String speed_string;
        if(!speed){
            speed_string = Util::String("Low Speed");
        }
        else speed_string = Util::String("Full Speed");
        Util::String port_num = Util::Io::FileInputStream(f.getCanonicalPath() + "/port").readLine(error);
        Util::String removable = Util::Io::FileInputStream(f.getCanonicalPath() + "/removable").readLine(error);
        Util::System::out << "Level : " + le + " , " << Util::Io::PrintStream::flush;
        Util::System::out << "Speed : " + speed_string + " , " << Util::Io::PrintStream::flush;
        Util::System::out << "Port : " + port_num + " , " << Util::Io::PrintStream::flush;
        Util::System::out << "Removable : " + removable << Util::Io::PrintStream::flush;
    }

    Util::System::out << Util::Io::PrintStream::endl << Util::Io::PrintStream::flush; 
}

int32_t main(int32_t argc, char* argv[]){
    Util::ArgumentParser arg_parser = Util::ArgumentParser();
    arg_parser.addArgument("level", false,"l");
    arg_parser.addSwitch("extended", "e");
    arg_parser.addSwitch("downstream", "d");
    arg_parser.addArgument("controller", false, "c");
    arg_parser.addArgument("root", false, "r");

    arg_parser.setHelpText("Show Usb.\n"
                               "Usage: lsusb [options]\n"
                               "Options:\n"
                               "\t-l, --level [number]: Display devices on level [number]\n"
                               "\t-e, --extended : Extended information\n"
                               "\t-d, --downstream : Display correct order\n"
                               "\t-c, --controller [number] : Display devices on controller [number]\n"
                               "\t-r, --root [number] : Display devices on root port [number]\n"
                               "\t-h, --help: Show this help message");

    if(!arg_parser.parse(argc, argv)){
        Util::System::error << arg_parser.getErrorString() 
                << Util::Io::PrintStream::endl << Util::Io::PrintStream::flush;
        return -1;
    }

    uint8_t level_active = 0;
    uint8_t level_number = 0;
    uint8_t extended_information_active = 0;
    uint8_t down_stream_active = 0;
    uint8_t controller_active = 0;
    uint8_t controller_number = 0;
    uint8_t root_active = 0;
    uint8_t root_number = 0;

    if(arg_parser.hasArgument("l")){
        level_active = 1;
        level_number = Util::String::parseInt(arg_parser.getArgument("l"));
    }
    if(arg_parser.hasArgument("level")){
        level_active = 1;
        level_number = Util::String::parseInt(arg_parser.getArgument("level"));
    }
    if(arg_parser.checkSwitch("e") || arg_parser.checkSwitch("extended")){
        extended_information_active = 1;
    }
    if(arg_parser.checkSwitch("d") || arg_parser.checkSwitch("downstream")){
        down_stream_active = 1;
    }
    if(arg_parser.hasArgument("c")){
        controller_active = 1;
        controller_number = Util::String::parseInt(arg_parser.getArgument("c"));
    }
    if(arg_parser.hasArgument("controller")){
        controller_active = 1;
        controller_number = Util::String::parseInt(arg_parser.getArgument("controller"));
    }
    if(arg_parser.hasArgument("r")){
        root_active = 1;
        root_number = Util::String::parseInt(arg_parser.getArgument("r"));
    }
    if(arg_parser.hasArgument("root")){
        root_active = 1;
        root_number = Util::String::parseInt(arg_parser.getArgument("root"));
    }

    Util::Io::File f = Util::Io::File("/system/usbfs");
    if(f.isDirectory()){
        Util::Array<Util::String> children = f.getChildren();
        for(uint32_t i = 0; i < children.length(); i++){
            if(controller_active && (i != controller_number)) continue;

            Util::String dev_string = Util::String::format("/system/usbfs/controller%u/devices", i);
            Util::Io::File dev_f = Util::Io::File(dev_string);
            if(down_stream_active){
                // dev0, dev1 are always both hub devices conntected to the root ports
                for(uint8_t dev_i = 0; dev_i < 2; dev_i++){
                    Util::String down_stream_port = Util::String::format("/system/usbfs/controller%u/devices/dev%u/usb_port0/dev0", i, dev_i);
                    uint8_t j = 0;
                    while(Util::Io::File::open(down_stream_port) != -1){
                        Util::Io::File downstream_dev = Util::Io::File(
                            Util::String::format("/system/usbfs/controller%u/devices/dev%u/usb_port%u/dev%u", i, dev_i, j, j));
                        j++;
                        print_device_information(downstream_dev, (root_active == 1) ? root_number : -1, extended_information_active,
                                    (level_active == 1) ? level_number : -1);
                        down_stream_port = Util::String::format("/system/usbfs/controller%u/devices/dev%u/usb_port%u/dev%u", i, dev_i, j, j);
                    }
                }
            }
            else{
                Util::String dev = Util::String::format("/system/usbfs/controller%u/devices/dev0", i);
                uint8_t j = 0;
                while(Util::Io::File::open(dev) != -1){
                    Util::Io::File fi = Util::Io::File(Util::String::format("/system/usbfs/controller%u/devices/dev%u", i, j++));
                    print_device_information(fi, root_active == 1 ? root_number : -1, extended_information_active,
                            level_active == 1 ? level_number : -1);
                    dev = Util::String::format("/system/usbfs/controller%u/devices/dev%u", i, j);
                }
            }
        }
    }

    return 0;
}