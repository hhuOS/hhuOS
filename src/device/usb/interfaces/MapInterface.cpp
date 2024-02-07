#include "MapInterface.h"
#include "../../../lib/util/collection/HashMap.h"
#include "../../../lib/util/collection/Map.h"
#include "../controller/uhci/data/UHCI_Data.h"
#include "../controller/uhci/UHCI.h"
#include "../controller/uhci/components/UHCIRegister.h"
#include "../driver/storage/CommandInterface.h"
#include "../driver/storage/MassStorageDriver.h"

void new_super_map(struct SuperMap *s, const char *map_descript,
                   Map_C pointer) {
  s->map_description = map_descript;
  s->map_pointer = pointer;
}

void newQH_TD(struct QH_TD_Map *map, const char *map_description) {
  map->super.get_c = &get_c_QH_TD;
  map->super.contains_c = &contains_c_QH_TD;
  map->super.put_c = &put_c_QH_TD;
  map->super.remove_c = &remove_c_QH_TD;
  map->super.new_super_map = &new_super_map;

  void *map_pointer = (Map_C) new Util::HashMap<QH *, TD *>();
  map->super.new_super_map(&map->super, map_description, map_pointer);
}

void *get_c_QH_TD(struct SuperMap *m, void *key) {
  if (((Util::Map<QH *, TD *> *)m->map_pointer)->containsKey((QH *)key)) {
    return ((Util::Map<QH *, TD *> *)m->map_pointer)->get((QH *)key);
  }
  return (void *)0;
}

int contains_c_QH_TD(struct SuperMap *m, void *key) {
  return ((Util::Map<QH *, TD *> *)m->map_pointer)->containsKey((QH *)key);
}

void put_c_QH_TD(struct SuperMap *m, void *key, void *value) {
  ((Util::Map<QH *, TD *> *)m->map_pointer)->put((QH *)key, (TD *)value);
}

void *remove_c_QH_TD(struct SuperMap *m, void *key) {
  return ((Util::Map<QH *, TD *> *)m->map_pointer)->remove((QH *)key);
}

void newRegisterMap(struct Register_Map *map, const char *map_description) {
  map->super.get_c = &get_c_Register;
  map->super.contains_c = &contains_c_Register;
  map->super.put_c = &put_c_Register;
  map->super.remove_c = &remove_c_Register;
  map->super.new_super_map = &new_super_map;

  void *map_pointer = (Map_C) new Util::HashMap<Register_Type, Register *>();
  map->super.new_super_map(&map->super, map_description, map_pointer);
}

void *get_c_Register(struct SuperMap *m, void *key) {
  if (((Util::Map<Register_Type, Register *> *)m->map_pointer)
          ->containsKey(*((Register_Type *)key)))
    return (((Util::Map<Register_Type, Register *> *)m->map_pointer)
                ->get(*((Register_Type *)key)));
  return (void *)0;
}

int contains_c_Register(struct SuperMap *m, void *key) {
  return ((Util::Map<Register_Type, Register *> *)m->map_pointer)
      ->containsKey(*((Register_Type *)key));
}

void put_c_Register(struct SuperMap *m, void *key, void *value) {
  ((Util::Map<Register_Type, Register *> *)m->map_pointer)
      ->put(*((Register_Type *)key), (Register *)value);
}

void *remove_c_Register(struct SuperMap *m, void *key) {
  return ((Util::Map<Register_Type, Register *> *)m->map_pointer)
      ->remove(*((Register_Type *)key));
}

void newQH_CallbackMap(struct QH_Callback_Function_Map *map,
                       const char *map_description) {
  map->super.get_c = &get_c_QH_Callback;
  map->super.contains_c = &contains_c_QH_Callback;
  map->super.put_c = &put_c_QH_Callback;
  map->super.remove_c = &remove_c_QH_Callback;
  map->super.new_super_map = &new_super_map;

  void *map_pointer = (Map_C) new Util::HashMap<QH *, callback_function>();
  map->super.new_super_map(&map->super, map_description, map_pointer);
}

