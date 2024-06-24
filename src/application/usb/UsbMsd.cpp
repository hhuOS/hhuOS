#include <cstdint>
#include "lib/util/usb/io_control/MassStorageDeviceControl.h"
#include "lib/util/io/file/File.h"
#include "lib/util/io/stream/FileOutputStream.h"
#include "lib/util/io/stream/FileInputStream.h"
#include "lib/util/base/System.h"
#include "lib/util/base/ArgumentParser.h"
#include "lib/util/base/String.h"
#include "lib/util/io/stream/FileOutputStream.h"
#include "lib/util/io/stream/FileInputStream.h"
#include "lib/interface.h"
#include "lib/util/base/Address.h"
#include "lib/util/async/Thread.h"

const constexpr uint8_t MAX_EXECUTION = 5;
const uint16_t MAGIC_NUM = 1000;
const uint8_t  U_TAG     = 50;

bool parse_required_arguments(uint8_t* size, Util::String& lba_start,
    Util::String& blocks, Util::String& volume_string, uint32_t* lba, 
    uint32_t* block, uint32_t* volume){
    auto lba_split   = lba_start.split(",", MAX_EXECUTION);
    auto block_split = blocks.split(",", MAX_EXECUTION);
    if(lba_split.length() != block_split.length()) return false;
    for(uint32_t i = 0; i < lba_split.length(); i++){
        lba[i]   = Util::String::parseInt(lba_split[i]);
        block[i] = Util::String::parseInt(block_split[i]);
    }
    *size   = block_split.length();
    *volume = Util::String::parseInt(volume_string);

    return true;
}

bool argument_values_ok(uint32_t block_num,
    uint32_t volume, uint32_t lba_start, uint32_t blocks, 
    uint32_t u_volume){
    if(u_volume >= volume) return false;
    if(lba_start + blocks > block_num) return false;

    return true;
}

bool contains_file(Util::Io::File& dir, Util::String& file_string){
    auto childs = dir.getChildren();
    for(Util::String s : childs){
        if(s == file_string) return true;
    }
    return false;
}

uint64_t build_msd_data(uint32_t blocks, uint8_t volume,
    uint16_t magic, uint8_t u_tag){
    uint64_t msd_data = ((uint64_t)blocks << 32);
    msd_data |= volume;
    msd_data |= ((uint64_t)magic << 8);
    msd_data |= ((uint64_t)u_tag << 24);

    return msd_data;
}

Util::String build_string(uint8_t* target_buffer, uint32_t len){
    return Util::String(target_buffer, len);
}

uint32_t write_data_to_file(Util::Array<Util::String>& unnamed_arguments, 
    Util::Io::File& tmp_file){
    Util::Io::FileOutputStream out_stream = Util::Io::FileOutputStream(tmp_file);
    Util::String start_str = "";
    Util::String res_str   = start_str.join(" ", unnamed_arguments);
    out_stream.write((uint8_t*)res_str, 0, res_str.length());

    return res_str.length();
}

void write_routine(Util::Io::File& msd_file, Util::Io::File& data_file,
    uint32_t* lba_arr, uint32_t* block_arr, uint8_t size, uint32_t volume,
    uint32_t block_len, uint32_t data_size){
    uint64_t msd_data;
    uint64_t io_size;
    for(int i=0; i<size; i++){
        uint32_t lba = lba_arr[i], block = block_arr[i];
        uint32_t left = block_len - (data_size % (block_len+1));
        uint32_t block_num = data_size / (block*block_len);
        uint8_t* read_buff   = new uint8_t[block_len*(block-block_num)];
        uint8_t* source_buff = new uint8_t[block_len*block_len];

        Util::Io::FileInputStream input_stream = Util::Io::FileInputStream(data_file);
        input_stream.read(source_buff, 0, data_size);
        msd_data = build_msd_data(block, volume, MAGIC_NUM, U_TAG);

        if(read_buff != (void*)0){
            io_size = readFile(msd_file.open(msd_file.getCanonicalPath()), read_buff,
            lba, msd_data);
            Util::Address<uint32_t> source_buff_addr = Util::Address<uint32_t>(
                source_buff+(block_len*block_num)+(block_len-left));
            Util::Address<uint32_t> read_buff_addr   = Util::Address<uint32_t>(read_buff+(block_len-left));
            source_buff_addr.copyRange(read_buff_addr, left + (block_len * (block-block_num-1)));
        }
        io_size = writeFile(msd_file.open(msd_file.getCanonicalPath()), source_buff,
            lba, msd_data);
        Util::System::out << Util::String::format("Wrote %u bytes ...", io_size) 
            << Util::Io::PrintStream::endl << Util::Io::PrintStream::flush;
    }
}

