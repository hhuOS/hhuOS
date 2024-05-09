#ifndef MapInterface__include
#define MapInterface__include

#define __MAP_GET__(map, type, val) \
  (type)__STRUCT_CALL__(map, get_c, val)

typedef void *Map_C;

struct SuperMap {
  void *(*get_c)(struct SuperMap *s, void *b);
  int (*contains_c)(struct SuperMap *s, void *b);
  void (*put_c)(struct SuperMap *s, void *a, void *b);
  void *(*remove_c)(struct SuperMap *s, void *b);
  void (*new_super_map)(struct SuperMap *s, const char *map_descript,
                        Map_C pointer);

  Map_C map_pointer;
  const char *map_description; // Map<?,?>
};

typedef struct SuperMap SuperMap;

// each new map needs to be struct accord
// recycle TD when interrupt transfer instead of creating new ones
struct QH_TD_Map { // Map<QH,TD*>
  struct SuperMap super;

  void (*new_map)(struct QH_TD_Map *m, const char *map_description);
};

// after completion or if there are errors while transmitting this callback
// function associated with the QH is called -> callback to driver
struct QH_Callback_Function_Map { // Map<QH, void (*callback)(int status, void*
                                  // data)>
  struct SuperMap super;

  void (*new_map)(struct QH_Callback_Function_Map *m,
                  const char *map_description);
};

struct QH_Status_Map { // Map<QH,int> counts retransmissions for a certain QH
  struct SuperMap super;

  void (*new_map)(struct QH_Status_Map *m, const char *map_description);
};

struct QH_Data_Map { // Map<QH,void*>
  struct SuperMap super;

  void (*new_map)(struct QH_Data_Map *m, const char *map_description);
};

struct QH_Device_Map { // Map<QH,Device>
  struct SuperMap super;

  void (*new_map)(struct QH_Device_Map *m, const char *map_description);
};

struct Register_Map { // Map<RegisterType,Register>
  struct SuperMap super;

  void (*new_map)(struct Register_Map *m, const char *map_description);
};

struct EventMap {
  struct SuperMap super;

  void (*new_map)(struct EventMap *m, const char *map_description);
};

struct Interface_Device_Map { // Map<Interface*,UsbDev*>
  struct SuperMap super;

  void (*new_map)(struct Interface_Device_Map *m, const char *map_description);
};

struct Address_Map {
  struct SuperMap super;

  void (*new_map)(struct Address_Map *m, const char *map_description);
};

struct Address_TD_Map {
  struct SuperMap super;

  void (*new_map)(struct Address_TD_Map *m, const char *map_description);
};

struct QH_Device_Request_Map {
  struct SuperMap super;

  void (*new_map)(struct QH_Device_Request_Map *m, const char *map_description);
};

struct QH_Measurement_Map{
  struct SuperMap super;

  void (*new_map)(struct QH_Measurement_Map* m, const char* map_description);
};

struct CommandBlockWrapper_Int_Map{
  struct SuperMap super;

  void (*new_map)(struct CommandBlockWrapper_Int_Map* m, const char* map_description);
};

struct CommandStatusWrapper_Int_Map{
  struct SuperMap super;

  void (*new_map)(struct CommandStatusWrapper_Int_Map* m, const char* map_description);
};

struct Data_Int_Map{
  struct SuperMap super;

  void (*new_map)(struct Data_Int_Map* m, const char* map_description);
};

struct Int_Callback_Map{
  struct SuperMap super;

  void (*new_map)(struct Int_Callback_Map* m, const char* map_description);
};

struct Int_Buffer_Map{
  struct SuperMap super;

  void (*new_map)(struct Int_Buffer_Map* m, const char* map_description);
};

struct Int_T_Len_Map{
  struct SuperMap super;

  void (*new_map)(struct Int_T_Len_Map* m, const char* map_description);
};

struct Int_Mem_Buffer_Map{
  struct SuperMap super;

  void (*new_map)(struct Int_Mem_Buffer_Map* m, const char* map_description);
};

