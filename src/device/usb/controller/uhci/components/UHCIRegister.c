#include "UHCIRegister.h"
#include "UHCIMemory.h"
#include "stdint.h"
#include "../../UsbControllerFlags.h"
#include "../../../dev/UsbDevice.h"
#include "../../../interfaces/LoggerInterface.h"

void new_reg(Register *reg, char *name, uint8_t length, void *raw, struct Addr_Region *addr_reg){
    reg->raw_data = raw;
    reg->name = name;
    reg->length = length;
    reg->addr_reg = addr_reg;
}

Register_Type command_reg_type_of(Register *reg){
    return Usb_Command;
}

void command_reg_reload(Register *reg){
    uint16_t d1 = *((uint16_t *)reg->raw_data);
    Command_Register *creg = (Command_Register*)container_of(reg, Command_Register, super);
    creg->max_packet = (d1 & MAXP) >> 7;
    creg->configure = (d1 & CF) >> 6;
    creg->software_debug = (d1 & SWDBG) >> 5;
    creg->global_resume = (d1 & FGR) >> 4;
    creg->global_suspend = (d1 & EGSM) >> 3;
    creg->global_reset = (d1 & GRESET) >> 2;
    creg->host_controller_reset = (d1 & HCRESET) >> 1;
    creg->run = (d1 & RS);
}

void command_reg_set(Register* reg, void* b){
    uint16_t word = *((uint16_t*)b);
    uint16_t raw = *((uint16_t*)reg->raw_data);
    word |= raw;
    reg->write(reg, &word);
    reg->reload(reg);
}

size_t command_reg_write(Register *reg, void* b){
    size_t word_count;
    uint16_t word = *((uint16_t*)b);
    Addr_Region *addr_reg = reg->addr_reg;
    Register_Type type = reg->type_of(reg);
    word_count = addr_reg->write(addr_reg, &word, reg->length, &type);
    *((uint16_t*)(reg->raw_data)) = word;
    reg->reload(reg);
    return word_count;
}

void command_reg_clear(Register* reg, void* b){
    uint16_t word = *((uint16_t*)b);
    uint16_t raw  = *((uint16_t*)reg->raw_data);
    raw &= ~word;
    reg->write(reg, &raw);
    reg->reload(reg);
}

size_t command_reg_read(Register *reg, void* buffer){
    Addr_Region *addr_reg = reg->addr_reg;
    Register_Type type = reg->type_of(reg);
    size_t read_count = addr_reg->read(addr_reg,reg->length, &type, buffer);
    *((uint16_t*)reg->raw_data) = *((uint16_t*)buffer);
    reg->reload(reg);
    return read_count;
}

void new_command_reg(Command_Register *reg, Addr_Region *addr_reg, uint16_t* buffer){
    reg->super.new_register = &new_reg;
    reg->super.reload = &command_reg_reload;
    reg->super.type_of = &command_reg_type_of;
    reg->super.read = &command_reg_read;
    reg->super.write = &command_reg_write;
    reg->super.set = &command_reg_set;
    reg->super.clear = &command_reg_clear;
    reg->super.dump = &dump_usb_cmd;

    mem_set((uint8_t*)buffer, 0x02, 0);

    reg->super.new_register(&reg->super, DEF_NAME(Usb, command), 0x02, buffer, addr_reg);
}

// status register is write clear !

Register_Type status_reg_type_of(Register *reg){
    return Usb_Status;
}

void status_reg_reload(Register *reg){
    uint16_t d1 = *((uint16_t *)reg->raw_data);
    Status_Register *sreg = (Status_Register*)container_of(reg, Status_Register, super);

    sreg->halted = (d1 & HALTED) >> 5;
    sreg->process_error = (d1 & PCS_ERR) >> 4;
    sreg->system_error = (d1 & SYS_ERR) >> 3;
    sreg->resume_detected = (d1 & RES_DTC) >> 2;
    sreg->error_interrupt = (d1 & ERR_INT) >> 1;
    sreg->interrupt = (d1 & INT);
}

void status_reg_set(Register* reg, void* b){
    uint16_t word = *((uint16_t*)b);
    uint16_t raw = *((uint16_t*)reg->raw_data);
    word |= raw;
    reg->write(reg, &word);
    reg->reload(reg);
}

