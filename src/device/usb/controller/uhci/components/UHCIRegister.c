#include "UHCIRegister.h"
#include "../../../dev/UsbDevice.h"
#include "../../../interfaces/LoggerInterface.h"
#include "../../UsbControllerFlags.h"
#include "../../components/ControllerMemory.h"
#include "stdint.h"

__REGISTER_DECLARE_FUNCTIONS__(Command, command, uint16_t);
__REGISTER_DECLARE_FUNCTIONS__(Status, status, uint16_t);
__REGISTER_DECLARE_FUNCTIONS__(Interrupt, interrupt, uint16_t);
__REGISTER_PORT_DECLARE_FUNCTION__;
__REGISTER_DECLARE_FUNCTIONS__(Frame_Numb, frame_number, uint16_t);
__REGISTER_DECLARE_FUNCTIONS__(Frame_Base, frame_base, uint32_t);
__REGISTER_DECLARE_FUNCTIONS__(SOF, sof, uint8_t);

__DECLARE_DUMP_USB_REG__(cmd);
__DECLARE_DUMP_USB_REG__(sts);
__DECLARE_DUMP_USB_REG__(intr);
__DECLARE_DUMP_USB_REG__(portsc);
__DECLARE_DUMP_USB_REG__(frnum);
__DECLARE_DUMP_USB_REG__(flbaseadd);
__DECLARE_DUMP_USB_REG__(sofmod);

__REGISTER_TYPE_OF_FUNCTION__(command) { return Usb_Command; }

__REGISTER_RELOAD_FUNCTION__(command){
  __REGISTER_RELOAD_INITIALIZER__(uint16_t, __REGISTER_NAME__(Command), reg, creg);
  creg->max_packet = (d1 & MAXP) >> 7;
  creg->configure = (d1 & CF) >> 6;
  creg->software_debug = (d1 & SWDBG) >> 5;
  creg->global_resume = (d1 & FGR) >> 4;
  creg->global_suspend = (d1 & EGSM) >> 3;
  creg->global_reset = (d1 & GRESET) >> 2;
  creg->host_controller_reset = (d1 & HCRESET) >> 1;
  creg->run = (d1 & RS);
}

__REGISTER_SET_FUNCTION__(command){
  __REGISTER_SET_INITIALIZER__(uint16_t, reg, b);
}

__REGISTER_WRITE_FUNCTION__(command) {
  __REGISTER_WRITE_INTIIALIZER__(uint16_t, reg, b);
}

__REGISTER_CLEAR_FUNCTION__(command) {
  __REGISTER_CLEAR_INITIALIZER__(uint16_t, reg, b);
}

__REGISTER_READ_FUNCTION__(command) {
  __REGISTER_READ_INITIALIZER__(uint16_t, reg, b);
}

__REGISTER_NEW_FUNCTION__(Command, command, uint16_t){
  __REGISTER_INITIALIZER__(reg, COMMAND, buffer, addr_reg, 0x02, 
    __DEF_NAME__(USB, COMMAND));
}

// status register is write clear !
__REGISTER_TYPE_OF_FUNCTION__(status) { return Usb_Status; }

__REGISTER_RELOAD_FUNCTION__(status) {
  __REGISTER_RELOAD_INITIALIZER__(uint16_t, __REGISTER_NAME__(Status), reg, sreg);
  sreg->halted = (d1 & HALTED) >> 5;
  sreg->process_error = (d1 & PCS_ERR) >> 4;
  sreg->system_error = (d1 & SYS_ERR) >> 3;
  sreg->resume_detected = (d1 & RES_DTC) >> 2;
  sreg->error_interrupt = (d1 & ERR_INT) >> 1;
  sreg->interrupt = (d1 & INT);
}

__REGISTER_CLEAR_FUNCTION__(status) {
  uint16_t word = *((uint16_t *)b);

  reg->write(reg, &word);
}

