#ifndef UHCI_REGISTER_INCLUDE
#define UHCI_REGISTER_INCLUDE

#include "../../../utility/Utils.h"
#include "stdint.h"
#include "../../components/ControllerMemory.h"
#include "../../../interfaces/LoggerInterface.h"
#include "../../components/ControllerRegister.h"

#define __UHCI_MERGE__(A) __STRINGIZE__(A)
#define __FRAME__(n) __UHCI_MERGE__(__CONCAT__(FRAME, _ ## n))
#define __PORT__(n) __UHCI_MERGE__(__CONCAT__(PORT, _ ## n))
#define __USB__(n) __UHCI_MERGE__(__CONCAT__(USB, _ ## n))
#define __DEF_NAME__(v1,v2) __ ## v1 ## __(v2)

enum Command{
    MAXP    = 0x0080,
    CF      = 0x0040,
    SWDBG   = 0x0020,
    FGR     = 0X0010,
    EGSM    = 0x0008,
    GRESET  = 0x0004,
    HCRESET = 0x0002,
    RS      = 0x0001
};

// write clear
enum Status{
    HALTED     = 0x0020,
    PCS_ERR    = 0x0010,
    SYS_ERR    = 0x0008,
    RES_DTC    = 0x0004,
    ERR_INT    = 0x0002,
    INT        = 0x0001
};

enum Interrupt{
    SHORT_PACK = 0x0008,
    COMPLETE   = 0x0004,
    RESUME     = 0x0002,
    TMOUT_CRC  = 0x0001
};

enum Port_I{
    SUSPEND    = 0x1000,
    RESET      = 0x0200,
    LOW_SPEED_ATTACH = 0x0100,
    RES_DET    = 0x0040,
    LINE_STS_M = 0x0020, // D-
    LINE_STS_P = 0x0010, // D+
    ENA_CHANGE = 0x0008, // write clear
    ENA        = 0x0004,
    CON_CHANGE = 0x0002, // write clear
    CONNECT    = 0x0001
};

#define __DECLARE_REGISTER__(type) \
    struct type ## _Register

#define __DECLARE_NEW_REG__(type, buffer_type, name) \
    void (*name)(__DECLARE_REGISTER__(type)* reg, struct Addr_Region* addr_region, \
                buffer_type* buffer)

__DECLARE_REGISTER__(Command){
    Register super;
    __DECLARE_NEW_REG__(Command, uint16_t, new_command_register);
    uint8_t max_packet;
    uint8_t configure;
    uint8_t software_debug;
    uint8_t global_resume;
    uint8_t global_suspend;
    uint8_t global_reset;
    uint8_t host_controller_reset;
    uint8_t run;
};

__DECLARE_REGISTER__(Status){
    Register super;
    __DECLARE_NEW_REG__(Status, uint16_t, new_status_register);
    uint8_t halted;
    uint8_t process_error;
    uint8_t system_error;
    uint8_t resume_detected;
    uint8_t error_interrupt;
    uint8_t interrupt;
};

__DECLARE_REGISTER__(Interrupt){
    Register super;
    __DECLARE_NEW_REG__(Interrupt, uint16_t, new_interrupt_register);
    uint8_t short_packet;
    uint8_t complete;
    uint8_t resume;
    uint8_t timeout_crc;
};

__DECLARE_REGISTER__(Port){
    Register super;
    void (*new_port_register)(__DECLARE_REGISTER__(Port)* reg, int pn, 
        struct Addr_Region* addr_region, uint16_t* buffer);
    uint8_t suspend;
    uint8_t reset;
    uint8_t low_speed;
    uint8_t resume_detected;
    uint8_t line_status_d_minus;
    uint8_t line_status_d_plus;
    uint8_t port_enable_changed;
    uint8_t port_enabled;
    uint8_t connected_changed;
    uint8_t connected;
};