void read_routine(Util::Io::File& msd_file, Util::Io::File& target_file,
    uint32_t* lba_arr, uint32_t* block_arr, uint8_t size, uint32_t volume,
    uint32_t block_len){
    Util::Io::FileInputStream input_stream = Util::Io::FileInputStream(msd_file);
    uint64_t msd_data;
    for(int i=0; i<size; i++){
        uint32_t lba = lba_arr[i], block = block_arr[i];
        uint8_t* target_buff = new uint8_t[block_len*block];
        msd_data = build_msd_data(block, volume, MAGIC_NUM, U_TAG);
        uint32_t size = readFile(msd_file.open(msd_file.getCanonicalPath()), target_buff,
            lba, msd_data);
        Util::System::out << Util::String::format("Read %u bytes ...", size) 
            << Util::Io::PrintStream::endl << Util::Io::PrintStream::flush;
        if(target_file.getName() == ""){
            Util::String data = build_string(target_buff, size);
            Util::System::out << data << Util::Io::PrintStream::endl << Util::Io::PrintStream::flush; 
        }
        else{
            Util::Io::FileOutputStream out_stream = Util::Io::FileOutputStream(target_file);
            out_stream.write(target_buff, 0, size);
        }
    }
}

uint32_t get_file_data_size(Util::Io::File& data_file){
    Util::Io::FileInputStream in_stream = Util::Io::FileInputStream(data_file);
    uint32_t data_size = 0;
    while(in_stream.read() != -1){
        data_size++;
    }
    return data_size;
}

