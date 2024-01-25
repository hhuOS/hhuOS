#ifndef UHCI_REGISTER_INCLUDE
#define UHCI_REGISTER_INCLUDE

#include "../../../utility/Utils.h"
#include "stdint.h"
#include "UHCIMemory.h"
#include "../../../interfaces/LoggerInterface.h"

#define STRINGIZE(x) #x
#define CONCAT(a,b) STRINGIZE(a ## _ ## b)
#define Frame(n) CONCAT(frame,n)
#define Port(n) CONCAT(port,n)
#define Usb(n) CONCAT(usb,n)
#define DEF_NAME(v1,v2) v1(v2)

enum Register_Type {
    Usb_Command = 0x00,
    Usb_Status  = 0x02,
    Usb_Interrupt = 0x04,
    Frame_Number = 0x06,
    Frame_List_Base_Address = 0x08,
    Start_of_Frame = 0x0C,
    Port1_Status = 0x10,
    Port2_Status = 0x12
};

typedef enum Register_Type Register_Type;

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
    ENA_CHANGE = 0x0008,
    ENA        = 0x0004,
    CON_CHANGE = 0x0002,
    CONNECT    = 0x0001
};

struct Register{
        struct Addr_Region* addr_reg;
        void* raw_data;
        void (*new_register)(struct Register* reg, char* name, uint8_t length, void* raw, struct Addr_Region* addr_reg);
        void (*reload)(struct Register* reg);
        size_t (*write)(struct Register* reg, void* b);
        void (*set)(struct Register* reg, void* b);
        size_t (*read)(struct Register* reg, void* buffer);
        void (*clear)(struct Register* reg, void* b);
        Register_Type (*type_of)(struct Register* reg);
        void (*dump)(struct Register* reg, Logger_C* logger);
        char* name;
        uint8_t length;
};

typedef struct Register Register;

struct Command_Register{
    Register super;
    void (*new_command_register)(struct Command_Register* reg, struct Addr_Region* addr_region, uint16_t* buffer);
    uint8_t max_packet;
    uint8_t configure;
    uint8_t software_debug;
    uint8_t global_resume;
    uint8_t global_suspend;
    uint8_t global_reset;
    uint8_t host_controller_reset;
    uint8_t run;
};

struct Status_Register{
    Register super;
    void (*new_status_register)(struct Status_Register* reg, struct Addr_Region* addr_region, uint16_t* buffer);
    uint8_t halted;
    uint8_t process_error;
    uint8_t system_error;
    uint8_t resume_detected;
    uint8_t error_interrupt;
    uint8_t interrupt;
};

struct Interrupt_Register{
    Register super;
    void (*new_interrupt_register)(struct Interrupt_Register* reg, struct Addr_Region* addr_region, uint16_t* buffer);
    uint8_t short_packet;
    uint8_t complete;
    uint8_t resume;
    uint8_t timeout_crc;
};

struct Port_Register{
    Register super;
    void (*new_port_register)(struct Port_Register* reg, int pn, struct Addr_Region* addr_region, uint16_t* buffer);
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

struct Frame_Numb_Register{
    Register super;
    void (*new_frame_number_register)(struct Frame_Numb_Register* reg, struct Addr_Region* addr_region, uint16_t* buffer);
    uint16_t frame_num;
};

struct Frame_Base_Register{
    Register super;
    void (*new_frame_base_register)(struct Frame_Base_Register* reg, struct Addr_Region* addr_region, uint32_t* buffer);
    uint32_t frame_address;
};

struct SOF_Register{
    Register super;
    void (*new_sof_register)(struct SOF_Register* reg, struct Addr_Region* addr_region, uint8_t* buffer);
    uint8_t frame_duration;
};

typedef struct Command_Register Command_Register;
typedef struct Status_Register Status_Register;
typedef struct Interrupt_Register Interrupt_Register;
typedef struct Port_Register Port_Register;
typedef struct Frame_Numb_Register Frame_Numb_Register;
typedef struct Frame_Base_Register Frame_Base_Register;
typedef struct SOF_Register SOF_Register;


void new_reg(struct Register *reg, char *name, uint8_t length, void *raw, struct Addr_Region *addr_reg);

Register_Type command_reg_type_of(struct Register *reg);
void command_reg_reload(struct Register *reg);
void command_reg_set(struct Register* reg, void* b);
size_t command_reg_write(struct Register *reg, void* b);
void command_reg_clear(struct Register* reg, void* b);
size_t command_reg_read(struct Register *reg, void* buffer);
void new_command_reg(struct Command_Register *reg, struct Addr_Region *addr_reg, uint16_t* buffer);

Register_Type status_reg_type_of(struct Register *reg);
void status_reg_reload(struct Register *reg);
void status_reg_set(struct Register* reg, void* b);
size_t status_reg_write(struct Register *reg, void* b);
//void status_reg_clear(struct Register* reg, void* b);
size_t status_reg_read(struct Register *reg, void* buffer);
void new_status_reg(struct Status_Register *reg, struct Addr_Region* addr_reg, uint16_t* buffer);

Register_Type interrupt_reg_type_of(struct Register *reg);
void interrupt_reg_reload(struct Register *reg);
void interrupt_reg_set(struct Register* reg, void* b);
void interrupt_reg_clear(struct Register* reg, void* b);
size_t interrupt_reg_write(struct Register *reg, void* b);
size_t interrupt_reg_read(struct Register *reg, void* buffer);
void new_interrupt_reg(struct Interrupt_Register *reg, struct Addr_Region* addr_reg, uint16_t* buffer);

Register_Type port_reg_type_of_2(struct Register *reg);
Register_Type port_reg_type_of_1(struct Register *reg);
void port_reg_reload(struct Register *reg);
size_t port_reg_write(struct Register *reg,void* b);
void port_reg_clear(struct Register* reg, void* b);
void port_reg_set(struct Register* reg, void* b);
size_t port_reg_read(struct Register *reg, void* buffer);
void new_port_reg(struct Port_Register *reg, int pn, struct Addr_Region* addr_reg, uint16_t* buffer);

Register_Type frame_number_reg_type_of(struct Register *reg);
void frame_number_reg_reload(struct Register *reg);
size_t frame_number_reg_write(struct Register *reg, void* b);
size_t frame_number_reg_read(struct Register *reg, void* buffer);
void new_frame_number_reg(struct Frame_Numb_Register *reg, struct Addr_Region* addr_reg, uint16_t* buffer);

Register_Type frame_base_reg_type_of(struct Register *reg);
void frame_base_reg_reload(struct Register *reg);
size_t frame_base_reg_write(struct Register *reg, void* b);
size_t frame_base_reg_read(struct Register *reg, void* buffer);
void new_frame_base_reg(struct Frame_Base_Register *reg, struct Addr_Region* addr_reg, uint32_t* buffer);

Register_Type sof_reg_type_of(struct Register *reg);
void sof_reg_reload(struct Register *reg);
size_t sof_reg_write(struct Register *reg, void* b);
size_t sof_reg_read(struct Register *reg, void* buffer);
void new_sof_reg(struct SOF_Register *reg, struct Addr_Region* addr_reg, uint8_t* buffer);

void dump_usb_cmd(Register* reg, Logger_C* logger);
void dump_usb_sts(Register* reg, Logger_C* logger);
void dump_usb_intr(Register* reg, Logger_C* logger);
void dump_usb_portsc(Register* reg, Logger_C* logger);
void dump_usb_frnum(Register* reg, Logger_C* logger);
void dump_usb_flbaseadd(Register* reg, Logger_C* logger);
void dump_usb_sofmod(Register* reg, Logger_C* logger);

#endif