void *get_c_QH_Callback(struct SuperMap *m, void *key) {
  if (((Util::Map<QH *, callback_function *> *)m->map_pointer)
          ->containsKey((QH *)key)) {
    return ((Util::Map<QH *, callback_function *> *)m->map_pointer)
        ->get((QH *)key);
  }
  return (void *)0;
}

int contains_c_QH_Callback(struct SuperMap *m, void *key) {
  return (int)(((Util::Map<QH *, callback_function> *)m->map_pointer)
                   ->containsKey((QH *)key));
}

void put_c_QH_Callback(struct SuperMap *m, void *key, void *value) {
  ((Util::Map<QH *, callback_function> *)m->map_pointer)
      ->put((QH *)key, (callback_function)value);
}

void *remove_c_QH_Callback(struct SuperMap *m, void *key) {
  return (void *)(((Util::Map<QH *, callback_function> *)m->map_pointer)
                      ->remove((QH *)key));
}

void newQH_StatusMap(struct QH_Status_Map *map, const char *map_description) {
  map->super.get_c = &get_c_QH_Status;
  map->super.contains_c = &contains_c_QH_Status;
  map->super.put_c = &put_c_QH_Status;
  map->super.remove_c = &remove_c_QH_Status;
  map->super.new_super_map = &new_super_map;

  void *map_pointer = (Map_C) new Util::HashMap<QH *, uint8_t *>();
  map->super.new_super_map(&map->super, map_description, map_pointer);
}

void *get_c_QH_Status(struct SuperMap *m, void *key) {
  if (((Util::Map<QH *, uint8_t *> *)m->map_pointer)->containsKey((QH *)key))
    return ((Util::Map<QH *, uint8_t *> *)m->map_pointer)->get((QH *)key);
  return (void *)0;
}

int contains_c_QH_Status(struct SuperMap *m, void *key) {
  return ((Util::Map<QH *, uint8_t *> *)m->map_pointer)->containsKey((QH *)key);
}

void put_c_QH_Status(struct SuperMap *m, void *key, void *value) {
  ((Util::Map<QH *, uint8_t *> *)m->map_pointer)
      ->put((QH *)key, (uint8_t *)value);
}

void *remove_c_QH_Status(struct SuperMap *m, void *key) {
  return ((Util::Map<QH *, uint8_t *> *)m->map_pointer)->remove((QH *)key);
}

void newQH_Data_Map(struct QH_Data_Map *map, const char *map_description) {
  map->super.get_c = &get_c_QH_Data_Map;
  map->super.contains_c = &contains_c_QH_Data_Map;
  map->super.put_c = &put_c_QH_Data_Map;
  map->super.remove_c = &remove_c_QH_Data_Map;
  map->super.new_super_map = &new_super_map;

  void *map_pointer = (Map_C) new Util::HashMap<QH *, void *>();
  map->super.new_super_map(&map->super, map_description, map_pointer);
}

void *get_c_QH_Data_Map(struct SuperMap *m, void *key) {
  if (((Util::Map<QH *, void *> *)m->map_pointer)->containsKey((QH *)key))
    return ((Util::Map<QH *, void *> *)m->map_pointer)->get((QH *)key);
  return (void *)0;
}

int contains_c_QH_Data_Map(struct SuperMap *m, void *key) {
  return ((Util::Map<QH *, void *> *)m->map_pointer)->containsKey((QH *)key);
}

void put_c_QH_Data_Map(struct SuperMap *m, void *key, void *value) {
  ((Util::Map<QH *, void *> *)m->map_pointer)->put((QH *)key, value);
}

void *remove_c_QH_Data_Map(struct SuperMap *m, void *key) {
  return ((Util::Map<QH *, void *> *)m->map_pointer)->remove((QH *)key);
}

void newQH_Device_Map(struct QH_Device_Map *map, const char *map_description) {
  map->super.get_c = &get_c_QH_Device_Map;
  map->super.contains_c = &contains_c_QH_Device_Map;
  map->super.put_c = &put_c_QH_Device_Map;
  map->super.remove_c = &remove_c_QH_Device_Map;
  map->super.new_super_map = &new_super_map;

  void *map_pointer = (Map_C) new Util::HashMap<QH *, UsbDev *>();
  map->super.new_super_map(&map->super, map_description, map_pointer);
}