// 0x00FF
size_t status_reg_write(Register *reg, void* b){
    size_t word_count;
    uint16_t word = *((uint16_t*)b);
    Addr_Region *addr_reg = reg->addr_reg;
    Register_Type type = reg->type_of(reg);
    word_count = addr_reg->write(addr_reg, &word, reg->length, &type);

    uint16_t r;
    reg->read(reg, &r);

    *((uint16_t*)(reg->raw_data)) = r & ~word;
    reg->reload(reg);
    
    return word_count;
}

size_t status_reg_read(Register *reg, void* buffer){
    Addr_Region *addr_reg = reg->addr_reg;
    Register_Type type = reg->type_of(reg);
    size_t read_count = addr_reg->read(addr_reg,reg->length, &type, buffer);
    *((uint16_t*)reg->raw_data) = *((uint16_t*)buffer);
    reg->reload(reg);
    return read_count;
}

void new_status_reg(Status_Register *reg, Addr_Region* addr_reg, uint16_t* buffer){
    reg->super.new_register = &new_reg;
    reg->super.reload = &status_reg_reload;
    reg->super.type_of = &status_reg_type_of;
    reg->super.set = &status_reg_set;
    //reg->super.clear = &status_reg_clear;
    reg->super.write = &status_reg_write;
    reg->super.read = &status_reg_read;
    reg->super.dump = &dump_usb_sts;

    mem_set((uint8_t*)buffer, 0x02, 0);

    reg->super.new_register(&reg->super, DEF_NAME(Usb, status), 0x02, buffer, addr_reg);
}

Register_Type interrupt_reg_type_of(Register *reg){
    return Usb_Interrupt;
}

void interrupt_reg_reload(Register *reg){
    uint16_t d1 = *((uint16_t*)reg->raw_data);
    Interrupt_Register *ireg = (Interrupt_Register*)container_of(reg, Interrupt_Register, super);

    ireg->short_packet = (d1 & SHORT_PACK) >> 3;
    ireg->complete = (d1 & COMPLETE) >> 2;
    ireg->resume = (d1 & RESUME) >> 1;
    ireg->timeout_crc = (d1 & TMOUT_CRC);
}

size_t interrupt_reg_write(Register *reg, void* b){
    size_t word_count;
    uint16_t word = *((uint16_t*)b);
    Addr_Region *addr_reg = reg->addr_reg;
    Register_Type type = reg->type_of(reg);
    word_count = addr_reg->write(addr_reg, &word, reg->length, &type);
    *((uint16_t*)(reg->raw_data)) = word;
    reg->reload(reg);

    return word_count;
}

void interrupt_reg_set(Register* reg, void* b){
    uint16_t word = *((uint16_t*)b);
    uint16_t raw = *((uint16_t*)reg->raw_data);
    word |= raw;
    reg->write(reg, &word);
    reg->reload(reg);
}

void interrupt_reg_clear(Register* reg, void* b){
    uint16_t word = *((uint16_t*)b);
    uint16_t raw  = *((uint16_t*)reg->raw_data);
    raw &= ~word;
    reg->write(reg, &raw);
    reg->reload(reg);
}

size_t interrupt_reg_read(Register *reg, void* buffer){
    size_t word_count;
    Addr_Region *addr_reg = reg->addr_reg;
    Register_Type type = reg->type_of(reg);
    word_count = addr_reg->read(addr_reg, reg->length, &type, buffer);
    *((uint16_t*)reg->raw_data) = *((uint16_t*)buffer);
    reg->reload(reg);
    
    return word_count;
}

void new_interrupt_reg(Interrupt_Register *reg, Addr_Region* addr_reg, uint16_t* buffer){
    reg->super.new_register = &new_reg;
    reg->super.reload = &interrupt_reg_reload;
    reg->super.type_of = &interrupt_reg_type_of;
    reg->super.set = &interrupt_reg_set;
    reg->super.clear = &interrupt_reg_clear;
    reg->super.write = &interrupt_reg_write;
    reg->super.read = &interrupt_reg_read;
    reg->super.dump = &dump_usb_intr;

    mem_set((uint8_t*)buffer, 0x02, 0);

    reg->super.new_register(&reg->super, DEF_NAME(Usb, interrupt), 0x02, buffer, addr_reg);
}