__DECLARE_REGISTER__(Frame_Numb){
    Register super;
    __DECLARE_NEW_REG__(Frame_Numb, uint16_t, new_frame_number_register);
    uint16_t frame_num;
};

__DECLARE_REGISTER__(Frame_Base){
    Register super;
    __DECLARE_NEW_REG__(Frame_Base, uint32_t, new_frame_base_register);
    uint32_t frame_address;
};

__DECLARE_REGISTER__(SOF){
    Register super;
    __DECLARE_NEW_REG__(SOF, uint8_t, new_sof_register);
    uint8_t frame_duration;
};

#define __REGISTER_NAME__(name) \
    name ## _Register

typedef __DECLARE_REGISTER__(Command) __REGISTER_NAME__(Command);
typedef __DECLARE_REGISTER__(Status) __REGISTER_NAME__(Status);
typedef __DECLARE_REGISTER__(Interrupt) __REGISTER_NAME__(Interrupt);
typedef __DECLARE_REGISTER__(Port) __REGISTER_NAME__(Port);
typedef __DECLARE_REGISTER__(Frame_Numb) __REGISTER_NAME__(Frame_Numb);
typedef __DECLARE_REGISTER__(Frame_Base) __REGISTER_NAME__(Frame_Base);
typedef __DECLARE_REGISTER__(SOF) __REGISTER_NAME__(SOF);

#define __REGISTER_NEW_FUNCTION__(type, name, buffer_type) \
    void new_ ## name ## _reg(__DECLARE_REGISTER__(type)* reg, \
            struct Addr_Region* addr_reg, \
            buffer_type* buffer)

#define __REGISTER_PORT_NEW_FUNCTION__ \
    void new_port_reg(__DECLARE_REGISTER__(Port)* reg, int pn, \
                      struct Addr_Region* addr_reg, \
                      uint16_t* buffer)

#define __REG_TYPE__ \
    type_of(__DECLARE_STRUCT__(Register*, reg))
#define __REG_RELOAD__ \
    reload(__DECLARE_STRUCT__(Register*, reg))
#define __REG_SET__ \
    set(__DECLARE_STRUCT__(Register*, reg), void* b)
#define __REG_WRITE__ \
    write(__DECLARE_STRUCT__(Register*, reg), void* b)
#define __REG_CLEAR__ \
    clear(__DECLARE_STRUCT__(Register*, reg), void* b)
#define __REG_READ__ \
    read(__DECLARE_STRUCT__(Register*, reg), void* b)

#define __REG__ _reg_

#define __REGISTER_CONCAT__(A,B) \
    __CONCAT__(A,B)