void *get_c_QH_Device_Map(struct SuperMap *m, void *key) {
  if (((Util::Map<QH *, UsbDev *> *)m->map_pointer)->containsKey((QH *)key))
    return ((Util::Map<QH *, UsbDev *> *)m->map_pointer)->get((QH *)key);
  return (void *)0;
}

int contains_c_QH_Device_Map(struct SuperMap *m, void *key) {
  return (int)((Util::Map<QH *, UsbDev *> *)m->map_pointer)
      ->containsKey((QH *)key);
}

void put_c_QH_Device_Map(struct SuperMap *m, void *key, void *value) {
  ((Util::Map<QH *, UsbDev *> *)m->map_pointer)
      ->put((QH *)key, (UsbDev *)value);
}

void *remove_c_QH_Device_Map(struct SuperMap *m, void *key) {
  return ((Util::Map<QH *, UsbDev *> *)m->map_pointer)->remove((QH *)key);
}

void newEventMap(struct EventMap *map, const char *map_description) {
  map->super.get_c = &get_c_event_map;
  map->super.contains_c = &contains_c_event_map;
  map->super.put_c = &put_c_event_map;
  map->super.remove_c = &remove_c_event_map;
  map->super.new_super_map = &new_super_map;

  void *map_pointer = (Map_C) new Util::HashMap<int, EventListener *>();
  map->super.new_super_map(&map->super, map_description, map_pointer);
}

void *get_c_event_map(struct SuperMap *m, void *key) {
  if (((Util::Map<int, EventListener *> *)m->map_pointer)
          ->containsKey(*((int *)key)))
    return ((Util::Map<int, EventListener *> *)m->map_pointer)
        ->get(*((int *)key));
  return (void *)0;
}

int contains_c_event_map(struct SuperMap *m, void *key) {
  return (int)((Util::Map<int, EventListener *> *)m->map_pointer)
      ->containsKey(*((int *)key));
}

void put_c_event_map(struct SuperMap *m, void *key, void *value) {
  ((Util::Map<int, EventListener *> *)m->map_pointer)
      ->put(*((int *)key), (EventListener *)value);
}

void *remove_c_event_map(struct SuperMap *m, void *key) {
  return ((Util::Map<int, EventListener *> *)m->map_pointer)
      ->remove(*((int *)key));
}

void newInterface_Device_Map(struct Interface_Device_Map *map,
                             const char *map_description) {
  map->super.get_c = &get_c_interface_device_map;
  map->super.contains_c = &contains_c_interface_device_map;
  map->super.put_c = &put_c_interface_device_map;
  map->super.remove_c = &remove_c_interface_device_map;
  map->super.new_super_map = &new_super_map;

  void *map_pointer = (Map_C) new Util::HashMap<Interface *, UsbDev *>();
  map->super.new_super_map(&map->super, map_description, map_pointer);
}

void *get_c_interface_device_map(struct SuperMap *m, void *key) {
  if (((Util::Map<Interface *, UsbDev *> *)m->map_pointer)
          ->containsKey((Interface *)key)) {
    return ((Util::Map<Interface *, UsbDev *> *)m->map_pointer)
        ->get((Interface *)key);
  }
  return (void *)0;
}

int contains_c_interface_device_map(struct SuperMap *m, void *key) {
  return (int)((Util::Map<Interface *, UsbDev *> *)m->map_pointer)
      ->containsKey((Interface *)key);
}

void put_c_interface_device_map(struct SuperMap *m, void *key, void *value) {
  ((Util::Map<Interface *, UsbDev *> *)m->map_pointer)
      ->put((Interface *)key, (UsbDev *)value);
}

void *remove_c_interface_device_map(struct SuperMap *m, void *key) {
  return ((Util::Map<Interface *, UsbDev *> *)m->map_pointer)
      ->remove((Interface *)key);
}

void newAddressMap(struct Address_Map *map, const char *map_description) {
  map->super.get_c = &get_c_address_map;
  map->super.contains_c = &contains_c_address_map;
  map->super.put_c = &put_c_address_map;
  map->super.remove_c = &remove_c_address_map;
  map->super.new_super_map = &new_super_map;

  void *map_pointer = (Map_C) new Util::HashMap<uint32_t, QH *>();
  map->super.new_super_map(&map->super, map_description, map_pointer);
}