void port_reg_reload(Register *reg){
    uint16_t d1 = *((uint16_t*)reg->raw_data);
    Port_Register* preg = (Port_Register*)container_of(reg, Port_Register, super);

    preg->suspend = (d1 & SUSPEND) >> 12;
    preg->reset   = (d1 & RESET) >> 9;
    preg->low_speed = (d1 & LOW_SPEED_ATTACH) >> 8;
    preg->resume_detected = (d1 & RES_DET) >> 6;
    preg->line_status_d_minus = (d1 & LINE_STS_M) >> 5;
    preg->line_status_d_plus  = (d1 & LINE_STS_P) >> 4;
    preg->port_enable_changed = (d1 & ENA_CHANGE) >> 3;
    preg->port_enabled = (d1 & ENA) >> 2;
    preg->connected_changed = (d1 & CON_CHANGE) >> 1;
    preg->connected = (d1 & CONNECT);
}

Register_Type port_reg_type_of_1(Register *reg){
    return Port1_Status;
}

Register_Type port_reg_type_of_2(Register *reg){
    return Port2_Status;
}

size_t port_reg_write(Register *reg, void* b){
    size_t word_count;
    uint16_t word = *((uint16_t*)b);
    Addr_Region *addr_reg = reg->addr_reg;
    Register_Type type = reg->type_of(reg);
    word_count = addr_reg->write(addr_reg, &word, reg->length, &type);
    *((uint16_t*)(reg->raw_data)) = word;
    reg->reload(reg);

    return word_count;
}

void port_reg_clear(Register* reg, void* b){
    uint16_t word = *((uint16_t*)b);
    uint16_t raw  = *((uint16_t*)reg->raw_data);
    raw &= ~word;
    reg->write(reg, &raw);
    reg->reload(reg);
}

void port_reg_set(Register* reg, void* b){
    uint16_t word = *((uint16_t*)b);
    uint16_t raw = *((uint16_t*)reg->raw_data);
    word |= raw;
    reg->write(reg, &word);
    reg->reload(reg);
}

size_t port_reg_read(Register *reg, void* buffer){
    size_t word_count;
    Addr_Region *addr_reg = reg->addr_reg;
    Register_Type type = reg->type_of(reg);
    word_count = addr_reg->read(addr_reg, reg->length, &type, buffer);
    *((uint16_t*)reg->raw_data) = *((uint16_t*)buffer);
    reg->reload(reg);
    
    return word_count;
}

void new_port_reg(Port_Register *reg, int pn, Addr_Region* addr_reg, uint16_t* buffer){
    reg->super.new_register = &new_reg;
    reg->super.reload = &port_reg_reload;
    reg->super.set = &port_reg_set;
    reg->super.clear = &port_reg_clear;
    reg->super.write = &port_reg_write;
    reg->super.read = &port_reg_read;
    reg->super.dump = &dump_usb_portsc;

    mem_set((uint8_t*)buffer, 0x02, 0);

    if (pn == 1)
    {
        reg->super.type_of = &port_reg_type_of_1;
        reg->super.new_register(&reg->super, DEF_NAME(Port, 1), 0x02, buffer, addr_reg);
    }
    else if (pn == 2)
    {
        reg->super.type_of = &port_reg_type_of_2;
        reg->super.new_register(&reg->super, DEF_NAME(Port, 2), 0x02, buffer, addr_reg);
    }
}

void frame_number_reg_reload(Register *reg){
    uint16_t d1 = *((uint16_t*)reg->raw_data);
    Frame_Numb_Register* f_n_reg = (Frame_Numb_Register*)container_of(reg, Frame_Numb_Register, super);

    f_n_reg->frame_num = d1;
}

Register_Type frame_number_reg_type_of(Register *reg){
    return Frame_Number;
}

size_t frame_number_reg_write(Register *reg, void* b){
    size_t word_count;
    uint16_t word = *((uint16_t*)b);
    Addr_Region *addr_reg = reg->addr_reg;
    Register_Type type = reg->type_of(reg);
    word_count = addr_reg->write(addr_reg, &word, reg->length, &type);
    *((uint16_t*)(reg->raw_data)) = word;
    reg->reload(reg);

    return word_count;
}