// 0x00FF
__REGISTER_WRITE_FUNCTION__(status) {
  size_t word_count;
  uint16_t word = *((uint16_t *)b);
  Addr_Region *addr_reg = reg->addr_reg;
  Register_Type type = reg->type_of(reg);
  word_count = addr_reg->write(addr_reg, &word, reg->length, &type);

  *((uint16_t *)(reg->raw_data)) &= ~word;
  reg->reload(reg);

  return word_count;
}

__REGISTER_READ_FUNCTION__(status) {
  __REGISTER_READ_INITIALIZER__(uint16_t, reg, b);
}

__REGISTER_SET_FUNCTION__(status){}

__REGISTER_NEW_FUNCTION__(Status, status, uint16_t) {
  __REGISTER_INITIALIZER__(reg, STATUS, buffer, addr_reg, 0x02, 
    __DEF_NAME__(USB, STATUS));
}

__REGISTER_TYPE_OF_FUNCTION__(interrupt) { return Usb_Interrupt; }

__REGISTER_RELOAD_FUNCTION__(interrupt) {
  __REGISTER_RELOAD_INITIALIZER__(uint16_t, __REGISTER_NAME__(Interrupt), reg, ireg);
  ireg->short_packet = (d1 & SHORT_PACK) >> 3;
  ireg->complete = (d1 & COMPLETE) >> 2;
  ireg->resume = (d1 & RESUME) >> 1;
  ireg->timeout_crc = (d1 & TMOUT_CRC);
}

__REGISTER_WRITE_FUNCTION__(interrupt) {
  __REGISTER_WRITE_INTIIALIZER__(uint16_t, reg, b);
}

__REGISTER_SET_FUNCTION__(interrupt) {
  __REGISTER_SET_INITIALIZER__(uint16_t, reg, b);
}

__REGISTER_CLEAR_FUNCTION__(interrupt) {
  __REGISTER_CLEAR_INITIALIZER__(uint16_t, reg, b);
}

__REGISTER_READ_FUNCTION__(interrupt) {
  __REGISTER_READ_INITIALIZER__(uint16_t, reg, b);
}

__REGISTER_NEW_FUNCTION__(Interrupt, interrupt, uint16_t) {
  __REGISTER_INITIALIZER__(reg, INTERRUPT, buffer, addr_reg, 0x02, 
    __DEF_NAME__(USB, INTERRUPT));
}

__REGISTER_RELOAD_FUNCTION__(port) {
  __REGISTER_RELOAD_INITIALIZER__(uint16_t, __REGISTER_NAME__(Port), reg, preg);
  preg->suspend = (d1 & SUSPEND) >> 12;
  preg->reset = (d1 & RESET) >> 9;
  preg->low_speed = (d1 & LOW_SPEED_ATTACH) >> 8;
  preg->resume_detected = (d1 & RES_DET) >> 6;
  preg->line_status_d_minus = (d1 & LINE_STS_M) >> 5;
  preg->line_status_d_plus = (d1 & LINE_STS_P) >> 4;
  preg->port_enable_changed = (d1 & ENA_CHANGE) >> 3;
  preg->port_enabled = (d1 & ENA) >> 2;
  preg->connected_changed = (d1 & CON_CHANGE) >> 1;
  preg->connected = (d1 & CONNECT);
}

__REGISTER_TYPE_OF_PORT__(port, 1) { return Port1_Status; }

__REGISTER_TYPE_OF_PORT__(port, 2) { return Port2_Status; }

__REGISTER_WRITE_FUNCTION__(port) {
  size_t word_count;
  uint16_t word = *((uint16_t *)b);
  Addr_Region *addr_reg = reg->addr_reg;
  Register_Type type = reg->type_of(reg);
  word_count = addr_reg->write(addr_reg, &word, reg->length, &type);

  *((uint16_t *)(reg->raw_data)) = (word & ~(CON_CHANGE | ENA_CHANGE)) | 
    ((*((uint16_t *)(reg->raw_data)) & (CON_CHANGE | ENA_CHANGE)) & 
    ~(word & (CON_CHANGE | ENA_CHANGE)));

  reg->reload(reg);

  return word_count;
}