void *get_c_address_map(struct SuperMap *m, void *key) {
  uint32_t phy_address = *((uint32_t *)key);
  if (((Util::Map<uint32_t, QH *> *)m->map_pointer)->containsKey(phy_address))
    return ((Util::Map<uint32_t, QH *> *)m->map_pointer)->get(phy_address);
  return (void *)0;
}

int contains_c_address_map(struct SuperMap *m, void *key) {
  uint32_t phy_address = *((uint32_t *)key);
  return (int)((Util::Map<uint32_t, QH *> *)m->map_pointer)
      ->containsKey(phy_address);
}

void put_c_address_map(struct SuperMap *m, void *key, void *value) {
  uint32_t phy_address = *((uint32_t *)key);
  ((Util::Map<uint32_t, QH *> *)m->map_pointer)->put(phy_address, (QH *)value);
}

void *remove_c_address_map(struct SuperMap *m, void *key) {
  uint32_t phy_address = *((uint32_t *)key);
  return ((Util::Map<uint32_t, QH *> *)m->map_pointer)->remove(phy_address);
}

// <uint32_t,TD*>
void newAddressTDMap(struct Address_TD_Map *map, const char *map_description) {
  map->super.get_c = &get_c_address_td_map;
  map->super.contains_c = &contains_c_address_td_map;
  map->super.put_c = &put_c_address_td_map;
  map->super.remove_c = &remove_c_address_td_map;
  map->super.new_super_map = &new_super_map;

  void *map_pointer = (Map_C) new Util::HashMap<uint32_t, TD *>();
  map->super.new_super_map(&map->super, map_description, map_pointer);
}

void *get_c_address_td_map(struct SuperMap *m, void *key) {
  uint32_t phy_address = *((uint32_t *)key);
  if (((Util::Map<uint32_t, TD *> *)m->map_pointer)->containsKey(phy_address))
    return ((Util::Map<uint32_t, TD *> *)m->map_pointer)->get(phy_address);
  return (void *)0;
}

int contains_c_address_td_map(struct SuperMap *m, void *key) {
  uint32_t phy_address = *((uint32_t *)key);
  return (int)((Util::Map<uint32_t, TD *> *)m->map_pointer)
      ->containsKey(phy_address);
}

void put_c_address_td_map(struct SuperMap *m, void *key, void *value) {
  uint32_t phy_address = *((uint32_t *)key);
  ((Util::Map<uint32_t, TD *> *)m->map_pointer)->put(phy_address, (TD *)value);
}

void *remove_c_address_td_map(struct SuperMap *m, void *key) {
  uint32_t phy_address = *((uint32_t *)key);
  return ((Util::Map<uint32_t, TD *> *)m->map_pointer)->remove(phy_address);
}

void newQH_DeviceRequest_Map(struct QH_Device_Request_Map *map,
                             const char *map_description) {
  map->super.get_c = &get_c_qh_device_request;
  map->super.contains_c = &contains_c_qh_device_request;
  map->super.put_c = &put_c_qh_device_request;
  map->super.remove_c = &remove_c_qh_device_request;
  map->super.new_super_map = &new_super_map;

  void *map_pointer = (Map_C) new Util::HashMap<QH *, UsbDeviceRequest *>();
  map->super.new_super_map(&map->super, map_description, map_pointer);
}

void *get_c_qh_device_request(struct SuperMap *m, void *key) {
  if (((Util::Map<QH *, UsbDeviceRequest *> *)m->map_pointer)
          ->containsKey((QH *)key))
    return ((Util::Map<QH *, UsbDeviceRequest *> *)m->map_pointer)
        ->get((QH *)key);
  return (void *)0;
}

int contains_c_qh_device_request(struct SuperMap *m, void *key) {
  return (int)((Util::Map<QH *, UsbDeviceRequest *> *)m->map_pointer)
      ->containsKey((QH *)key);
}

void put_c_qh_device_request(struct SuperMap *m, void *key, void *value) {
  ((Util::Map<QH *, UsbDeviceRequest *> *)m->map_pointer)
      ->put((QH *)key, (UsbDeviceRequest *)value);
}