size_t frame_number_reg_read(Register *reg, void* buffer){
    size_t word_count;
    Addr_Region *addr_reg = reg->addr_reg;
    Register_Type type = reg->type_of(reg);
    word_count = addr_reg->read(addr_reg,reg->length, &type, buffer);
    *((uint16_t*)reg->raw_data) = *((uint16_t*)buffer);
    reg->reload(reg);
    
    return word_count;
}

void new_frame_number_reg(Frame_Numb_Register *reg, Addr_Region* addr_reg, uint16_t* buffer){
    reg->super.new_register = &new_reg;
    reg->super.reload = &frame_number_reg_reload;
    reg->super.type_of = &frame_number_reg_type_of;
    reg->super.write = &frame_number_reg_write;
    reg->super.read = &frame_number_reg_read;
    reg->super.dump = &dump_usb_frnum;

    mem_set((uint8_t*)buffer, 0x02, 0);

    reg->super.new_register(&reg->super, DEF_NAME(Frame, number), 0x02, buffer, addr_reg);
}

void frame_base_reg_reload(Register *reg){
    uint32_t d1 = *((uint32_t*)reg->raw_data);
    Frame_Base_Register* f_b_r = (Frame_Base_Register*)container_of(reg, Frame_Base_Register, super);

    f_b_r->frame_address = d1;
}

Register_Type frame_base_reg_type_of(Register *reg){
    return Frame_List_Base_Address;
}

size_t frame_base_reg_write(Register *reg, void* b){
    
    size_t word_count;
    uint32_t* dword = (uint32_t*)b;
    Addr_Region *addr_reg = reg->addr_reg;
    Register_Type type = reg->type_of(reg);

    word_count = addr_reg->write(addr_reg, dword, reg->length, &type);
    *((uint32_t*)(reg->raw_data)) = *dword;
    reg->reload(reg);

    return word_count;
}

size_t frame_base_reg_read(Register *reg, void* buffer){
    size_t word_count;
    Addr_Region *addr_reg = reg->addr_reg;
    Register_Type type = reg->type_of(reg);
    word_count = addr_reg->read(addr_reg,reg->length, &type, buffer);
    *((uint32_t*)reg->raw_data) = *((uint32_t*)buffer);
    reg->reload(reg);

    return word_count;   
}

void new_frame_base_reg(Frame_Base_Register *reg, Addr_Region* addr_reg, uint32_t* buffer){
    reg->super.new_register = &new_reg;
    reg->super.type_of = &frame_base_reg_type_of;
    reg->super.reload = &frame_base_reg_reload;
    reg->super.write = &frame_base_reg_write;
    reg->super.read = &frame_base_reg_read;
    reg->super.dump = &dump_usb_flbaseadd;

    mem_set((uint8_t*)buffer, 0x04, 0);

    reg->super.new_register(&reg->super, DEF_NAME(Frame, base_address), 0x04, buffer, addr_reg);
}

void sof_reg_reload(Register *reg){
    uint8_t d1 = *((uint8_t*)reg->raw_data);
    SOF_Register* sof = (SOF_Register*)container_of(reg, SOF_Register, super);

    sof->frame_duration = d1;
}

Register_Type sof_reg_type_of(Register *reg){
    return Start_of_Frame;
}

size_t sof_reg_write(Register *reg, void* b){
    size_t word_count;
    uint8_t byte = *((uint8_t*)b);
    Addr_Region *addr_reg = reg->addr_reg;
    Register_Type type = reg->type_of(reg);

    word_count = addr_reg->write(addr_reg, &byte, reg->length, &type);
    *((uint8_t*)(reg->raw_data)) = byte;
    reg->reload(reg);

    return word_count;
}   

size_t sof_reg_read(Register *reg, void* buffer){
    size_t word_count;
    Addr_Region *addr_reg = reg->addr_reg;
    Register_Type type = reg->type_of(reg);
    word_count = addr_reg->read(addr_reg,reg->length, &type, buffer);
    *((uint8_t*)reg->raw_data) = *((uint8_t*)buffer);
    reg->reload(reg);

    return word_count;
}

void new_sof_reg(SOF_Register *reg, Addr_Region* addr_reg, uint8_t* buffer){
    reg->super.new_register = &new_reg;
    reg->super.type_of = &sof_reg_type_of;
    reg->super.reload = &sof_reg_reload;
    reg->super.write = &sof_reg_write;
    reg->super.read = &sof_reg_read;
    reg->super.dump = &dump_usb_sofmod;

    mem_set(buffer, 0x01, 0);

    reg->super.new_register(&reg->super, STRINGIZE(start_of_frame), 0x01, buffer, addr_reg);
}

