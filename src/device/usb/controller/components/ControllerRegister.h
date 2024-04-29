#ifndef CONTROLLER_REGISTER_INCLUDE
#define CONTROLLER_REGISTER_INCLUDE

#include "../../interfaces/LoggerInterface.h"
#include "../../utility/Utils.h"
#include "ControllerMemory.h"

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

struct Register{
    struct Addr_Region* addr_reg;
    void* raw_data;
    void (*new_register)(struct Register* reg, char* name, uint8_t length, 
        void* raw, struct Addr_Region* addr_reg);
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

static inline void new_reg(Register *reg, char *name, uint8_t length, void *raw,
             struct Addr_Region *addr_reg) {
  reg->raw_data = raw;
  reg->name = name;
  reg->length = length;
  reg->addr_reg = addr_reg;
}

#endif