void *remove_c_qh_device_request(struct SuperMap *m, void *key) {
  return ((Util::Map<QH *, UsbDeviceRequest *> *)m->map_pointer)
      ->remove((QH *)key);
}

void newQH_Measuremnt_Map(struct QH_Measurement_Map *map,
                             const char *map_description) {
  map->super.get_c = &get_c_qh_measurement;
  map->super.contains_c = &contains_c_qh_measurement;
  map->super.put_c = &put_c_qh_measurement;
  map->super.remove_c = &remove_c_qh_measurement;
  map->super.new_super_map = &new_super_map;

  void *map_pointer = (Map_C) new Util::HashMap<QH *, uint32_t*>();
  map->super.new_super_map(&map->super, map_description, map_pointer);
}

void *get_c_qh_measurement(struct SuperMap *m, void *key) {
  if (((Util::Map<QH *, uint32_t*> *)m->map_pointer)
          ->containsKey((QH *)key))
    return ((Util::Map<QH *, uint32_t *> *)m->map_pointer)
        ->get((QH *)key);
  return (void *)0;
}

int contains_c_qh_measurement(struct SuperMap *m, void *key) {
  return (int)((Util::Map<QH *, uint32_t *> *)m->map_pointer)
      ->containsKey((QH *)key);
}

void put_c_qh_measurement(struct SuperMap *m, void *key, void *value) {
  ((Util::Map<QH *, uint32_t *> *)m->map_pointer)
      ->put((QH *)key, (uint32_t *)value);
}

void *remove_c_qh_measurement(struct SuperMap *m, void *key) {
  return ((Util::Map<QH *, uint32_t *> *)m->map_pointer)
      ->remove((QH *)key);
}

void newCommandBlockIntMap(CommandBlockWrapper_Int_Map *map,
                             const char *map_description) {
  map->super.get_c = &get_command_block_int;
  map->super.contains_c = &contains_command_block_int;
  map->super.put_c = &put_command_block_int;
  map->super.remove_c = &remove_command_block_int;
  map->super.new_super_map = &new_super_map;

  void *map_pointer = (Map_C) new Util::HashMap<CommandBlockWrapper *, uint32_t*>();
  map->super.new_super_map(&map->super, map_description, map_pointer);
}

void *get_command_block_int(struct SuperMap *m, void *key) {
  if (((Util::Map<CommandBlockWrapper *, uint32_t*> *)m->map_pointer)
          ->containsKey((CommandBlockWrapper *)key))
    return ((Util::Map<CommandBlockWrapper *, uint32_t*> *)m->map_pointer)
        ->get((CommandBlockWrapper *)key);
  return (void *)0;
}

int contains_command_block_int(struct SuperMap *m, void *key) {
  return (int)((Util::Map<CommandBlockWrapper *, uint32_t *> *)m->map_pointer)
      ->containsKey((CommandBlockWrapper*)key);
}

void put_command_block_int(struct SuperMap *m, void *key, void *value) {
  ((Util::Map<CommandBlockWrapper *, uint32_t *> *)m->map_pointer)
      ->put((CommandBlockWrapper *)key, (uint32_t *)value);
}

void *remove_command_block_int(struct SuperMap *m, void *key) {
  return ((Util::Map<CommandBlockWrapper *, uint32_t *> *)m->map_pointer)
      ->remove((CommandBlockWrapper *)key);
}

void newCommandStatusIntMap(CommandStatusWrapper_Int_Map *map,
                             const char *map_description) {
  map->super.get_c = &get_command_status_int;
  map->super.contains_c = &contains_command_status_int;
  map->super.put_c = &put_command_status_int;
  map->super.remove_c = &remove_command_status_int;
  map->super.new_super_map = &new_super_map;

  void *map_pointer = (Map_C) new Util::HashMap<CommandStatusWrapper *, uint32_t*>();
  map->super.new_super_map(&map->super, map_description, map_pointer);
}

void *get_command_status_int(struct SuperMap *m, void *key) {
  if (((Util::Map<CommandStatusWrapper *, uint32_t*> *)m->map_pointer)
          ->containsKey((CommandStatusWrapper *)key))
    return ((Util::Map<CommandStatusWrapper *, uint32_t*> *)m->map_pointer)
        ->get((CommandStatusWrapper *)key);
  return (void *)0;
}