void dump_usb_cmd(Register* reg, Logger_C* logger){
    if(logger == (void*)0) return;
    Command_Register* c_reg = (Command_Register*)container_of(reg, Command_Register, super);
    
    char* c_reg_msg = "Command Register : \n\nMAXP : %u\nCF : %u\n" \
                      "SWDBG : %u\nFGR : %u \nEGSM : %u\nGRESET : %u\n" \
                      "HCRESET : %u\nRS : %u\n";

    logger->debug_c(logger, c_reg_msg, c_reg->max_packet, c_reg->configure, c_reg->software_debug,
                    c_reg->global_reset, c_reg->global_suspend, c_reg->global_reset,
                    c_reg->host_controller_reset, c_reg->run);               
}

void dump_usb_sts(Register* reg, Logger_C* logger){
    if(logger == (void*)0) return;
    Status_Register* status_reg = (Status_Register*)container_of(reg, Status_Register, super);
    
    char* status_reg_msg = "Status Register : \n\nHCHalted : %u\nHCProcessError : %u\n" \
                           "HostSystemError : %u\nResumeDetect : %u\n" \
                           "UsbError : %u\nUsbInterrupt : %u\n";

    logger->debug_c(logger, status_reg_msg, status_reg->halted, status_reg->process_error,
                    status_reg->system_error, status_reg->resume_detected,
                    status_reg->error_interrupt, status_reg->interrupt);                           
}

void dump_usb_intr(Register* reg, Logger_C* logger){
    if(logger == (void*)0) return;
    Interrupt_Register* intr_reg = (Interrupt_Register*)container_of(reg, Interrupt_Register, super);

    char* intr_reg_msg = "Interrupt Register : \n\nSPI : %u\nIOC : %u\nRI : %u\nTO/CRC : %u";

    logger->debug_c(logger, intr_reg_msg, intr_reg->short_packet, intr_reg->complete, intr_reg->resume, intr_reg->timeout_crc);
}

void dump_usb_portsc(Register* reg, Logger_C* logger){
    if(logger == (void*)0) return;
    Port_Register* port_reg = (Port_Register*)container_of(reg, Port_Register, super);

    char* port_reg_msg = "Port Register : \n\nSuspend : %u\nPortReset : %u\n" \
                         "LowSpeed : %u\nResumeDetected : %u\nLineStatusD- : %u\n" \
                         "LineStatusD+ : %u\nPortEDChange : %u\nPortED : %u\n" \
                         "CSC : %u\nCCS : %u\n";

    logger->debug_c(logger, port_reg_msg, port_reg->suspend, port_reg->reset, port_reg->low_speed,
                    port_reg->resume_detected, port_reg->line_status_d_minus, port_reg->line_status_d_plus,
                    port_reg->port_enable_changed, port_reg->port_enabled, port_reg->connected_changed,
                    port_reg->connected);                     
}

void dump_usb_frnum(Register* reg, Logger_C* logger){
    if(logger == (void*)0) return;
    Frame_Numb_Register* fn_reg = (Frame_Numb_Register*)container_of(reg, Frame_Numb_Register, super);

    char* fn_reg_msg = "Frame Number Register : \n\nFrame Number : %u\n";

    logger->debug_c(logger, fn_reg_msg, fn_reg->frame_num);
}

void dump_usb_flbaseadd(Register* reg, Logger_C* logger){
    if(logger == (void*)0) return;
    Frame_Base_Register* fb_reg = (Frame_Base_Register*)container_of(reg, Frame_Base_Register, super);

    char* fb_reg_msg = "Frame Base Register : \n\nFrame Address : %u\n";

    logger->debug_c(logger, fb_reg_msg, fb_reg->frame_address);
}

void dump_usb_sofmod(Register* reg, Logger_C* logger){
    if(logger == (void*)0) return;
    SOF_Register* sof_reg = (SOF_Register*)container_of(reg, SOF_Register, super);

    char* sof_reg_msg = "SOF Register : \n\nFrame Duration : %u\n";

    logger->debug_c(logger, sof_reg_msg, sof_reg->frame_duration);
}