#define __REGISTER_TYPE_OF_PORT__(name, pn) \
    __REGISTER_TYPE_OF_FUNCTION__(name ## pn)

#define __REGISTER_TYPE_OF_FUNCTION__(name) \
    static Register_Type __REGISTER_CONCAT__(name, \
    __REGISTER_CONCAT__(__REG__, __REG_TYPE__)) 

#define __REGISTER_RELOAD_FUNCTION__(name) \
    static void __REGISTER_CONCAT__(name, __REGISTER_CONCAT__(__REG__, __REG_RELOAD__))

#define __REGISTER_SET_FUNCTION__(name) \
    static void __REGISTER_CONCAT__(name, __REGISTER_CONCAT__(__REG__, __REG_SET__))

#define __REGISTER_WRITE_FUNCTION__(name) \
    static size_t __REGISTER_CONCAT__(name, __REGISTER_CONCAT__(__REG__, __REG_WRITE__))

#define __REGISTER_CLEAR_FUNCTION__(name) \
    static void __REGISTER_CONCAT__(name, __REGISTER_CONCAT__(__REG__, __REG_CLEAR__))

#define __REGISTER_READ_FUNCTION__(name) \
    static size_t __REGISTER_CONCAT__(name, __REGISTER_CONCAT__(__REG__, __REG_READ__))

#define __REGISTER_DECLARE_FUNCTIONS__(type, name, buffer_type) \
    __REGISTER_TYPE_OF_FUNCTION__(name); \
    __REGISTER_RELOAD_FUNCTION__(name); \
    __REGISTER_SET_FUNCTION__(name); \
    __REGISTER_WRITE_FUNCTION__(name); \
    __REGISTER_CLEAR_FUNCTION__(name); \
    __REGISTER_READ_FUNCTION__(name)

#define __REGISTER_PORT_DECLARE_FUNCTION__ \
    __REGISTER_TYPE_OF_PORT__(port, 1); \
    __REGISTER_TYPE_OF_PORT__(port, 2); \
    __REGISTER_RELOAD_FUNCTION__(port); \
    __REGISTER_SET_FUNCTION__(port); \
    __REGISTER_WRITE_FUNCTION__(port); \
    __REGISTER_CLEAR_FUNCTION__(port); \
    __REGISTER_READ_FUNCTION__(port)

__REGISTER_NEW_FUNCTION__(Command, command, uint16_t);
__REGISTER_NEW_FUNCTION__(Status, status, uint16_t);
__REGISTER_NEW_FUNCTION__(Interrupt, interrupt, uint16_t);
__REGISTER_NEW_FUNCTION__(Frame_Numb, frame_number, uint16_t);
__REGISTER_NEW_FUNCTION__(Frame_Base, frame_base, uint32_t);
__REGISTER_NEW_FUNCTION__(SOF, sof, uint8_t);
__REGISTER_PORT_NEW_FUNCTION__;

#define __REGISTER_RELOAD_INITIALIZER__(dtype, type, reg, name) \
    dtype d1 = *((dtype*)reg->raw_data); \
    type* name = \
    (type*)container_of(reg, type, super)

#define __REGISTER_WRITE_INTIIALIZER__(dtype, reg, b) \
    size_t word_count; \
    dtype word = *((dtype *)b); \
    Addr_Region *addr_reg = reg->addr_reg; \
    Register_Type type = reg->type_of(reg); \
    word_count = addr_reg->write(addr_reg, &word, reg->length, &type); \
    *((dtype *)(reg->raw_data)) = word; \
    reg->reload(reg); \
    \
    return word_count

#define __REGISTER_SET_INITIALIZER__(dtype, reg, b) \
    dtype word = *((dtype *)b); \
    dtype raw = *((dtype *)reg->raw_data); \
    word |= raw; \
    reg->write(reg, &word); \
    reg->reload(reg)

#define __REGISTER_CLEAR_INITIALIZER__(dtype, reg, b) \
    dtype word = *((dtype *)b); \
    dtype raw = *((dtype *)reg->raw_data); \
    raw &= ~word; \
    reg->write(reg, &raw); \
    reg->reload(reg)

#define __REGISTER_READ_INITIALIZER__(dtype, reg, buffer) \
    size_t word_count; \
    Addr_Region *addr_reg = reg->addr_reg; \
    Register_Type type = reg->type_of(reg); \
    word_count = addr_reg->read(addr_reg, reg->length, &type, buffer); \
    *((dtype *)reg->raw_data) = *((dtype*)buffer); \
    reg->reload(reg); \
    \
    return word_count

#define __REGISTER_SUPER_ROUTINE__(reg, name, dump_name) \
    __SUPER__(reg, new_register) = &new_reg; \
    __SUPER__(reg, reload) = __CONCAT__(&name, _reg_reload); \
    __SUPER__(reg, type_of) = __CONCAT__(&name, _reg_type_of); \
    __SUPER__(reg, clear) = __CONCAT__(&name, _reg_clear); \
    __SUPER__(reg, write) = __CONCAT__(&name, _reg_write); \
    __SUPER__(reg, read) = __CONCAT__(&name, _reg_read); \
    __SUPER__(reg, set) = __CONCAT__(&name, _reg_set); \
    __SUPER__(reg, dump) = __CONCAT__(&dump_usb_, dump_name)

#define __REGISTER_STATUS_INITIALIZER__(reg) \
    __REGISTER_SUPER_ROUTINE__(reg, status, sts)

#define __REGISTER_COMMAND_INITIALIZER__(reg) \
    __REGISTER_SUPER_ROUTINE__(reg, command, cmd)

#define __REGISTER_INTERRUPT_INITIALIZER__(reg) \
    __REGISTER_SUPER_ROUTINE__(reg, interrupt, intr)

#define __REGISTER_FRAME_NUMBER_INITIALIZER__(reg) \
    __REGISTER_SUPER_ROUTINE__(reg, frame_number, frnum)

#define __REGISTER_FRAME_BASE_INITIALIZER__(reg) \
    __REGISTER_SUPER_ROUTINE__(reg, frame_base, flbaseadd)

#define __REGISTER_SOF_INITIALIZER__(reg) \
    __REGISTER_SUPER_ROUTINE__(reg, sof, sofmod)

#define __REGISTER_INITIALIZER__(reg, type, buffer, region, len, def_name) \
    __REGISTER_ ## type ## _INITIALIZER__(reg); \
    __mem_set((uint8_t *)buffer, len, 0); \
    __CALL_SUPER__(reg->super, new_register, def_name, len, buffer, \
                    region)

#define __REGISTER_PORT_INITIALIZER__(reg) \
    __SUPER__(reg, new_register) = &new_reg; \
    __SUPER__(reg, reload) = &port_reg_reload; \
    __SUPER__(reg, clear) = &port_reg_clear; \
    __SUPER__(reg, write) = &port_reg_write; \
    __SUPER__(reg, read) = &port_reg_read; \
    __SUPER__(reg, set) = &port_reg_set; \
    __SUPER__(reg, dump) = &dump_usb_portsc


#define __REGISTER_PORT_SUB_ROUTINE_INTERMEDIATE__(A, B) \
    __CONCAT__(A,B)

#define __REGISTER_PORT_SUB_ROUTINE__(reg, pn, buffer, region) \
    __REGISTER_PORT_SUB_ROUTINE_INTERMEDIATE__(reg->super.type_of = \
      __REGISTER_PORT_SUB_ROUTINE_INTERMEDIATE__(&port, pn), \
      __REGISTER_PORT_SUB_ROUTINE_INTERMEDIATE__(__REG__, type_of)); \
    __CALL_SUPER__(reg->super, new_register, __DEF_NAME__(PORT, pn), \
                   0x02, buffer, region)

#define __DECLARE_DUMP_USB_REG__(name) \
    static void dump_usb_ ## name(Register* reg, Logger_C* logger)

#define __REGISTER_DEFAULT_NEW__(reg_sufix, reg_name, buffer_type, \
    define_reg, reg_init_routine, size) \
    __REGISTER_NEW_FUNCTION__(reg_sufix, reg_name, buffer_type) { \
        __REGISTER_INITIALIZER__(reg, reg_init_routine, buffer, addr_reg, \
            size, define_reg); \
    }

#define __REGISTER_DEFAULT_READ__(name, buffer_type) \
    __REGISTER_READ_FUNCTION__(name) { \
        __REGISTER_READ_INITIALIZER__(buffer_type, reg, b); \
    }

#define __REGISTER_DEFAULT_CLEAR__(name, buffer_type) \
    __REGISTER_CLEAR_FUNCTION__(name) { \
        __REGISTER_CLEAR_INITIALIZER__(buffer_type, reg, b); \
    }

#define __REGISTER_DEFAULT_WRITE__(name, buffer_type) \
    __REGISTER_WRITE_FUNCTION__(name) { \
        __REGISTER_WRITE_INTIIALIZER__(buffer_type, reg, b); \
    }

#define __REGISTER_DEFAULT_SET__(name, buffer_type) \
    __REGISTER_SET_FUNCTION__(name){ \
        __REGISTER_SET_INITIALIZER__(buffer_type, reg, b); \
    }

#endif