__REGISTER_READ_FUNCTION__(port) {
  __REGISTER_READ_INITIALIZER__(uint16_t, reg, b);
}

__REGISTER_SET_FUNCTION__(port){}
__REGISTER_CLEAR_FUNCTION__(port){}

__REGISTER_PORT_NEW_FUNCTION__ {
  __REGISTER_PORT_INITIALIZER__(reg);

  if (pn == 1) {
    __REGISTER_PORT_SUB_ROUTINE__(reg, 1, buffer, addr_reg);
  } else if (pn == 2) {
    __REGISTER_PORT_SUB_ROUTINE__(reg, 2, buffer, addr_reg);
  }
}

__REGISTER_RELOAD_FUNCTION__(frame_number) {
  __REGISTER_RELOAD_INITIALIZER__(uint16_t, __REGISTER_NAME__(Frame_Numb), reg, 
    f_n_reg);

  f_n_reg->frame_num = d1;
}

__REGISTER_TYPE_OF_FUNCTION__(frame_number) { return Frame_Number; }

__REGISTER_WRITE_FUNCTION__(frame_number) {
  __REGISTER_WRITE_INTIIALIZER__(uint16_t, reg, b);
}

__REGISTER_READ_FUNCTION__(frame_number) {
  __REGISTER_READ_INITIALIZER__(uint16_t, reg, b);
}

__REGISTER_SET_FUNCTION__(frame_number) {}
__REGISTER_CLEAR_FUNCTION__(frame_number) {}

__REGISTER_NEW_FUNCTION__(Frame_Numb, frame_number, uint16_t) {
  __REGISTER_INITIALIZER__(reg, FRAME_NUMBER, buffer, addr_reg, 0x02, 
    __DEF_NAME__(FRAME, NUMBER));
}

__REGISTER_RELOAD_FUNCTION__(frame_base) {
  __REGISTER_RELOAD_INITIALIZER__(uint32_t, __REGISTER_NAME__(Frame_Base), reg, 
    f_b_r);
  f_b_r->frame_address = d1;
}

__REGISTER_TYPE_OF_FUNCTION__(frame_base) {
  return Frame_List_Base_Address;
}

__REGISTER_WRITE_FUNCTION__(frame_base) {
  __REGISTER_WRITE_INTIIALIZER__(uint32_t, reg, b);
}

__REGISTER_READ_FUNCTION__(frame_base) {
  __REGISTER_READ_INITIALIZER__(uint32_t, reg, b);
}

__REGISTER_CLEAR_FUNCTION__(frame_base) {}
__REGISTER_SET_FUNCTION__(frame_base) {}

__REGISTER_NEW_FUNCTION__(Frame_Base, frame_base, uint32_t) {
  __REGISTER_INITIALIZER__(reg, FRAME_BASE, buffer, addr_reg, 0x04, 
    __DEF_NAME__(FRAME, BASE_ADDR));
}

__REGISTER_RELOAD_FUNCTION__(sof){
  __REGISTER_RELOAD_INITIALIZER__(uint8_t, __REGISTER_NAME__(SOF), reg, sof);
  sof->frame_duration = d1;
}

__REGISTER_TYPE_OF_FUNCTION__(sof) { return Start_of_Frame; }

__REGISTER_WRITE_FUNCTION__(sof) {
  __REGISTER_WRITE_INTIIALIZER__(uint8_t, reg, b);
}

__REGISTER_READ_FUNCTION__(sof) {
  __REGISTER_READ_INITIALIZER__(uint8_t, reg, b);
}

__REGISTER_CLEAR_FUNCTION__(sof) {}
__REGISTER_SET_FUNCTION__(sof) {}

__REGISTER_NEW_FUNCTION__(SOF, sof, uint8_t) {
  __REGISTER_INITIALIZER__(reg, SOF, buffer, addr_reg, 0x01, 
    __DEF_NAME__(USB, SOF));
}

