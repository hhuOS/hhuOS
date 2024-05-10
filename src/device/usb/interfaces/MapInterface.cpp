#include "MapInterface.h"
#include "../../../lib/util/collection/HashMap.h"
#include "../../../lib/util/collection/Map.h"
#include "../controller/uhci/data/UHCI_Data.h"
#include "../controller/uhci/UHCI.h"
#include "../controller/uhci/components/UHCIRegister.h"
#include "../driver/storage/CommandInterface.h"
#include "../driver/storage/MassStorageDriver.h"

extern "C" {
__DEFINE_MAP_FUNCTIONS__(QH_Status);
__DEFINE_MAP_FUNCTIONS__(QH_Callback);
__DEFINE_MAP_FUNCTIONS__(Register);
__DEFINE_MAP_FUNCTIONS__(QH_TD);
__DEFINE_MAP_FUNCTIONS__(QH_Device_Map);
__DEFINE_MAP_FUNCTIONS__(QH_Data_Map);
__DEFINE_MAP_FUNCTIONS__(event_map);
__DEFINE_MAP_FUNCTIONS__(interface_device_map);
__DEFINE_MAP_FUNCTIONS__(address_map);
__DEFINE_MAP_FUNCTIONS__(address_td_map);
__DEFINE_MAP_FUNCTIONS__(qh_device_request);
__DEFINE_MAP_FUNCTIONS__(qh_measurement);
__DEFINE_MAP_FUNCTIONS__(command_block_int);
__DEFINE_MAP_FUNCTIONS__(command_status_int);
__DEFINE_MAP_FUNCTIONS__(data_int);
__DEFINE_MAP_FUNCTIONS__(int_callback);
__DEFINE_MAP_FUNCTIONS__(int_buffer);
__DEFINE_MAP_FUNCTIONS__(int_t_len);
__DEFINE_MAP_FUNCTIONS__(int_mem_buffer);
}

static inline void new_super_map(struct SuperMap *s, const char *map_descript,
                   Map_C pointer) {
  s->map_description = map_descript;
  s->map_pointer = pointer;
}

__MAP_INIT_PTR__(newQH_TD, QH_TD_Map, QH_TD, QH, TD);
 
__MAP_INIT_VALUE__(newRegisterMap, Register_Map, Register, Register_Type,
  Register);

__MAP_INIT_PTR__(newQH_CallbackMap, QH_Callback_Function_Map, QH_Callback,
  QH, callback_function);

__MAP_INIT_PTR__(newQH_StatusMap, QH_Status_Map, QH_Status, QH, uint8_t);

__MAP_INIT_PTR__(newQH_Data_Map, QH_Data_Map, QH_Data_Map, QH, void);

__MAP_INIT_PTR__(newQH_Device_Map, QH_Device_Map, QH_Device_Map, QH, UsbDev);

__MAP_INIT_VALUE__(newEventMap, EventMap, event_map, int, EventListener);

__MAP_INIT_PTR__(newInterface_Device_Map, Interface_Device_Map, 
  interface_device_map, Interface, UsbDev);

__MAP_INIT_VALUE__(newAddressMap, Address_Map, address_map, uint32_t, QH);

__MAP_INIT_VALUE__(newAddressTDMap, Address_TD_Map, address_td_map, 
  uint32_t, TD);

__MAP_INIT_PTR__(newQH_DeviceRequest_Map, QH_Device_Request_Map, 
  qh_device_request, QH, UsbDeviceRequest);

__MAP_INIT_PTR__(newQH_Measuremnt_Map, QH_Measurement_Map, qh_measurement, 
  QH, uint32_t);

__MAP_INIT_PTR__(newCommandBlockIntMap, CommandBlockWrapper_Int_Map, 
  command_block_int, CommandBlockWrapper, uint32_t);

__MAP_INIT_PTR__(newCommandStatusIntMap, CommandStatusWrapper_Int_Map,
  command_status_int, CommandStatusWrapper, uint32_t);

__MAP_INIT_PTR__(newDataIntMap, Data_Int_Map, data_int, uint8_t, uint32_t);

__MAP_INIT_VALUE__(newIntCallbackMap, Int_Callback_Map, int_callback,
  uint32_t, msd_callback);

__MAP_INIT_VALUE__(newIntBufferMap, Int_Buffer_Map, int_buffer,
  uint32_t, uint8_t);

__MAP_INIT_VALUE__(newIntTLenMap, Int_T_Len_Map, int_t_len, uint32_t, 
  uint32_t);

__MAP_INIT_VALUE__(newIntMemBufferMap, Int_Mem_Buffer_Map, int_mem_buffer,
  uint32_t, uint8_t);