int contains_command_status_int(struct SuperMap *m, void *key) {
  return (int)((Util::Map<CommandStatusWrapper *, uint32_t *> *)m->map_pointer)
      ->containsKey((CommandStatusWrapper*)key);
}

void put_command_status_int(struct SuperMap *m, void *key, void *value) {
  ((Util::Map<CommandStatusWrapper *, uint32_t *> *)m->map_pointer)
      ->put((CommandStatusWrapper *)key, (uint32_t *)value);
}

void *remove_command_status_int(struct SuperMap *m, void *key) {
  return ((Util::Map<CommandStatusWrapper *, uint32_t *> *)m->map_pointer)
      ->remove((CommandStatusWrapper *)key);
}

void newDataIntMap(Data_Int_Map *map,
                             const char *map_description) {
  map->super.get_c = &get_data_int;
  map->super.contains_c = &contains_data_int;
  map->super.put_c = &put_data_int;
  map->super.remove_c = &remove_data_int;
  map->super.new_super_map = &new_super_map;

  void *map_pointer = (Map_C) new Util::HashMap<uint8_t *, uint32_t*>();
  map->super.new_super_map(&map->super, map_description, map_pointer);
}

void *get_data_int(struct SuperMap *m, void *key) {
  if (((Util::Map<uint8_t *, uint32_t*> *)m->map_pointer)
          ->containsKey((uint8_t *)key))
    return ((Util::Map<uint8_t *, uint32_t*> *)m->map_pointer)
        ->get((uint8_t *)key);
  return (void *)0;
}

int contains_data_int(struct SuperMap *m, void *key) {
  return (int)((Util::Map<uint8_t *, uint32_t *> *)m->map_pointer)
      ->containsKey((uint8_t*)key);
}

void put_data_int(struct SuperMap *m, void *key, void *value) {
  ((Util::Map<uint8_t *, uint32_t *> *)m->map_pointer)
      ->put((uint8_t *)key, (uint32_t *)value);
}

void *remove_data_int(struct SuperMap *m, void *key) {
  return ((Util::Map<uint8_t *, uint32_t *> *)m->map_pointer)
      ->remove((uint8_t *)key);
}

void newIntCallbackMap(Int_Callback_Map *map,
                             const char *map_description) {
  map->super.get_c = &get_int_callback;
  map->super.contains_c = &contains_int_callback;
  map->super.put_c = &put_int_callback;
  map->super.remove_c = &remove_int_callback;
  map->super.new_super_map = &new_super_map;

  void *map_pointer = (Map_C) new Util::HashMap<uint32_t, msd_callback*>();
  map->super.new_super_map(&map->super, map_description, map_pointer);
}

void *get_int_callback(struct SuperMap *m, void *key) {
  if (((Util::Map<uint32_t, msd_callback> *)m->map_pointer)
          ->containsKey(*((uint32_t *)key)))
    return ((Util::Map<uint32_t, msd_callback*> *)m->map_pointer)
        ->get(*((uint32_t *)key));
  return (void *)0;
}

int contains_int_callback(struct SuperMap *m, void *key) {
  return (int)((Util::Map<uint32_t, msd_callback*> *)m->map_pointer)
      ->containsKey(*((uint32_t*)key));
}

void put_int_callback(struct SuperMap *m, void *key, void *value) {
  ((Util::Map<uint32_t, msd_callback*> *)m->map_pointer)
      ->put(*((uint32_t*)key), (msd_callback*)value);
}

void *remove_int_callback(struct SuperMap *m, void *key) {
  return ((Util::Map<uint32_t, msd_callback*> *)m->map_pointer)
      ->remove(*((uint32_t*)key));
}

void newIntBufferMap(Int_Buffer_Map *map,
                             const char *map_description) {
  map->super.get_c = &get_command_block_int;
  map->super.contains_c = &contains_command_block_int;
  map->super.put_c = &put_command_block_int;
  map->super.remove_c = &remove_command_block_int;
  map->super.new_super_map = &new_super_map;

  void *map_pointer = (Map_C) new Util::HashMap<uint32_t, uint8_t*>();
  map->super.new_super_map(&map->super, map_description, map_pointer);
}

