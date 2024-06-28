#ifndef MapInterface__include
#define MapInterface__include

#include "../utility/Utils.h"

#define __MAP_GET__(map, type, val) \
  (type)__STRUCT_CALL__(map, get_c, val)

#define __MAP_PUT__(map, key, val) \
  __STRUCT_CALL__(map, put_c, key, val)

#define __MAP_REMOVE__(map, type, key) \
  (type)__STRUCT_CALL__(map, remove_c, key)

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

#define __DECLARE_MAP__(type) \
  typedef struct type { \
    struct SuperMap super; \
    void (*new_map)(struct type* m, const char* map_description); \
  } type;

#define __MAP_PARAMETER_LIST__ \
  struct SuperMap* m, void* key

#define __MAP_PARAMETER_LIST_EXT__ \
  __MAP_PARAMETER_LIST__ , void* value

#define __MAP_PROTO__(proto, parameter_list) \
  proto(parameter_list)

#define __DEFINE_GET__(name) \
  __MAP_PROTO__(static void* get_c_ ## name, __MAP_PARAMETER_LIST__)

#define __DEFINE_CONTAINS__(name) \
  __MAP_PROTO__(static int contains_c_ ## name, __MAP_PARAMETER_LIST__)

#define __DEFINE_PUT__(name) \
  __MAP_PROTO__(static void put_c_ ## name, __MAP_PARAMETER_LIST_EXT__)

#define __DEFINE_REMOVE__(name) \
  __MAP_PROTO__(static void* remove_c_ ## name, __MAP_PARAMETER_LIST__)

#define __DEFINE_MAP_FUNCTIONS__(name) \
  __DEFINE_GET__(name); \
  __DEFINE_CONTAINS__(name); \
  __DEFINE_PUT__(name); \
  __DEFINE_REMOVE__(name)

#define __DEFINE_MAP_NEW__(name, type) \
  void name(struct type* map, const char* description)

#define __MAP_GET_POINTER__(name, key_t, value_t) \
  __DEFINE_GET__(name) {  \
    if (((Util::Map<key_t *, value_t *> *)m->map_pointer)->containsKey((key_t *)key)) { \
      return ((Util::Map<key_t *, value_t *> *)m->map_pointer)->get((key_t *)key); \
    } \
    return (void *)0; \
  }

#define __MAP_GET_FUNC_PTR__(name, key_t, value_t) \
  __DEFINE_GET__(name) { \
    if (((Util::Map<key_t *, value_t> *)m->map_pointer)->containsKey((key_t *)key)) { \
      return (void*)(((Util::Map<key_t *, value_t> *)m->map_pointer)->get((key_t *)key)); \
    } \
    return (void *)0; \
  }
  
#define __MAP_GET_VALUE__(name, key_t, value_t) \
  __DEFINE_GET__(name) { \
    if (((Util::Map<key_t, value_t *> *)m->map_pointer) \
          ->containsKey(*((key_t *)key))) \
    return (((Util::Map<key_t, value_t *> *)m->map_pointer) \
                ->get(*((key_t *)key))); \
    return (void *)0; \
  }

#define __MAP_CONTAINS_POINTER__(name, key_t, value_t) \
  __DEFINE_CONTAINS__(name) { \
    return ((Util::Map<key_t *, value_t *> *)m->map_pointer)->containsKey((key_t *)key); \
  }

#define __MAP_CONTAINS_FUNC_PTR__(name, key_t, value_t) \
  __DEFINE_CONTAINS__(name) { \
    return ((Util::Map<key_t *, value_t> *)m->map_pointer)->containsKey((key_t *)key); \
  }

#define __MAP_CONTAINS_VALUE__(name, key_t, value_t) \
  __DEFINE_CONTAINS__(name) { \
    return ((Util::Map<key_t, value_t *> *)m->map_pointer) \
      ->containsKey(*((key_t *)key)); \
  }

#define __MAP_PUT_POINTER__(name, key_t, value_t) \
  __DEFINE_PUT__(name){ \
    ((Util::Map<key_t *, value_t *> *)m->map_pointer)->put((key_t *)key, (value_t *)value); \
  }

#define __MAP_PUT_FUNC_PTR__(name, key_t, value_t) \
  __DEFINE_PUT__(name){ \
    ((Util::Map<key_t *, value_t> *)m->map_pointer)->put((key_t *)key, (value_t)value); \
  }

#define __MAP_PUT_VALUE__(name, key_t, value_t) \
  __DEFINE_PUT__(name){ \
    ((Util::Map<key_t, value_t *> *)m->map_pointer) \
      ->put(*((key_t *)key), (value_t *)value); \
  }

#define __MAP_REMOVE_POINTER__(name, key_t, value_t) \
  __DEFINE_REMOVE__(name){ \
    return ((Util::Map<key_t *, value_t *> *)m->map_pointer)->remove((key_t *)key); \
  }

#define __MAP_REMOVE_FUNC_PTR__(name, key_t, value_t) \
  __DEFINE_REMOVE__(name){ \
    return (void*)(((Util::Map<key_t *, value_t> *)m->map_pointer)->remove((key_t *)key)); \
  }

#define __MAP_REMOVE_VALUE__(name, key_t, value_t) \
  __DEFINE_REMOVE__(name){ \
    return ((Util::Map<key_t, value_t *> *)m->map_pointer) \
      ->remove(*((key_t *)key)); \
  }

#define __MAP_SUPER_ROUTINE__(name, key_t, value_t) \
  __SUPER__(map, get_c) = __CONCAT__(&get_c_, name); \
  __SUPER__(map, contains_c) = __CONCAT__(&contains_c_, name); \
  __SUPER__(map, put_c) = __CONCAT__(&put_c_, name); \
  __SUPER__(map, remove_c) = __CONCAT__(&remove_c_, name); \
  __SUPER__(map, new_super_map) = &new_super_map; \
  \
  void *map_pointer = (Map_C) new Util::HashMap<key_t, value_t>(); \
  __CALL_SUPER__(map->super, new_super_map, description, map_pointer)

#define __MAP_NEW_PTR_ROUTINE__(func_name, name, type, key_t, value_t) \
  __DEFINE_MAP_NEW__(func_name, type) { \
    __MAP_SUPER_ROUTINE__(name, key_t*, value_t*); \
  }

#define __MAP_NEW_VALUE_ROUTINE__(func_name, name, type, key_t, value_t) \
  __DEFINE_MAP_NEW__(func_name, type) { \
    __MAP_SUPER_ROUTINE__(name, key_t, value_t*); \
  }

#define __MAP_NEW_FUNC_ROUTINE__(func_name, name, type, key_t, value_t) \
  __DEFINE_MAP_NEW__(func_name, type) { \
    __MAP_SUPER_ROUTINE__(name, key_t*, value_t); \
  }

#define __MAP_INIT_PTR__(func_name, type, name, key_t, value_t) \
  __MAP_NEW_PTR_ROUTINE__(func_name, name, type, key_t, value_t); \
  __MAP_GET_POINTER__(name, key_t, value_t); \
  __MAP_CONTAINS_POINTER__(name, key_t, value_t); \
  __MAP_PUT_POINTER__(name, key_t, value_t); \
  __MAP_REMOVE_POINTER__(name, key_t, value_t)

#define __MAP_INIT_VALUE__(func_name, type, name, key_t, value_t) \
  __MAP_NEW_VALUE_ROUTINE__(func_name, name, type, key_t, value_t); \
  __MAP_GET_VALUE__(name, key_t, value_t); \
  __MAP_CONTAINS_VALUE__(name, key_t, value_t); \
  __MAP_PUT_VALUE__(name, key_t, value_t); \
  __MAP_REMOVE_VALUE__(name, key_t, value_t)

#define __MAP_INIT_FUNC_PTR__(func_name, type, name, key_t, value_t) \
  __MAP_NEW_FUNC_ROUTINE__(func_name, name, type, key_t, value_t); \
  __MAP_GET_FUNC_PTR__(name, key_t, value_t); \
  __MAP_CONTAINS_FUNC_PTR__(name, key_t, value_t); \
  __MAP_PUT_FUNC_PTR__(name, key_t, value_t); \
  __MAP_REMOVE_FUNC_PTR__(name, key_t, value_t)

// each new map needs to be struct accord
// recycle TD when interrupt transfer instead of creating new ones
__DECLARE_MAP__(QH_TD_Map);

// after completion or if there are errors while transmitting this callback
// function associated with the QH is called -> callback to driver
__DECLARE_MAP__(QH_Callback_Function_Map);

//counts retransmissions for a certain QH
__DECLARE_MAP__(QH_Status_Map);

__DECLARE_MAP__(QH_Data_Map);
__DECLARE_MAP__(QH_Device_Map);
__DECLARE_MAP__(Register_Map);
__DECLARE_MAP__(EventMap);
__DECLARE_MAP__(Interface_Device_Map);
__DECLARE_MAP__(Address_Map);
__DECLARE_MAP__(Address_TD_Map);
__DECLARE_MAP__(QH_Device_Request_Map);
__DECLARE_MAP__(QH_Measurement_Map);
__DECLARE_MAP__(CommandBlockWrapper_Int_Map);
__DECLARE_MAP__(CommandStatusWrapper_Int_Map);
__DECLARE_MAP__(Data_Int_Map);
__DECLARE_MAP__(Int_Callback_Map);
__DECLARE_MAP__(Int_Buffer_Map);
__DECLARE_MAP__(Int_T_Len_Map);
__DECLARE_MAP__(Int_Mem_Buffer_Map);
__DECLARE_MAP__(QH_Interface_Map);
__DECLARE_MAP__(Interface_Buffer_Map);
__DECLARE_MAP__(Interface_Write_Callback_Map);

#ifdef __cplusplus

extern "C" {
#endif

__DEFINE_MAP_NEW__(newQH_StatusMap, QH_Status_Map);
__DEFINE_MAP_NEW__(newQH_CallbackMap, QH_Callback_Function_Map);
__DEFINE_MAP_NEW__(newRegisterMap, Register_Map);
__DEFINE_MAP_NEW__(newQH_TD, QH_TD_Map);
__DEFINE_MAP_NEW__(newQH_Device_Map, QH_Device_Map);
__DEFINE_MAP_NEW__(newQH_Data_Map, QH_Data_Map);
__DEFINE_MAP_NEW__(newEventMap, EventMap);
__DEFINE_MAP_NEW__(newInterface_Device_Map, Interface_Device_Map);
__DEFINE_MAP_NEW__(newAddressMap, Address_Map);
__DEFINE_MAP_NEW__(newAddressTDMap, Address_TD_Map);
__DEFINE_MAP_NEW__(newQH_DeviceRequest_Map, QH_Device_Request_Map);
__DEFINE_MAP_NEW__(newQH_Measuremnt_Map, QH_Measurement_Map);
__DEFINE_MAP_NEW__(newCommandBlockIntMap, CommandBlockWrapper_Int_Map);
__DEFINE_MAP_NEW__(newCommandStatusIntMap, CommandStatusWrapper_Int_Map);
__DEFINE_MAP_NEW__(newDataIntMap, Data_Int_Map);
__DEFINE_MAP_NEW__(newIntCallbackMap, Int_Callback_Map);
__DEFINE_MAP_NEW__(newIntBufferMap, Int_Buffer_Map);
__DEFINE_MAP_NEW__(newIntTLenMap, Int_T_Len_Map);
__DEFINE_MAP_NEW__(newIntMemBufferMap, Int_Mem_Buffer_Map);
__DEFINE_MAP_NEW__(newQH_Interface_Map, QH_Interface_Map);
__DEFINE_MAP_NEW__(newInterface_Buffer_Map, Interface_Buffer_Map);
__DEFINE_MAP_NEW__(newInterface_Write_Callback_Map, Interface_Write_Callback_Map);

#ifdef __cplusplus
}
#endif

#endif