int32_t main(int32_t argc, char* argv[]){
    Util::Io::File msd_file = Util::Io::File("/device/msd0");
    if(!msd_file.exists()){
        Util::System::error << "Device not present !" << Util::Io::PrintStream::endl << Util::Io::PrintStream::flush;
        return -1;
    }
    Util::ArgumentParser arg_parser = Util::ArgumentParser();
    arg_parser.addSwitch("write");
    arg_parser.addSwitch("read");
    arg_parser.addArgument("dir");
    arg_parser.addArgument("file");
    arg_parser.addArgument("lba-start", true); 
    arg_parser.addArgument("blocks", true);  
    arg_parser.addArgument("volume", true);

    arg_parser.setHelpText("Usage : msd [--read | --write] [--lba-start x] [--volume x] [--blocks x] [options] [args]\n"
                           "Options:\n"
                           "\t--file [arg]: Specify file to read or write from/to\n"
                           "\t--dir  [arg]: Specify a directory, to search the file specified via --file\n");

    uint32_t volume_num;
    msd_file.control(GET_VOLUMES, {(uint32_t)&volume_num});
    Util::System::out << Util::String::format("Volumes found : %u", volume_num) << Util::Io::PrintStream::endl;
    uint32_t drive_size[volume_num], block_size[volume_num], 
        blocks_num[volume_num], capacities[volume_num];
    for(uint32_t i = 0; i < volume_num; i++){
        msd_file.control(GET_SIZE, {(uint32_t)&drive_size[i], i});
        msd_file.control(GET_BLOCK_LEN, {(uint32_t)&block_size[i], i});
        msd_file.control(GET_BLOCK_NUM, {(uint32_t)&blocks_num[i], i});
        msd_file.control(GET_CAPACITIES_FOUND, {(uint32_t)&capacities[i], i});
        Util::System::out << Util::String::format("Volume %u size             : %u", i, drive_size[i]) << Util::Io::PrintStream::endl;
        Util::System::out << Util::String::format("Volume %u block size       : %u", i, block_size[i]) << Util::Io::PrintStream::endl;
        Util::System::out << Util::String::format("Volume %u total blocks     : %u", i, blocks_num[i]) << Util::Io::PrintStream::endl;
        Util::System::out << Util::String::format("Volume %u total capacities : %u", i, capacities[i]) << Util::Io::PrintStream::endl 
            << Util::Io::PrintStream::flush;
    }

    // multiple reads/writes allowed -> lba-start size has to match blocks size

    if(!arg_parser.parse(argc, argv)){
        Util::System::error << arg_parser.getErrorString() 
                << Util::Io::PrintStream::endl << Util::Io::PrintStream::flush;
        return -1;
    }
    uint8_t size;
    uint32_t lba_array[MAX_EXECUTION];
    uint32_t block_array[MAX_EXECUTION];
    uint32_t vol;
    Util::String lbas   = arg_parser.getArgument("lba-start");
    Util::String blocks = arg_parser.getArgument("blocks");
    Util::String volume = arg_parser.getArgument("volume");
    if(!parse_required_arguments(&size, lbas, blocks, volume, 
            lba_array,block_array, &vol)){
        Util::System::error << "Error encountered during parsing ..." << Util::Io::PrintStream::endl << 
            Util::Io::PrintStream::flush;
        return -1;
    }
    for(int i=0; i<size; i++){
        if(!argument_values_ok(blocks_num[0], volume_num, lba_array[i],
            block_array[i], vol)) {
            Util::System::error << "Error encountered while checking for correctness of arguments ..." << Util::Io::PrintStream::endl << 
                Util::Io::PrintStream::flush;
            return -1;
        }
    }

    uint8_t directory_option = 0, file_option = 0;
    uint8_t read = 0, write = 0;
    Util::Io::File directory = Util::Io::File("");
    Util::Io::File file      = Util::Io::File("");
    if(!arg_parser.checkSwitch("write") && !arg_parser.checkSwitch("read")){
        Util::System::error << "Need to specify i/o operation ..." << Util::Io::PrintStream::endl
            << Util::Io::PrintStream::flush;
        return -1;
    }
    if(arg_parser.checkSwitch("write") && arg_parser.checkSwitch("read")){
        Util::System::error << "Only single i/o operation permitted per call ..." 
            << Util::Io::PrintStream::endl << Util::Io::PrintStream::flush;
        return -1;
    }
    if(arg_parser.checkSwitch("write"))
        write = 1;
    if(arg_parser.checkSwitch("read"))
        read = 1;
    if(arg_parser.hasArgument("dir")){
        Util::String directory_string = arg_parser.getArgument("dir");
        directory = Util::Io::File(directory_string);
        if(!directory.exists()){
            if(write){
                Util::System::error << "Invalid directory ..." 
                << Util::Io::PrintStream::endl << Util::Io::PrintStream::flush;
                return -1;
            }
            Util::System::out << "Directory created ..." 
                << Util::Io::PrintStream::endl << Util::Io::PrintStream::flush;
            directory.create(Util::Io::File::REGULAR);
        }
        directory_option = 1;
    }
    uint32_t data_written = 0;
    if(arg_parser.hasArgument("file")){
        Util::String file_string = arg_parser.getArgument("file");
        if(directory_option){
            if(!contains_file(directory, file_string) && write){
               Util::System::error << "Invalid file ..." 
                << Util::Io::PrintStream::endl << Util::Io::PrintStream::flush;
               return -1;
            }
            file = Util::Io::File(directory.getCanonicalPath().join("/", 
                {directory.getName(), file_string}));
            if(!file.exists() && read){
                Util::System::out << "File created ..." 
                << Util::Io::PrintStream::endl << Util::Io::PrintStream::flush;
                file.create(Util::Io::File::REGULAR);
            }
        }
        else{
            file = Util::Io::File(file_string);
            if(!file.exists()){
                if(write){
                    Util::System::error << "Invalid file ..." 
                    << Util::Io::PrintStream::endl << Util::Io::PrintStream::flush;
                    return -1;
                }
                file.create(Util::Io::File::REGULAR);
            }
        }
        data_written = get_file_data_size(file);
        file_option = 1;
    }
    if(!directory_option && !file_option){
        auto text = arg_parser.getUnnamedArguments();
        if(!text.length() && write) {
            Util::System::error << "No data to be transmitted ..." 
                << Util::Io::PrintStream::endl << Util::Io::PrintStream::flush;
            return -1;
        }
        if(write){
            Util::System::out << "Created tmp file ..." 
                << Util::Io::PrintStream::endl << Util::Io::PrintStream::flush;
            file = Util::Io::File("msd_tmp_data");
            file.create(Util::Io::File::REGULAR);
            Util::System::out << "Writing data to tmp file ..." 
                << Util::Io::PrintStream::endl << Util::Io::PrintStream::flush;
            data_written = write_data_to_file(text, file);
        }   
    }

    if(write) {
        Util::System::out << "Starting to write ..." 
            << Util::Io::PrintStream::endl << Util::Io::PrintStream::flush;
        write_routine(msd_file, file, lba_array, block_array, size, vol, block_size[0], data_written);
    }
    else {
        Util::System::out << "Starting to read ..." 
            << Util::Io::PrintStream::endl << Util::Io::PrintStream::flush;
        read_routine(msd_file, file, lba_array, block_array, size, vol, block_size[0]);
    }
    
    if(file.getName() == "msd_tmp_data"){
        file.remove();
    }
    return 1;
}