typedef struct QH_Callback_Function_Map QH_Callback_Function_Map;
typedef struct QH_TD_Map QH_TD_Map;
typedef struct Register_Map Register_Map;
typedef struct QH_Status_Map QH_Status_Map;
typedef struct QH_Data_Map QH_Data_Map;
typedef struct QH_Device_Map QH_Device_Map;
typedef struct EventMap EventMap;
typedef struct Interface_Device_Map Interface_Device_Map;
typedef struct Address_Map Address_Map;
typedef struct Address_TD_Map Address_TD_Map;
typedef struct QH_Device_Request_Map QH_Device_Request_Map;
typedef struct QH_Measurement_Map QH_Measurement_Map;
typedef struct CommandBlockWrapper_Int_Map CommandBlockWrapper_Int_Map;
typedef struct CommandStatusWrapper_Int_Map CommandStatusWrapper_Int_Map;
typedef struct Data_Int_Map Data_Int_Map;
typedef struct Int_Callback_Map Int_Callback_Map;
typedef struct Int_Buffer_Map Int_Buffer_Map;
typedef struct Int_T_Len_Map Int_T_Len_Map;
typedef struct Int_Mem_Buffer_Map Int_Mem_Buffer_Map;

#ifdef __cplusplus
extern "C" {
#endif

void *remove_c_QH_Status(struct SuperMap *m, void *key);
void put_c_QH_Status(struct SuperMap *m, void *key, void *value);
int contains_c_QH_Status(struct SuperMap *m, void *key);
void *get_c_QH_Status(struct SuperMap *m, void *key);
void newQH_StatusMap(struct QH_Status_Map *map, const char *map_description);

void *remove_c_QH_Callback(struct SuperMap *m, void *key);
void put_c_QH_Callback(struct SuperMap *m, void *key, void *value);
int contains_c_QH_Callback(struct SuperMap *m, void *key);
void *get_c_QH_Callback(struct SuperMap *m, void *key);
void newQH_CallbackMap(struct QH_Callback_Function_Map *map,
                       const char *map_description);

void *remove_c_Register(struct SuperMap *m, void *key);
void put_c_Register(struct SuperMap *m, void *key, void *value);
int contains_c_Register(struct SuperMap *m, void *key);
void *get_c_Register(struct SuperMap *m, void *key);
void newRegisterMap(struct Register_Map *map, const char *map_description);

void *remove_c_QH_TD(struct SuperMap *m, void *key);
void put_c_QH_TD(struct SuperMap *m, void *key, void *value);
int contains_c_QH_TD(struct SuperMap *m, void *key);
void *get_c_QH_TD(struct SuperMap *m, void *key);
void newQH_TD(struct QH_TD_Map *map, const char *map_description);

void *remove_c_QH_Device_Map(struct SuperMap *m, void *key);
void put_c_QH_Device_Map(struct SuperMap *m, void *key, void *value);
int contains_c_QH_Device_Map(struct SuperMap *m, void *key);
void *get_c_QH_Device_Map(struct SuperMap *m, void *key);
void newQH_Device_Map(struct QH_Device_Map *map, const char *map_description);

void *remove_c_QH_Data_Map(struct SuperMap *m, void *key);
void put_c_QH_Data_Map(struct SuperMap *m, void *key, void *value);
int contains_c_QH_Data_Map(struct SuperMap *m, void *key);
void *get_c_QH_Data_Map(struct SuperMap *m, void *key);
void newQH_Data_Map(struct QH_Data_Map *map, const char *map_description);

void *get_c_event_map(struct SuperMap *m, void *key);
int contains_c_event_map(struct SuperMap *m, void *key);
void put_c_event_map(struct SuperMap *m, void *key, void *value);
void *remove_c_event_map(struct SuperMap *m, void *key);
void newEventMap(struct EventMap *map, const char *map_description);

void *remove_c_interface_device_map(struct SuperMap *m, void *key);
void put_c_interface_device_map(struct SuperMap *m, void *key, void *value);
int contains_c_interface_device_map(struct SuperMap *m, void *key);
void *get_c_interface_device_map(struct SuperMap *m, void *key);
void newInterface_Device_Map(struct Interface_Device_Map *map,
                             const char *map_description);

void newAddressMap(struct Address_Map *map, const char *map_description);
void *get_c_address_map(struct SuperMap *m, void *key);
int contains_c_address_map(struct SuperMap *m, void *key);
void put_c_address_map(struct SuperMap *m, void *key, void *value);
void *remove_c_address_map(struct SuperMap *m, void *key);

void newAddressTDMap(struct Address_TD_Map *map, const char *map_description);
void *get_c_address_td_map(struct SuperMap *m, void *key);
int contains_c_address_td_map(struct SuperMap *m, void *key);
void put_c_address_td_map(struct SuperMap *m, void *key, void *value);
void *remove_c_address_td_map(struct SuperMap *m, void *key);

void newQH_DeviceRequest_Map(struct QH_Device_Request_Map *map,
                             const char *map_description);
void *get_c_qh_device_request(struct SuperMap *m, void *key);
int contains_c_qh_device_request(struct SuperMap *m, void *key);
void put_c_qh_device_request(struct SuperMap *m, void *key, void *value);
void *remove_c_qh_device_request(struct SuperMap *m, void *key);

void newQH_Measuremnt_Map(struct QH_Measurement_Map *map,
                             const char *map_description);
void *get_c_qh_measurement(struct SuperMap *m, void *key);
int contains_c_qh_measurement(struct SuperMap *m, void *key);
void put_c_qh_measurement(struct SuperMap *m, void *key, void *value);
void *remove_c_qh_measurement(struct SuperMap *m, void *key);

void newCommandBlockIntMap(CommandBlockWrapper_Int_Map *map,
                             const char *map_description);
void *get_command_block_int(struct SuperMap *m, void *key);
int contains_command_block_int(struct SuperMap *m, void *key);
void put_command_block_int(struct SuperMap *m, void *key, void *value);
void *remove_command_block_int(struct SuperMap *m, void *key);

void newCommandStatusIntMap(CommandStatusWrapper_Int_Map *map,
                             const char *map_description);
void *get_command_status_int(struct SuperMap *m, void *key);
int contains_command_status_int(struct SuperMap *m, void *key);
void put_command_status_int(struct SuperMap *m, void *key, void *value);
void *remove_command_status_int(struct SuperMap *m, void *key);

void newDataIntMap(Data_Int_Map *map,
                             const char *map_description);
void *get_data_int(struct SuperMap *m, void *key);
int contains_data_int(struct SuperMap *m, void *key);
void put_data_int(struct SuperMap *m, void *key, void *value);
void *remove_data_int(struct SuperMap *m, void *key);

void newIntCallbackMap(Int_Callback_Map *map,
                             const char *map_description);
void *get_int_callback(struct SuperMap *m, void *key);
int contains_int_callback(struct SuperMap *m, void *key);
void put_int_callback(struct SuperMap *m, void *key, void *value);
void *remove_int_callback(struct SuperMap *m, void *key);

void newIntBufferMap(Int_Buffer_Map *map,
                             const char *map_description);
void *get_int_buffer(struct SuperMap *m, void *key);
int contains_int_buffer(struct SuperMap *m, void *key);
void put_int_buffer(struct SuperMap *m, void *key, void *value);
void *remove_int_buffer(struct SuperMap *m, void *key);

void newIntTLenMap(Int_T_Len_Map *map,
                             const char *map_description);
void *get_int_t_len(struct SuperMap *m, void *key);
int contains_int_t_len(struct SuperMap *m, void *key);
void put_int_t_len(struct SuperMap *m, void *key, void *value);
void *remove_int_t_len(struct SuperMap *m, void *key);

void newIntMemBufferMap(Int_Mem_Buffer_Map *map,
                             const char *map_description);
void *get_int_mem_buffer(struct SuperMap *m, void *key);
int contains_int_mem_buffer(struct SuperMap *m, void *key);
void put_int_mem_buffer(struct SuperMap *m, void *key, void *value);
void *remove_int_mem_buffer(struct SuperMap *m, void *key);

#ifdef __cplusplus
}
#endif

#endif