static void dump_usb_cmd(Register *reg, Logger_C *logger) {
  if (logger == (void *)0)
    return;
  Command_Register *c_reg =
      (Command_Register *)container_of(reg, Command_Register, super);

  char *c_reg_msg = "Command Register : \n\nMAXP : %u\nCF : %u\n"
                    "SWDBG : %u\nFGR : %u \nEGSM : %u\nGRESET : %u\n"
                    "HCRESET : %u\nRS : %u\n";

  logger->debug_c(logger, c_reg_msg, c_reg->max_packet, c_reg->configure,
                  c_reg->software_debug, c_reg->global_reset,
                  c_reg->global_suspend, c_reg->global_reset,
                  c_reg->host_controller_reset, c_reg->run);
}

static void dump_usb_sts(Register *reg, Logger_C *logger) {
  if (logger == (void *)0)
    return;
  Status_Register *status_reg =
      (Status_Register *)container_of(reg, Status_Register, super);

  char *status_reg_msg =
      "Status Register : \n\nHCHalted : %u\nHCProcessError : %u\n"
      "HostSystemError : %u\nResumeDetect : %u\n"
      "UsbError : %u\nUsbInterrupt : %u\n";

  logger->debug_c(logger, status_reg_msg, status_reg->halted,
                  status_reg->process_error, status_reg->system_error,
                  status_reg->resume_detected, status_reg->error_interrupt,
                  status_reg->interrupt);
}

static void dump_usb_intr(Register *reg, Logger_C *logger) {
  if (logger == (void *)0)
    return;
  Interrupt_Register *intr_reg =
      (Interrupt_Register *)container_of(reg, Interrupt_Register, super);

  char *intr_reg_msg =
      "Interrupt Register : \n\nSPI : %u\nIOC : %u\nRI : %u\nTO/CRC : %u";

  logger->debug_c(logger, intr_reg_msg, intr_reg->short_packet,
                  intr_reg->complete, intr_reg->resume, intr_reg->timeout_crc);
}

static void dump_usb_portsc(Register *reg, Logger_C *logger) {
  if (logger == (void *)0)
    return;
  Port_Register *port_reg =
      (Port_Register *)container_of(reg, Port_Register, super);

  char *port_reg_msg = "Port Register : \n\nSuspend : %u\nPortReset : %u\n"
                       "LowSpeed : %u\nResumeDetected : %u\nLineStatusD- : %u\n"
                       "LineStatusD+ : %u\nPortEDChange : %u\nPortED : %u\n"
                       "CSC : %u\nCCS : %u\n";

  logger->debug_c(logger, port_reg_msg, port_reg->suspend, port_reg->reset,
                  port_reg->low_speed, port_reg->resume_detected,
                  port_reg->line_status_d_minus, port_reg->line_status_d_plus,
                  port_reg->port_enable_changed, port_reg->port_enabled,
                  port_reg->connected_changed, port_reg->connected);
}

static void dump_usb_frnum(Register *reg, Logger_C *logger) {
  if (logger == (void *)0)
    return;
  Frame_Numb_Register *fn_reg =
      (Frame_Numb_Register *)container_of(reg, Frame_Numb_Register, super);

  char *fn_reg_msg = "Frame Number Register : \n\nFrame Number : %u\n";

  logger->debug_c(logger, fn_reg_msg, fn_reg->frame_num);
}

static void dump_usb_flbaseadd(Register *reg, Logger_C *logger) {
  if (logger == (void *)0)
    return;
  Frame_Base_Register *fb_reg =
      (Frame_Base_Register *)container_of(reg, Frame_Base_Register, super);

  char *fb_reg_msg = "Frame Base Register : \n\nFrame Address : %u\n";

  logger->debug_c(logger, fb_reg_msg, fb_reg->frame_address);
}

static void dump_usb_sofmod(Register *reg, Logger_C *logger) {
  if (logger == (void *)0)
    return;
  SOF_Register *sof_reg =
      (SOF_Register *)container_of(reg, SOF_Register, super);

  char *sof_reg_msg = "SOF Register : \n\nFrame Duration : %u\n";

  logger->debug_c(logger, sof_reg_msg, sof_reg->frame_duration);
}