void *get_int_buffer(struct SuperMap *m, void *key) {
  if (((Util::Map<uint32_t, uint8_t*> *)m->map_pointer)
          ->containsKey(*((uint32_t *)key)))
    return ((Util::Map<uint32_t, uint8_t*> *)m->map_pointer)
        ->get(*((uint32_t *)key));
  return (void *)0;
}

int contains_int_buffer(struct SuperMap *m, void *key) {
  return (int)((Util::Map<uint32_t, uint8_t *> *)m->map_pointer)
      ->containsKey(*((uint32_t*)key));
}

void put_int_buffer(struct SuperMap *m, void *key, void *value) {
  ((Util::Map<uint32_t, uint8_t *> *)m->map_pointer)
      ->put(*((uint32_t *)key), (uint8_t *)value);
}

void *remove_int_buffer(struct SuperMap *m, void *key) {
  return ((Util::Map<uint32_t, uint8_t *> *)m->map_pointer)
      ->remove(*((uint32_t *)key));
}

void newIntTLenMap(Int_T_Len_Map *map,
                             const char *map_description) {
  map->super.get_c = &get_int_t_len;
  map->super.contains_c = &contains_int_t_len;
  map->super.put_c = &put_int_t_len;
  map->super.remove_c = &remove_int_t_len;
  map->super.new_super_map = &new_super_map;

  void *map_pointer = (Map_C) new Util::HashMap<uint32_t, uint32_t*>();
  map->super.new_super_map(&map->super, map_description, map_pointer);
}

void *get_int_t_len(struct SuperMap *m, void *key) {
  if (((Util::Map<uint32_t, uint32_t*> *)m->map_pointer)
          ->containsKey(*((uint32_t *)key)))
    return ((Util::Map<uint32_t, uint32_t*> *)m->map_pointer)
        ->get(*((uint32_t *)key));
  return (void *)0;
}

int contains_int_t_len(struct SuperMap *m, void *key) {
  return (int)((Util::Map<uint32_t, uint32_t *> *)m->map_pointer)
      ->containsKey(*((uint32_t*)key));
}

void put_int_t_len(struct SuperMap *m, void *key, void *value) {
  ((Util::Map<uint32_t, uint32_t *> *)m->map_pointer)
      ->put(*((uint32_t *)key), (uint32_t *)value);
}

void *remove_int_t_len(struct SuperMap *m, void *key) {
  return ((Util::Map<uint32_t, uint32_t *> *)m->map_pointer)
      ->remove(*((uint32_t *)key));
}

void newIntMemBufferMap(Int_Mem_Buffer_Map *map,
                             const char *map_description) {
  map->super.get_c = &get_int_t_len;
  map->super.contains_c = &contains_int_t_len;
  map->super.put_c = &put_int_t_len;
  map->super.remove_c = &remove_int_t_len;
  map->super.new_super_map = &new_super_map;

  void *map_pointer = (Map_C) new Util::HashMap<uint32_t, uint8_t*>();
  map->super.new_super_map(&map->super, map_description, map_pointer);
}

void *get_int_mem_buffer(struct SuperMap *m, void *key) {
  if (((Util::Map<uint32_t, uint8_t*> *)m->map_pointer)
          ->containsKey(*((uint32_t *)key)))
    return ((Util::Map<uint32_t, uint8_t*> *)m->map_pointer)
        ->get(*((uint32_t *)key));
  return (void *)0;
}

int contains_int_mem_buffer(struct SuperMap *m, void *key) {
  return (int)((Util::Map<uint32_t, uint8_t *> *)m->map_pointer)
      ->containsKey(*((uint32_t*)key));
}

void put_int_mem_buffer(struct SuperMap *m, void *key, void *value) {
  ((Util::Map<uint32_t, uint8_t *> *)m->map_pointer)
      ->put(*((uint32_t *)key), (uint8_t *)value);
}

void *remove_int_mem_buffer(struct SuperMap *m, void *key) {
  return ((Util::Map<uint32_t, uint8_t *> *)m->map_pointer)
      ->remove(*((uint32_t *)key));
}