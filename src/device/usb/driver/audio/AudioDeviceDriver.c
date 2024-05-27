#include "AudioDeviceDriver.h"
#include "../../controller/UsbControllerFlags.h"
#include "../../include/UsbErrors.h"

static uint8_t get_upstream_channels_by_source_id(AudioDriver* driver, 
    uint8_t bSourceID, ACInterface* ac_interface);
static uint8_t get_upstream_channels_by_source_id_depth(AudioDriver* driver,
    uint8_t bSourceID, ACInterface* ac_interface, uint8_t depth);
static void* find_structure_by_id(AudioDriver* driver, ACInterface* ac_interface,
    uint8_t bSourceID, enum ACInterfaceDescriptorSubtypes* subtype);
static void* get_upstream_structure(AudioDriver* driver, ACInterface* ac_interface, 
    enum ACInterfaceDescriptorSubtypes* subtype, void* unit, uint8_t bSource_num);
static int mixer_build_routine(AudioDriver* driver, uint8_t* start,
  MemoryService_C* m, ACInterface* ac_interface);
static int8_t is_bm_control_programmable(AudioDriver* driver, 
    struct MixerUnit* mixer_unit, uint8_t input_channel, 
    uint8_t output_channel, uint8_t output_channels, uint8_t input_channels);
static uint8_t mixer_bmControlLength(AudioDriver* driver,
    uint8_t input_channels, uint8_t output_channels);
static int selector_build_routine(AudioDriver* driver, uint8_t* start,
  ACInterface* ac_interface, MemoryService_C* m);
static int feature_build_routine(AudioDriver* driver, uint8_t* start,
  ACInterface* ac_interface, MemoryService_C* m);
static int up_down_mix_processing_unit_build_routine(AudioDriver* driver,
    uint8_t* start, MemoryService_C* m, 
    ProcessingUnitDescriptor* processing_unit_descriptor);
static int dolby_prologic_processing_unit_build_routine(AudioDriver* driver,
    MemoryService_C* m, uint8_t* start, ProcessingUnitDescriptor* processing_unit_descriptor);
static int stereo_extender_processing_unit_build_routine(AudioDriver* driver,
    MemoryService_C* m, uint8_t* start, ProcessingUnitDescriptor* processing_unit_descriptor);
static int reverberation_processing_unit_build_routine(AudioDriver* driver,
    MemoryService_C* m, uint8_t* start, ProcessingUnitDescriptor* processing_unit_descriptor);
static int chorus_processing_unit_build_routine(AudioDriver* driver, 
    MemoryService_C* m, uint8_t* start, ProcessingUnitDescriptor* processing_unit_descriptor);
static int dynamic_range_compressor_processing_unit_build_routine(AudioDriver* driver, 
    MemoryService_C* m, uint8_t* start, ProcessingUnitDescriptor* processing_unit_descriptor);
static void parse_process_specific_descriptor(AudioDriver* driver, 
    ProcessingUnitDescriptor* process_unit, uint16_t wProcessType,
    uint8_t* pos, MemoryService_C* m);
static int processing_unit_build_routine(AudioDriver* driver, uint8_t* start,
    ACInterface* ac_interface, MemoryService_C* m);
static int extension_unit_build_routine(AudioDriver* driver, uint8_t* start,
    ACInterface* ac_interface, MemoryService_C* m);
static int associated_interface_build_routine(AudioDriver* driver, uint8_t* start,
    ACInterface* ac_interface, MemoryService_C* m);
static int handle_ac_routine(AudioDriver* driver, uint8_t** start, 
  MemoryService_C* m, uint8_t minor);
static int handle_as_routine(AudioDriver* driver, uint8_t** start, 
  uint8_t* end, MemoryService_C* m, uint8_t minor);
static int handle_as_endpoint_routine(AudioDriver* driver, MemoryService_C* m, uint8_t* start,
  Alternate_Interface* alt_interface, uint8_t endpoint_num);
static void audio_device_routine(AudioDriver* driver, uint8_t* start,
    uint8_t* end, MemoryService_C* mem_service, uint8_t minor);
static int configure_audio_device(AudioDriver* driver);
static void feature_control_first_form(AudioDriver* driver, uint8_t control, 
    uint8_t request_form, UsbDev* dev, UsbDeviceRequest* device_req, uint8_t bRequest,
    uint8_t channel_number, uint8_t unit_id, uint8_t audio_interface,
    void* data, uint8_t len, callback_function callback, uint8_t flags);
static void feature_control_second_form(AudioDriver* driver, uint8_t control, 
    uint8_t request_form, UsbDev* dev, UsbDeviceRequest* device_req, 
    uint8_t bRequest, uint8_t feature_unit_id, uint8_t audio_interface, 
    void* data, uint8_t len, callback_function callback, uint8_t flags);
static SpatialLocations __get_loc_by_channel_number(AudioDriver* driver, 
    ClusterDescriptor cluster, uint8_t channel_number);
static int16_t __get_channel_number_by_loc(AudioDriver* driver, ClusterDescriptor cluster,
    SpatialLocations loc);
static int8_t feature_control_sub_routine_checks(AudioDriver* driver,
    AudioDev* audio_dev, uint8_t request_form, uint8_t channel_number, 
    uint8_t feature_unit_id, uint8_t audio_interface);
static int8_t audio_mute_control(AudioDriver* driver, AudioDev* audio_dev,
    uint8_t request_form, uint8_t bRequest, uint8_t channel_number, 
    uint8_t audio_interface, uint8_t feature_unit_id, void* data, uint8_t len, 
    callback_function callback, uint8_t flags);
static int8_t audio_volume_control(AudioDriver* driver, AudioDev* audio_dev,
    uint8_t request_form, uint8_t bRequest, uint8_t channel_number,
    uint8_t audio_interface, uint8_t feature_unit_id, void* data, uint8_t len,
    callback_function callback, uint8_t flags);
static int8_t audio_mixer_control(AudioDriver* driver, AudioDev* audio_dev,
    uint8_t request_form, uint8_t bRequest, uint8_t icn, uint8_t ocn,
    uint8_t mixer_unit_id, uint8_t audio_interface, void* data, uint8_t len,
    callback_function callback);
static int8_t mixer_first_form_build(AudioDriver* driver, MemoryService_C* mem_service,
    uint16_t mixer_value, AudioDev* audio_dev, uint8_t request_form, uint8_t bRequest,
    uint8_t icn, uint8_t ocn, uint8_t audio_interface, uint8_t mixer_unit_id);
static int8_t mixer_second_form_build(AudioDriver* driver, MemoryService_C* mem_service,
    AudioDev* audio_dev, uint8_t request_form, uint8_t bRequest, uint8_t icn,
    uint8_t ocn, uint8_t mixer_unit_id, uint8_t audio_interface);
static int8_t set_mixer_unit_control(AudioDriver* driver, AudioDev* audio_dev,
    uint16_t mixer_value, uint8_t icn, uint8_t ocn, uint8_t mixer_unit_id,
    uint8_t audio_interface);
static int8_t get_mixer_unit_control(AudioDriver* driver, AudioDev* audio_dev);
static int8_t audio_selector_control(AudioDriver* driver, AudioDev* audio_dev,
    uint8_t request_form, uint8_t bRequest, uint8_t selector_unit_id, uint8_t audio_interface, 
    void* data, uint8_t len, callback_function callback);
static int8_t selector_set_cur(AudioDriver* driver, AudioDev* audio_dev, 
    uint8_t bSelector, uint8_t selector_unit_id, uint8_t audio_interface);
static int8_t audio_processing_control(AudioDriver* driver, AudioDev* audio_dev,
    uint8_t request_form, uint8_t bRequest, uint8_t control_selector,
    uint8_t processing_unit_id, uint8_t audio_interface,
    void* data, uint8_t len, callback_function callback);
static int8_t audio_extension_control(AudioDriver* driver, AudioDev* audio_dev,
    uint8_t request_form, uint8_t bRequest, uint8_t control_selector,
    uint8_t extension_unit_id, uint8_t audio_interface, void* data, uint8_t len,
    callback_function callback);
static int8_t audio_enable_processing_control(AudioDriver* driver, 
    AudioDev* audio_dev, uint8_t request_form, uint8_t bRequest, 
    uint8_t processing_unit_id, uint8_t audio_interface, void* data, 
    uint8_t len, callback_function callback);
static int8_t enable_processing_control(AudioDriver* driver, AudioDev* audio_dev,
    uint8_t processing_unit_id, uint8_t audio_interface);
static int8_t disable_processing_control(AudioDriver* driver, AudioDev* audio_dev,
    uint8_t processing_unit_id, uint8_t audio_interface);
static int8_t audio_mode_select_control(AudioDriver* driver, AudioDev* audio_dev,
    uint8_t request_form, uint8_t bRequest, uint8_t processing_unit_id,
    uint8_t audio_interface, void* data, uint8_t len);
static int8_t audio_spaciousness_control(AudioDriver* driver, AudioDev* audio_dev,
    uint8_t request_form, uint8_t bRequest, uint8_t processing_unit_id,
    uint8_t audio_interface, void* data, uint8_t len);
static int8_t audio_reverb_type_control(AudioDriver* driver, AudioDev* audio_dev,
    uint8_t request_form, uint8_t bRequest, uint8_t processing_unit_id,
    uint8_t audio_interface, void* data, uint8_t len);
static int8_t audio_reverb_level_control(AudioDriver* driver, AudioDev* audio_dev,
    uint8_t request_form, uint8_t bRequest, uint8_t processing_unit_id,
    uint8_t audio_interface, void* data, uint8_t len);
static int8_t audio_chorus_level_control(AudioDriver* driver, AudioDev* audio_dev,
    uint8_t request_form, uint8_t bRequest, uint8_t processing_unit_id,
    uint8_t audio_interface, void* data, uint8_t len);
static int8_t audio_set_cur_spaciousness_control(AudioDriver* driver, AudioDev* audio_dev,
    uint8_t request_form, uint8_t bRequest, uint8_t processing_unit_id,
    uint8_t audio_interface, void* data, uint8_t len, uint8_t bSpaciousness);
static enum ProcessingUnitProcessTypes retrieve_process_type(AudioDriver* driver,
    uint8_t* pos);
static int8_t supports_audio_mode(AudioDriver* driver, AudioDev* audio_dev,
    uint8_t processing_unit_id, uint8_t mode_value);
static int8_t set_audio_mode_attribute(AudioDriver* driver, AudioDev* audio_dev,
    uint8_t processing_unit_id, uint8_t audio_interface, uint8_t bMode);
static int8_t enable_extension_unit(AudioDriver* driver, AudioDev* audio_dev,
    uint8_t extension_unit_id, uint8_t audio_interface);
static int8_t disable_extension_unit(AudioDriver* driver, AudioDev* audio_dev,
    uint8_t extension_unit_id, uint8_t audio_interface);
static int8_t audio_copy_protect_control(AudioDriver* driver, AudioDev* audio_dev,
    uint8_t request_form, uint8_t bRequest, uint8_t channel_number,
    uint8_t audio_interface, uint8_t terminal_unit_id, void* data, uint8_t len,
    callback_function callback, uint8_t flags);
static int8_t set_copy_protect(AudioDriver* driver, AudioDev* audio_dev,
    uint8_t output_terminal_id, CPL copy_protect_value, uint8_t channel_number,
    uint8_t audio_interface);
static int8_t get_copy_protect(AudioDriver* driver, AudioDev* audio_dev,
    uint8_t input_terminal_id, CPL copy_protect_value, uint8_t channel_number,
    uint8_t audio_interface);
static int8_t set_sound_value(AudioDriver* driver, int16_t value, AudioDev* audio_dev,
    uint8_t channel_number, uint8_t audio_interface, uint8_t feature_unit_id);
static int8_t set_max_sound_value(AudioDriver* driver, int16_t value, 
    AudioDev* audio_dev, uint8_t channel_number, uint8_t audio_interface,
    uint8_t feature_unit_id);
static int8_t set_min_sound_value(AudioDriver* driver, int16_t value,
    AudioDev* audio_dev, uint8_t channel_number, uint8_t audio_interface, 
    uint8_t feature_unit_id);
static int8_t set_volume_resolution(AudioDriver* driver, AudioDev* audio_dev,
    int16_t resolution_value, uint8_t channel_number, uint8_t audio_interface,
    uint8_t feature_unit_id);
static int8_t increment_sound(AudioDriver* driver, AudioDev* audio_dev,
    uint8_t channel_number, uint8_t audio_interface, uint8_t feature_unit_id);
static int8_t decrement_sound(AudioDriver* driver, AudioDev* audio_dev,
    uint8_t channel_number, uint8_t audio_interface, uint8_t feature_unit_id);
static int8_t mute(AudioDriver* driver, AudioDev* audio_dev, uint8_t channel_number,
    uint8_t audio_interface, uint8_t feature_unit_id);
static int8_t unmute(AudioDriver* driver, AudioDev* audio_dev,
    uint8_t channel_number, uint8_t audio_interface, uint8_t feature_unit_id);
static AudioDev* get_free_audio_dev(AudioDriver* driver);
static AudioDev* match_audio_dev(AudioDriver* driver, UsbDev* dev);
static void free_audio_dev(AudioDriver* driver, AudioDev* audio_dev);
static int handle_format_routine(AudioDriver* driver, uint8_t* start, MemoryService_C* m,
    ASInterface* as_interface);
static int handle_format_type_1(AudioDriver* driver, uint8_t* start,
    MemoryService_C* m, ASInterface* as_interface);
static int handle_format_type_2(AudioDriver* driver);
static int handle_format_type_3(AudioDriver* driver);
static struct DiscreteNumberSamplingFrequencies* __parse_discrete_sample_freq(AudioDriver* driver, uint8_t* start, 
    uint8_t discrete_sample_num, MemoryService_C* m);
static struct ContinuousSamplingFrequency* __parse_contig_sample_freq(AudioDriver* driver, uint8_t* start,
    MemoryService_C* m);
static int8_t set_sampling_frequency(AudioDriver* driver, AudioDev* audio_dev,
    Interface* as_interface, struct SampleFrequency sample_freq);
static struct SampleFrequency get_sampling_frequency(AudioDriver* driver,
    AudioDev* audio_dev, Interface* as_interface);

static AudioDriver* internal_audio_driver = 0;

static inline uint8_t __is_cs_interface(AudioDriver* driver, uint8_t* start){
  return __IF_EXT__((*(start + 1) == CS_INTERFACE), 1, 0);
}

static inline uint8_t __is_as_endpoint(AudioDriver* driver, uint8_t* start){
  return __IF_EXT__((*(start + 1) == CS_ENDPOINT) &&
    (*(start + 2) == EP_GENERAL), 1, 0);
}

static inline uint8_t __is_ac_interface(AudioDriver* driver, uint8_t* start){
  return __IF_EXT__(__is_cs_interface(driver, start) && 
    (*(start + 2) == HEADER), 1, 0);
}

static inline uint8_t __is_as_interface(AudioDriver* driver, uint8_t* start){
  return __IF_EXT__(__is_cs_interface(driver, start) && 
    (*(start + 2) == AS_GENERAL), 1, 0);
}

static inline uint8_t __is_format_type(AudioDriver* driver, uint8_t* start){
    return __IF_EXT__(__is_cs_interface(driver, start) && 
        (*(start+2) == FORMAT_TYPE), 1, 0);
}

static inline uint8_t __is_input_terminal(AudioDriver* driver, uint8_t* start){
  return __IF_EXT__((*(start + 2) == INPUT_TERMINAL), 1, 0);
}

static inline uint8_t __is_output_terminal(AudioDriver* driver, uint8_t* start){
  return __IF_EXT__((*(start + 2) == OUTPUT_TERMINAL), 1, 0);
}

static inline uint8_t __is_mixer(AudioDriver* driver, uint8_t* start){
  return __IF_EXT__((*(start + 2) == MIXER_UNIT), 1, 0);
}

static inline uint8_t __is_selector(AudioDriver* driver, uint8_t* start){
  return __IF_EXT__((*(start + 2) == SELECTOR_UNIT), 1, 0);
}

static inline uint8_t __is_feature(AudioDriver* driver, uint8_t* start){
  return __IF_EXT__((*(start + 2) == FEATURE_UNIT), 1, 0);
}

static inline uint8_t __is_processing_unit(AudioDriver* driver, uint8_t* start){
  return __IF_EXT__((*(start + 2) == PROCESSING_UNIT), 1, 0);
}

static inline uint8_t __is_extension_unit(AudioDriver* driver, uint8_t* start){
  return __IF_EXT__((*(start + 2) == EXTENSION_UNIT), 1, 0);
}

static int8_t __is_channel_in_cluster(AudioDriver* driver, ClusterDescriptor cluster,
    SpatialLocations loc){
    return __IF_EXT__(__NOT_ZERO__(cluster.wChannelConfig & loc), 1, -1);
}

static void __set_default_audio_properties(AudioDriver* driver, 
    AudioDev* audio_dev, UsbDev* dev){
    audio_dev->usb_driver = (UsbDriver*)driver;
    audio_dev->usb_dev = dev;
}

static int16_t probe_audio(UsbDev* dev, Interface* interface){
    AudioDev* audio_dev = match_audio_dev(internal_audio_driver, dev);
    if(__IS_NULL__(audio_dev)){
        audio_dev = get_free_audio_dev(internal_audio_driver);
        __IF_RET_NEG__(__IS_NULL__(audio_dev));
    }
    uint8_t bInterfaceClass = interface->active_interface->
        alternate_interface_desc.bInterfaceClass;
    uint8_t bInterfaceSubClass = interface->active_interface->
        alternate_interface_desc.bInterfaceSubClass;
    if(bInterfaceClass == AUDIO){
        if(bInterfaceSubClass == AUDIO_CONTROL){
            audio_dev->audio_control_interface = interface;
            __set_default_audio_properties(internal_audio_driver,
                audio_dev, dev);
            return __RET_S__;
        }
        else if(bInterfaceSubClass == AUDIO_STREAMING){
            audio_dev->audio_streaming_interfaces[audio_dev->audio_streaming_interfaces_num] = interface;
            audio_dev->audio_streaming_interfaces_num++;
            __set_default_audio_properties(internal_audio_driver, 
                audio_dev, dev);
            return __RET_S__;
        }
    }
    return __RET_E__;
}

static void disconnect_audio(UsbDev* dev, Interface* interface){}

static void callback_audio(UsbDev* dev, uint32_t status, void* data){
    __IF_RET__(status & E_TRANSFER);
    
}

void new_audio_driver(AudioDriver* driver, char* name, UsbDevice_ID* entry){
    __FOR_RANGE__(i, int, 0, MAX_DEVICES_PER_USB_DRIVER){
        driver->audio_map[i] = 0;
        driver->dev[i].usb_dev = 0;
        driver->dev[i].endpoint_addr = 0;
        driver->dev[i].buffer = 0;
        driver->dev[i].buffer_size = 0;
        driver->dev[i].priority = 0;
        driver->dev[i].audio_control_interface = 0;
        driver->dev[i].audio_streaming_interfaces_num = 0;
        driver->dev[i].interval = 0;
        driver->dev[i].callback = &callback_audio;
        driver->dev[i].usb_driver = (UsbDriver *)driver;
        driver->dev[i].curr_volume = 0;
        driver->dev[i].max_volume = 0;
        driver->dev[i].min_volume = 0;
        __FOR_RANGE__(j, int, 0, __MAX_STREAMING_INTERFACES__){
            driver->dev[i].audio_streaming_interfaces[i] = 0;
        }
    }
    internal_audio_driver = driver;
    __INIT_AUDIO_DRIVER__(driver, name, entry);
}

static AudioDev *get_free_audio_dev(AudioDriver *driver) {
  __GET_FREE_DEV__(AudioDev, driver->dev, driver->audio_map);
}

static AudioDev *match_audio_dev(AudioDriver *driver, UsbDev *dev) {
  __MATCH_DEV__(AudioDev, driver->dev, usb_dev, dev);
}

static void free_audio_dev(AudioDriver *driver, AudioDev *audio_dev) {
  __FREE_DEV__(audio_dev, driver->dev, driver->audio_map);
}

static void audio_callback(UsbDev *dev, uint32_t status, void *data) {
  if (status & E_TRANSFER) {
    dev->error_while_transfering = 1;
  } else if (status & S_TRANSFER) {
    dev->error_while_transfering = 0;
  }
}

static inline void __parse_common_unit_descriptor_part(AudioDriver* driver, uint8_t* start,
    uint8_t* len_member, uint8_t* type_member, uint8_t* subtype_member,
    uint8_t* unit_id){
    *len_member = *start;
    *type_member = *(start+1);
    *subtype_member = *(start+2);
    *unit_id = *(start+3);
}

static inline void __set_requests(AudioDriver* driver, UsbDev* dev, 
    UsbDeviceRequest* device_req, uint8_t recipient, uint8_t bRequest, 
    uint8_t wValueHigh, uint8_t wValueLow, uint8_t wIndexHigh,
    uint8_t wIndexLow, void* data, uint8_t len, callback_function callback, 
    uint8_t flags){
    __STRUCT_CALL__(dev, request_build, device_req, 
        HOST_TO_DEVICE | TYPE_REQUEST_CLASS | recipient, 
        bRequest, wValueHigh, wValueLow, 8, 
        __8_BIT_H_SHIFT__(wIndexHigh) | wIndexLow, len);
    __STRUCT_CALL__(dev, request, device_req, data, PRIORITY_QH_8, 0,
         callback, flags);
}

static inline void __get_requests(AudioDriver* driver, UsbDev* dev, 
    UsbDeviceRequest* device_req, uint8_t recipient, uint8_t bRequest, 
    uint8_t wValueHigh, uint8_t wValueLow, uint8_t wIndexHigh, 
    uint8_t wIndexLow, void* data, uint8_t len, callback_function callback,
    uint8_t flags){
    __STRUCT_CALL__(dev, request_build, device_req, 
        DEVICE_TO_HOST | TYPE_REQUEST_CLASS | recipient, 
        bRequest, wValueHigh, wValueLow, 8, 
        __8_BIT_H_SHIFT__(wIndexHigh) | wIndexLow, len);
    __STRUCT_CALL__(dev, request, device_req, data, PRIORITY_QH_8, 0,
         callback, flags);
}

static inline void __request_terminal_set(AudioDriver* driver, UsbDev* dev, 
    UsbDeviceRequest* device_req, uint8_t control_selector, 
    uint8_t audio_interface, uint8_t terminal_id, void* data, uint8_t len,
    callback_function callback, uint8_t flags){
    __set_requests(driver, dev, device_req, RECIPIENT_INTERFACE, SET_CUR,
        control_selector, 0, terminal_id, audio_interface, data, len, 
        callback, flags);
}

static inline void __request_terminal_get(AudioDriver* driver, UsbDev* dev,
    UsbDeviceRequest* device_req, uint8_t bRequest, uint8_t control_selector,
    uint8_t audio_interface, uint8_t terminal_id, void* data, uint8_t len,
    callback_function callback, uint8_t flags){
    __IF_RET__(bRequest != GET_CUR && bRequest != GET_MIN
        && bRequest != GET_MAX && bRequest != GET_RES);
    __get_requests(driver, dev, device_req, RECIPIENT_INTERFACE, bRequest,
        control_selector, 0, terminal_id, audio_interface, data, len, 
        callback, flags);
}

static inline void __request_mixer_set(AudioDriver* driver, UsbDev* dev,
    UsbDeviceRequest* device_req, uint8_t icn, uint8_t ocn,
    uint8_t mixer_unit_id, uint8_t audio_interface, void* data, uint8_t len,
    callback_function callback, uint8_t flags){
    __set_requests(driver, dev, device_req, RECIPIENT_INTERFACE, SET_CUR,
        icn, ocn, mixer_unit_id, audio_interface, data, len, callback, flags);
}

static inline void __request_mixer_get(AudioDriver* driver, UsbDev* dev,
    UsbDeviceRequest* device_req, uint8_t bRequest, uint8_t icn, uint8_t ocn,
    uint8_t mixer_unit_id, uint8_t audio_interface, void* data, uint8_t len,
    callback_function callback, uint8_t flags){
    __IF_RET__(bRequest != GET_CUR && bRequest != GET_MIN && 
        bRequest != GET_MAX && bRequest != GET_RES);
    __get_requests(driver, dev, device_req, RECIPIENT_INTERFACE, bRequest,
        icn, ocn, mixer_unit_id, audio_interface, data, len, callback, flags);
}

static inline void __request_selector_set(AudioDriver* driver, UsbDev* dev,
    UsbDeviceRequest* device_req, uint8_t selector_unit_id,
    uint8_t audio_interface, void* data, uint8_t len,
    callback_function callback, uint8_t flags){
    __set_requests(driver, dev, device_req, RECIPIENT_INTERFACE, SET_CUR,
        0, 0, selector_unit_id, audio_interface, data, len, callback, flags);
}

static inline void __request_selector_get(AudioDriver* driver, UsbDev* dev,
    UsbDeviceRequest* device_req, uint8_t bRequest, uint8_t selector_unit_id,
    uint8_t audio_interface, void* data, uint8_t len, 
    callback_function callback, uint8_t flags){
    __IF_RET__(bRequest != GET_CUR && bRequest != GET_MIN && 
        bRequest != GET_MAX);
    __get_requests(driver, dev, device_req, RECIPIENT_INTERFACE, bRequest,
        0, 0, selector_unit_id, audio_interface, data, len, callback, flags);
}

static inline void __request_feature_set(AudioDriver* driver, UsbDev* dev,
    UsbDeviceRequest* device_req, uint8_t control_selector, 
    uint8_t channel_number, uint8_t feature_unit_id,
    uint8_t audio_interface, void* data, uint8_t len, 
    callback_function callback, uint8_t flags){
    __set_requests(driver, dev, device_req, RECIPIENT_INTERFACE, SET_CUR,
        control_selector, channel_number, feature_unit_id, audio_interface,
        data, len, callback, flags);
}

static inline void __request_feature_get(AudioDriver* driver, UsbDev* dev,
    UsbDeviceRequest* device_req, uint8_t bRequest, uint8_t control_selector, 
    uint8_t channel_number, uint8_t feature_unit_id,
    uint8_t audio_interface, void* data, uint8_t len,
    callback_function callback, uint8_t flags){
    __IF_RET__(bRequest != GET_CUR && bRequest != GET_MIN && 
        bRequest != GET_MAX && bRequest != GET_RES);
    __get_requests(driver, dev, device_req, RECIPIENT_INTERFACE, bRequest,
        control_selector, channel_number, feature_unit_id, audio_interface,
        data, len, callback, flags);
}

static inline void __request_processing_set(AudioDriver* driver, UsbDev* dev,
    UsbDeviceRequest* device_req, uint8_t control_selector, 
    uint8_t processing_unit_id, uint8_t audio_interface, void* data, uint8_t len,
    callback_function callback, uint8_t flags){
    __set_requests(driver, dev, device_req, RECIPIENT_INTERFACE, SET_CUR,
        control_selector, 0, processing_unit_id, audio_interface, data, len, 
        callback, flags);
}

static inline void __request_processing_get(AudioDriver* driver, UsbDev* dev,
    UsbDeviceRequest* device_req, uint8_t bRequest, uint8_t control_selector, 
    uint8_t processing_unit_id, uint8_t audio_interface, void* data, uint8_t len,
    callback_function callback, uint8_t flags){
    __IF_RET__(bRequest != GET_CUR && bRequest != GET_MIN && 
        bRequest != GET_MAX && bRequest != GET_RES);
    __get_requests(driver, dev, device_req, RECIPIENT_INTERFACE, bRequest,
        control_selector, 0, processing_unit_id, audio_interface, data, len, 
        callback, flags);
}

static inline void __request_extension_set(AudioDriver* driver, UsbDev* dev,
    UsbDeviceRequest* device_req, uint8_t control_selector,
    uint8_t extension_unit_id, uint8_t audio_interface, void* data, uint8_t len,
    callback_function callback, uint8_t flags){
    __set_requests(driver ,dev, device_req, RECIPIENT_INTERFACE, SET_CUR,
        control_selector, 0, extension_unit_id, audio_interface, data, len, 
        callback, flags);
}

static inline void __request_extension_get(AudioDriver* driver, UsbDev* dev,
    UsbDeviceRequest* device_req, uint8_t bRequest, uint8_t control_selector,
    uint8_t extension_unit_id, uint8_t audio_interface, void* data, uint8_t len,
    callback_function callback, uint8_t flags){
    __IF_RET__(bRequest != GET_CUR && bRequest != GET_MIN && 
        bRequest != GET_MAX && bRequest != GET_RES);
    __get_requests(driver, dev, device_req, RECIPIENT_INTERFACE, bRequest,
        control_selector, 0, extension_unit_id, audio_interface, data, len, 
            callback, flags);
}

static inline void __request_endpoint_set(AudioDriver* driver, UsbDev* dev,
    UsbDeviceRequest* device_req, uint8_t control_selector, uint8_t endpoint,
    void* data, uint8_t len, callback_function callback, uint8_t flags){
    __set_requests(driver, dev, device_req, RECIPIENT_ENDPOINT, SET_CUR,
        control_selector, 0, 0, endpoint, data, len, callback, flags);
}

static inline void __request_endpoint_get(AudioDriver* driver, UsbDev* dev,
    UsbDeviceRequest* device_req, uint8_t bRequest, uint8_t control_selector, 
    uint8_t endpoint, void* data, uint8_t len, callback_function callback, 
    uint8_t flags){
    __IF_RET__(bRequest != GET_CUR && bRequest != GET_MIN && 
        bRequest != GET_MAX && bRequest != GET_RES);
    __get_requests(driver, dev, device_req, RECIPIENT_ENDPOINT, bRequest,
        control_selector, 0, 0, endpoint, data, len, callback, flags);
}

static inline uint8_t* __parse_bmControls(AudioDriver* driver, uint8_t* pos,
    uint8_t bm_control_number, uint8_t* control_size, MemoryService_C* m){
    __ALLOC_KERNEL_MEM_T__(m, uint8_t, bmaControl_K, *control_size);
    __FOR_RANGE__(i, int, 0, *control_size){
        *(bmaControl_K+i) = *((pos + i) +
        (bm_control_number*(*control_size)));
    }

    return bmaControl_K;
}

static void inline __parse_bmaControls(AudioDriver* driver, uint8_t*** member,
    uint8_t* control_size, MemoryService_C* m, uint8_t upstream_channels,
    uint8_t* start){
    __ALLOC_KERNEL_MEM_T__(m, uint8_t*, bmaControls, sizeof(uint8_t*) * 
        (upstream_channels+1));
    uint8_t* bmaControl_offset = start+6;
    __FOR_RANGE__(j, int, 0, upstream_channels+1){
        uint8_t* bmControl = __parse_bmControls(driver, bmaControl_offset, 
            j, control_size, m);
        *(bmaControls+j) = bmControl;
    }
    *member = bmaControls;
}

static inline int8_t __valid_request_form(AudioDriver* driver, uint8_t request_form){
    return __IF_EXT__((request_form == __SET_REQUEST__ || 
        request_form == __GET_REQUEST__), 1, 0);
}

static inline uint8_t __get_total_channels_in_cluster(AudioDriver* driver, 
    ClusterDescriptor cluster){
    return cluster.bNrChannels;
}

static inline void* __search_for_unit(AudioDriver* driver, uint8_t id){
    return 0;
}

static inline uint8_t* __get_feature_bmControl_of_channel(AudioDriver* driver, 
    ACInterface* ac_interface, uint8_t channel_number, uint8_t feature_unit_id){ 
    /*__IF_RET_NULL__(__IS_NULL__(ac_interface->feature_unit));
    struct FeatureUnit* feature_unit = __search_for_unit(driver);
    uint8_t upstream_channels = get_upstream_channels_by_source_id_depth(driver, 
      ac_interface->feature_unit->feature_unit_desc.bSourceID,
        ac_interface, __MAX_UPSTREAM_DEPTH__);
    __IF_RET_NULL__(channel_number > upstream_channels);
    return *(ac_interface->feature_unit->feature_unit_desc.bmaControls + channel_number);
*/}

static inline int8_t __bmControl_supported(AudioDriver* driver, uint8_t* bmControl,
    enum FeatureUnitbmaControls control_bit){
    __IF_EXT__((__IS_ZERO__((*bmControl & control_bit))), -1 , 0);
}

static inline int8_t __bmControl_mute_supported(AudioDriver* driver, uint8_t* bmControl){
    return __bmControl_supported(driver, bmControl, BIT_MUTE);
}

static inline int8_t __bmControl_volume_supported(AudioDriver* driver, uint8_t* bmControl){
    return __bmControl_supported(driver, bmControl, BIT_VOL);
}

static inline int8_t __bmControl_bass_supported(AudioDriver* driver, uint8_t* bmControl){
    return __bmControl_supported(driver, bmControl, BIT_BASS);
}

static inline int8_t __bmControl_mid_supported(AudioDriver* driver, uint8_t* bmControl){
    return __bmControl_supported(driver, bmControl, BIT_MID);
}

static inline int8_t __bmControl_graphic_equalizer_supported(AudioDriver* driver, 
    uint8_t* bmControl){
    return __bmControl_supported(driver, bmControl, BIT_GRAPHIC_EQ);
}

static inline int8_t __bmControl_delay_supported(AudioDriver* driver, uint8_t* bmControl){
    return __bmControl_supported(driver, bmControl, BIT_DELAY);
}

static inline void __parse_waModes(AudioDriver* driver, uint8_t* pos,
    uint8_t* number_modes, UP_DOWN_MixProcessingUnitDescriptor* 
        up_down_mix_processing_unit_descriptor){
    __FOR_RANGE__(i, int, 0, *number_modes){
        *(up_down_mix_processing_unit_descriptor->waModes + i) = 
            (uint16_t*)(pos + i);
    }
}

static inline void __assign_process(AudioDriver* driver, void* process,
    ProcessingUnitDescriptor* processing_unit_descriptor){
    processing_unit_descriptor->process_specific_descriptor = process;
}

static inline int8_t __input_terminal_present(AudioDriver* driver, 
    ACInterface* ac_interface, uint8_t input_terminal_id){
    __TRAVERSE_UNIT__{
        __INPUT_TERMINAL_MATCH__(driver, ac_interface, input_terminal_id, i, 
            return __RET_S__);
    }
    return __RET_N__;
}

static inline int8_t __output_terminal_present(AudioDriver* driver, ACInterface* ac_interface,
    uint8_t output_terminal_id){
    __TRAVERSE_UNIT__{
        __OUTPUT_TERMINAL_MATCH__(driver, ac_interface, output_terminal_id, i, 
            return __RET_S__);
    }
    return __RET_N__;
}

static inline int8_t __feature_unit_present(AudioDriver* driver, ACInterface* ac_interface, 
    uint8_t feature_unit_id){
    __TRAVERSE_UNIT__{
        __FEATURE_UNIT_MATCH__(driver, ac_interface, feature_unit_id, i, return __RET_S__);
    }   
    return __RET_N__;
}

static inline int8_t __processing_unit_present(AudioDriver* driver, ACInterface* ac_interface,
    uint8_t processing_unit_id){
    __TRAVERSE_UNIT__{
        __PROCESSING_UNIT_MATCH__(driver, ac_interface, processing_unit_id, i, return __RET_S__);
    }
    return __RET_N__;
}

static inline int8_t __extension_unit_present(AudioDriver* driver, ACInterface* ac_interface,
    uint8_t extension_unit_id){
    __TRAVERSE_UNIT__{
        __EXTENSION_UNIT_MATCH__(driver, ac_interface, extension_unit_id, i, return __RET_S__);
    }
    return __RET_N__;
}

static inline int8_t __selector_unit_present(AudioDriver* driver, ACInterface* ac_interface,
    uint8_t selector_unit_id){
    __TRAVERSE_UNIT__{
        __SELECTOR_UNIT_MATCH__(driver, ac_interface, selector_unit_id, i, return __RET_S__);
    }
    return __RET_N__;
}

static inline int8_t __mixer_unit_present(AudioDriver* driver, ACInterface* ac_interface,
    uint8_t mixer_unit_id){
    __TRAVERSE_UNIT__{
        __MIXER_UNIT_MATCH__(driver, ac_interface, mixer_unit_id, i, return __RET_S__);
    }
    return __RET_N__;
}

static inline int8_t __channel_present(AudioDriver* driver, AudioDev* audio_dev,
    uint8_t channel_number){
    return __RET_S__;
}

static inline int8_t __audio_interface_present(AudioDriver* driver, AudioDev* dev, 
    uint8_t audio_interface){
    // each audio control interface resides in an interface
    uint8_t num_interfaces = 
        dev->usb_dev->active_config->config_desc.bNumInterfaces;
    return __IF_EXT__(audio_interface < num_interfaces, 1, 0);
}

static inline void __build_selector_control_block(AudioDriver* driver, 
    SelectorControlParameterBlock* selector_control_block, uint8_t selector_value){
    selector_control_block->wLength = 1;
    selector_control_block->bSelector = selector_value;
}

static inline void __build_mixer_control_first_form(AudioDriver* driver, 
    MixerControlParameterBlockFirstForm* mixer_control_block, uint16_t mixer_value){
    mixer_control_block->wLength = 2;
    mixer_control_block->wMixer = mixer_value;
}

// second form ;; for the wLength field just use the number of prog controls 
// calculated prior
// each wMixer field is 2 bytes long
static inline void __build_mixer_control_second_form(AudioDriver* driver, 
    MixerControlParameterBlockSecondForm* mixer_control_block, uint8_t wLength,
    uint8_t* bMute){
    
}

static inline void __build_copy_protect_control(AudioDriver* driver, 
    CopyProtectControlParameterBlock* copy_protect_block, CPL copy_protect){
    copy_protect_block->control_selector = COPY_PROTECT_CONTROL;
    copy_protect_block->wLength = 1;
    copy_protect_block->bCopyProtect = copy_protect;
}

static inline void __build_enable_processing_control_block(AudioDriver* driver,
    struct EnableProcessingControlParameterBlock* enable_processing_block, 
    uint8_t bEnable){
    enable_processing_block->control_selector = XU_ENABLE_CONTROL;
    enable_processing_block->wLength = 1;
    enable_processing_block->bEnable = bEnable;
}

static inline void __build_mode_select_control_block(AudioDriver* driver,
    struct ModeSelectControlParameterBlock* mode_select_block, uint8_t bMode){
    mode_select_block->control_selector = DP_MODE_SELECT_CONTROL;
    mode_select_block->wLength = 1;
    mode_select_block->bMode = bMode;
}

static inline void __build_spaciousness_control_block(AudioDriver* driver,
    struct SpaciousnessControlParameterBlock* control_block, uint8_t value){
    control_block->control_selector = STEREO_SPACIOUSNESS_CONTROL;
    control_block->wLength = 1;
    control_block->bSpaciousness = value;
}

static inline void __build_reverb_type_control_block(AudioDriver* driver,
    struct ReverbTypeControlParameterBlock* reverb_control_block,
    uint8_t bReverbType){
    
}

static inline void __build_reverb_level_control_block(AudioDriver* driver,
    struct ReverbLevelControlParameterBlock* reverb_control_block,
    uint8_t bReverbLevel){
    reverb_control_block->control_selector = REVERB_LEVEL_CONTROL;
    reverb_control_block->wLength = 1;
    reverb_control_block->bReverbLevel = bReverbLevel;
}

static inline void __build_chorus_level_control_block(AudioDriver* driver,
    struct ChorusLevelControlParameterBlock* chorus_control_block,
    uint8_t bChorusLevel){
    chorus_control_block->control_selector = CHORUS_LEVEL_CONTROL;
    chorus_control_block->wLength = 1;
    chorus_control_block->bChorusLevel = bChorusLevel;
}

static inline void __build_enable_extension_processing_control_block(AudioDriver* driver,
    struct EnableProcessingControlParameterBlock* control_block, uint8_t bOn){
    control_block->control_selector = XU_ENABLE_CONTROL;
    control_block->wLength = 1;
    control_block->bEnable = bOn;
}

static inline void __build_volume_control_block(AudioDriver* driver, 
    VolumeControlParameterBlockFirstForm* volume_control_block, int16_t value){
    volume_control_block->control_selector = VOLUME_CONTROL;
    volume_control_block->wLength = 2;
    volume_control_block->wVolume = value;
}

static inline void __build_mute_control_block(AudioDriver* driver,
    MuteControlParameterBlockFirstForm* mute_control_block, uint8_t value){
    mute_control_block->control_selector = MUTE_CONTROL;
    mute_control_block->wLength = 1;
    mute_control_block->bMute = value;
}

// for the enumeration we are assuming that enumerating the unit/terminal is 
// done, so that when tracking upstream information the upstream unit/terminal
// is always available ; meaning that the unit/terminal was already enumerated 
// before !

static int input_build_routine(AudioDriver* driver, uint8_t* start,
  ACInterface* ac_interface, MemoryService_C* m){
  __IF_RET_NEG__(__UNIT_COUNT__(ac_interface, INPUT_TERMINAL));
  __ALLOC_KERNEL_MEM_S__(m, struct InputTerminal, audio_input_terminal);
  __TYPE_CAST__(InputTerminalDescriptor*, audio_in_desc, start);
  audio_input_terminal->in_terminal_desc = *audio_in_desc;
  __ADD_UNIT__(ac_interface, in_terminal, INPUT_TERMINAL, audio_input_terminal);

  return __RET_S__;
}

static int output_build_routine(AudioDriver* driver, uint8_t* start,
  ACInterface* ac_interface, MemoryService_C* m){
  __IF_RET_NEG__(__UNIT_COUNT__(ac_interface, OUTPUT_TERMINAL));
  __ALLOC_KERNEL_MEM_S__(m, struct OutputTerminal, audio_output_terminal);
  __TYPE_CAST__(OutputTerminalDescriptor*, audio_out_desc, start);
  audio_output_terminal->out_terminal_desc = *audio_out_desc;
  __ADD_UNIT__(ac_interface, out_terminal, OUTPUT_TERMINAL, audio_output_terminal);

  return __RET_S__;
}

static inline void __parse_baSourceID(AudioDriver* driver, uint8_t* start,
    uint8_t number_pins, MemoryService_C* mem_service,
    uint8_t** member){
    __ALLOC_KERNEL_MEM_T__(mem_service, uint8_t, baSource, number_pins);
    __FOR_RANGE__(i, int , 0, number_pins){
        *(baSource+i) = *(start+i);
    }
    *member = baSource;
}

static inline void __parse_cluster(AudioDriver* driver, ClusterDescriptor* cluster,
    uint8_t* start, uint8_t offset){
    start += offset;
    cluster->bNrChannels =  *(start);
    cluster->wChannelConfig = *(start+1);
    cluster->iChannelNames = (*start+3);
}

static void mute_control_callback(UsbDev *dev, uint32_t status, void *data){
    __IF_RET__(status & E_TRANSFER);
    __TYPE_CAST__(MuteControlParameterBlockFirstForm*, mute_control, data);
    MemoryService_C* mem_service = __DEV_MEMORY(dev);
    // need to free device request + free map io mem
}

static void volume_control_callback(UsbDev* dev, uint32_t status, void* data){
    __IF_RET__(status & E_TRANSFER);
    __TYPE_CAST__(MuteControlParameterBlockFirstForm*, mute_control, data);
    MemoryService_C* mem_service = __DEV_MEMORY(dev);
    // need to free device request + free map io mem
}

static void copy_protect_control_callback(UsbDev* dev, uint32_t status, void* data){
    __IF_RET__(status & E_TRANSFER);
    __TYPE_CAST__(CopyProtectControlParameterBlock*, copy_protect_block, data);    
    MemoryService_C* mem_service = __DEV_MEMORY(dev);
}

static void extension_callback(UsbDev* dev, uint32_t status, void* data){

}

static uint8_t* __parse_mixer_bmControls(AudioDriver* driver, uint8_t* pos, 
   MemoryService_C* m, uint8_t len){
  __ALLOC_KERNEL_MEM_S__(m, uint8_t, bmControls);
  uint8_t* end = pos + len;
  uint8_t ind = 0;
  while(pos < end){
    *(bmControls + ind) = *pos; pos++;
  }
  return bmControls;
}

static int mixer_build_routine(AudioDriver* driver, uint8_t* start,
  MemoryService_C* m, ACInterface* ac_interface){
  __IF_RET_NEG__(__UNIT_COUNT__(ac_interface, MIXER_UNIT));
  __ALLOC_KERNEL_MEM_S__(m, struct MixerUnit, mixer_unit);
  MixerUnitDescriptor mixer_unit_desc;
  uint8_t bNrInPins = *(start + 4);
  __parse_common_unit_descriptor_part(driver, start, 
    &mixer_unit_desc.bLength, &mixer_unit_desc.bDescriptorType,
    &mixer_unit_desc.bDescriptorSubtype, &mixer_unit_desc.bUnitID);
  mixer_unit_desc.bNrInPins = bNrInPins;
  __parse_baSourceID(driver, start + 5, bNrInPins, m, &mixer_unit_desc.baSourceID);
  __parse_cluster(driver, &mixer_unit_desc.cluster, start, 6 + bNrInPins);
  uint8_t output_channels = __get_total_channels_in_cluster(driver, 
    mixer_unit_desc.cluster);
  uint8_t bmControlLen = mixer_bmControlLength(driver, bNrInPins, 
    output_channels);
  uint8_t* bmControls = __parse_mixer_bmControls(driver, start+5+bNrInPins+4, m, 
    bmControlLen);
  mixer_unit_desc.bmControls = bmControls;
  mixer_unit_desc.iMixer = *(start+5+bNrInPins+4+bmControlLen);
  mixer_unit->bmControlsLength = bmControlLen;
  mixer_unit->mixer_unit_desc = mixer_unit_desc;
  __ADD_UNIT__(ac_interface, mixer_unit, MIXER_UNIT, mixer_unit);

  return __RET_S__;
}

static uint8_t get_upstream_channels_by_source_id_depth(AudioDriver* driver,
    uint8_t bSourceID, ACInterface* ac_interface, uint8_t depth){
    __IF_RET_ZERO__(depth == 0);
    __TRAVERSE_UNIT__{
        __TERMINAL_MATCH_ID__(driver, ac_interface->out_terminal[i]->out_terminal_desc, bSourceID,
        return get_upstream_channels_by_source_id_depth(driver, 
            ac_interface->out_terminal[i]->out_terminal_desc.bSourceID, ac_interface, depth-1));
        __UNIT_MATCH_ID__(driver, ac_interface->selector_unit[i]->selector_unit_desc, bSourceID,
            return get_upstream_channels_by_source_id_depth(driver,
                *ac_interface->selector_unit[i]->selector_unit_desc.baSourceID, ac_interface, depth-1));
        __UNIT_MATCH_ID__(driver, ac_interface->feature_unit[i]->feature_unit_desc, bSourceID,
            return get_upstream_channels_by_source_id_depth(driver, 
                ac_interface->feature_unit[i]->feature_unit_desc.bSourceID, ac_interface, depth-1));

        __TOTAL_CHANNEL_MATCH_TERMINAL__(driver, ac_interface->in_terminal[i]->in_terminal_desc, 
            bSourceID);
        __TOTAL_CHANNEL_MATCH_UNIT__(driver, ac_interface->mixer_unit[i]->mixer_unit_desc, bSourceID);
        __TOTAL_CHANNEL_MATCH_UNIT__(driver, ac_interface->processing_unit[i]->processing_unit_desc, bSourceID);
        __TOTAL_CHANNEL_MATCH_UNIT__(driver, ac_interface->extension_unit[i]->extension_unit_desc, bSourceID);
    }
    return __RET_N__;
}

static uint8_t get_upstream_channels_by_source_id(AudioDriver* driver, 
    uint8_t bSourceID, ACInterface* ac_interface){
    __TRAVERSE_UNIT__{
        __IF_RET_ZERO__(ac_interface->out_terminal[i]->out_terminal_desc.bTerminalID == bSourceID);
        __IF_RET_ZERO__(ac_interface->selector_unit[i]->selector_unit_desc.bUnitID == bSourceID
            || ac_interface->feature_unit[i]->feature_unit_desc.bUnitID == bSourceID);
        __TOTAL_CHANNEL_MATCH_TERMINAL__(driver, ac_interface->in_terminal[i]->in_terminal_desc, 
            bSourceID);
        __TOTAL_CHANNEL_MATCH_UNIT__(driver, ac_interface->mixer_unit[i]->mixer_unit_desc, bSourceID);
        __TOTAL_CHANNEL_MATCH_UNIT__(driver, ac_interface->processing_unit[i]->processing_unit_desc, bSourceID);
        __TOTAL_CHANNEL_MATCH_UNIT__(driver, ac_interface->extension_unit[i]->extension_unit_desc, bSourceID);
    }
    
    return __RET_N__;
}

static void* find_structure_by_id(AudioDriver* driver, ACInterface* ac_interface,
    uint8_t bSourceID, enum ACInterfaceDescriptorSubtypes* subtype){
    __TRAVERSE_UNIT__{
        __TERMINAL_MATCH_ID__(driver, ac_interface->in_terminal[i]->in_terminal_desc, bSourceID,
        *subtype = INPUT_TERMINAL; return ac_interface->in_terminal[i]);
        __TERMINAL_MATCH_ID__(driver, ac_interface->out_terminal[i]->out_terminal_desc, bSourceID,
            *subtype = OUTPUT_TERMINAL; return ac_interface->out_terminal[i]);
        __UNIT_MATCH_ID__(driver, ac_interface->mixer_unit[i]->mixer_unit_desc, bSourceID,
            *subtype = MIXER_UNIT; return ac_interface->mixer_unit[i]);
        __UNIT_MATCH_ID__(driver, ac_interface->selector_unit[i]->selector_unit_desc, bSourceID,
            *subtype = SELECTOR_UNIT; return ac_interface->selector_unit[i]);
        __UNIT_MATCH_ID__(driver, ac_interface->feature_unit[i]->feature_unit_desc, bSourceID,
            *subtype = FEATURE_UNIT; return ac_interface->feature_unit[i]);
        __UNIT_MATCH_ID__(driver, ac_interface->processing_unit[i]->processing_unit_desc, bSourceID,
            *subtype = PROCESSING_UNIT; return ac_interface->processing_unit[i]);
        __UNIT_MATCH_ID__(driver, ac_interface->extension_unit[i]->extension_unit_desc, bSourceID,
            *subtype = EXTENSION_UNIT; return ac_interface->extension_unit[i]);
    }
    return (void*)0;
}

// ret : terminal or unit
static void* get_upstream_structure(AudioDriver* driver, ACInterface* ac_interface, 
    enum ACInterfaceDescriptorSubtypes* subtype, void* unit, uint8_t bSource_num){
    uint8_t* raw = (uint8_t*)unit;
    switch(*(raw + 2)){
        case OUTPUT_TERMINAL : __SINGLE_UPSTREAM__(driver, ac_interface, unit, subtype, 
            OutputTerminalDescriptor)
        case MIXER_UNIT : __MULTI_UPSTREAM__(driver, ac_interface, unit, subtype,
            bSource_num, MixerUnitDescriptor);
        case SELECTOR_UNIT : __MULTI_UPSTREAM__(driver, ac_interface, unit, subtype,
            bSource_num, SelectorUnitDescriptor);
        case FEATURE_UNIT : __SINGLE_UPSTREAM__(driver, ac_interface, unit, subtype,
            FeatureUnitDescriptor);
        case PROCESSING_UNIT : __MULTI_UPSTREAM__(driver, ac_interface, unit, subtype,
            bSource_num, ProcessingUnitDescriptor);
        case EXTENSION_UNIT : __MULTI_UPSTREAM__(driver, ac_interface, unit, subtype,
            bSource_num, ExtensionUnitDescriptor);
        default: return (void*)0;
    }
}

static int8_t is_bm_control_programmable(AudioDriver* driver, 
    struct MixerUnit* mixer_unit, uint8_t input_channel, 
    uint8_t output_channel, uint8_t output_channels, uint8_t input_channels){
    uint8_t N = mixer_unit->bmControlsLength;
    uint8_t shift;
    __IF_ELSE__(((output_channels % 8) == 0), shift = output_channels / 8,
        shift = (output_channels / 8) + 1);
    uint8_t index = input_channel * shift;
    __IF_RET_NEG__(index > N);
    uint8_t* row = mixer_unit->mixer_unit_desc.bmControls + index;
    return ((*row) & output_channel);
}

static uint8_t mixer_bmControlLength(AudioDriver* driver,
    uint8_t input_channels, uint8_t output_channels){
    // bmControls consists of each row corresponding to an input channel and each 
    // column corresponding to an output channel
    // total count of bmControls is ((n * m) / 8) if mod 8 == 0; otherwise ((n * m) / 8) + 1
    uint8_t N;
    __IF_ELSE__(((input_channels * output_channels) / 8) % 8 != 0, 
        N = ((input_channels * output_channels) / 8) + 1,
        N = ((input_channels* output_channels) / 8));
    return N;
}

static int selector_build_routine(AudioDriver* driver, uint8_t* start,
  ACInterface* ac_interface, MemoryService_C* m){
  __IF_RET_NEG__(__UNIT_COUNT__(ac_interface, SELECTOR_UNIT));
  SelectorUnitDescriptor selector_unit_desc;
  uint8_t bNrInPins = *(start + 4);
  selector_unit_desc.bLength = *start;
  selector_unit_desc.bDescriptorType = *(start+1);
  selector_unit_desc.bDescriptorSubtype = *(start+2);
  selector_unit_desc.bUnitID = *(start+3);
  selector_unit_desc.bNrInPins = bNrInPins;
  __parse_baSourceID(driver, start+5, bNrInPins, m, 
    &selector_unit_desc.baSourceID);
  selector_unit_desc.iSelector = *(start+5+bNrInPins);
  ac_interface->selector_unit[__GET_UNIT_COUNT__(ac_interface, 
    SELECTOR_UNIT)]->selector_unit_desc = selector_unit_desc;
}

static int feature_build_routine(AudioDriver* driver, uint8_t* start,
  ACInterface* ac_interface, MemoryService_C* m){
  __IF_RET_NEG__(__UNIT_COUNT__(ac_interface, FEATURE_UNIT));
  __ALLOC_KERNEL_MEM_S__(m, struct FeatureUnit, feature_unit);
  FeatureUnitDescriptor feature_unit_desc;
  uint8_t control_size = *(start+5);
  __parse_common_unit_descriptor_part(driver, start, &feature_unit_desc.bLength,
    &feature_unit_desc.bDescriptorType, &feature_unit_desc.bDescriptorSubtype,
    &feature_unit_desc.bUnitID);
  feature_unit_desc.bSourceID = *(start+4);
  feature_unit_desc.bControlSize = control_size;
  uint8_t upstream_channels = get_upstream_channels_by_source_id_depth(driver, 
    feature_unit_desc.bSourceID, ac_interface, __MAX_UPSTREAM_DEPTH__);
  __parse_bmaControls(driver, &feature_unit_desc.bmaControls, &control_size,
    m, upstream_channels, start);
  feature_unit_desc.iFeature = *(start + 6 + 
    ((upstream_channels+1) * control_size));
  feature_unit->feature_unit_desc = feature_unit_desc;
  __ADD_UNIT__(ac_interface, feature_unit, FEATURE_UNIT, feature_unit);
  
  return __RET_S__;
}

static int up_down_mix_processing_unit_build_routine(AudioDriver* driver,
    uint8_t* start, MemoryService_C* m, 
    ProcessingUnitDescriptor* processing_unit_descriptor){
    __EXT_SPECIFIC_PROCESSING_UNIT_ROUTINE__(driver, start, processing_unit_descriptor,
        UP_DOWN_MixProcessingUnitDescriptor, m);
}

static int dolby_prologic_processing_unit_build_routine(AudioDriver* driver,
    MemoryService_C* m, uint8_t* start, ProcessingUnitDescriptor* processing_unit_descriptor){
    __EXT_SPECIFIC_PROCESSING_UNIT_ROUTINE__(driver, start, processing_unit_descriptor,
        DolbyPrologicProcessingUnitDescriptor, m);
}

static int stereo_extender_processing_unit_build_routine(AudioDriver* driver,
    MemoryService_C* m, uint8_t* start, ProcessingUnitDescriptor* processing_unit_descriptor){
    __SIMPLE_SPECIFIC_PROCESSING_UNIT_ROUTINE__(driver, start, 
        processing_unit_descriptor, THREE_D_StereoExtenderProcessingUnitDescriptor, m);
}

static int reverberation_processing_unit_build_routine(AudioDriver* driver,
    MemoryService_C* m, uint8_t* start, ProcessingUnitDescriptor* processing_unit_descriptor){
    __SIMPLE_SPECIFIC_PROCESSING_UNIT_ROUTINE__(driver, start, processing_unit_descriptor,
        ReverberationProcessingUnitDescriptor, m);
}

static int chorus_processing_unit_build_routine(AudioDriver* driver, 
    MemoryService_C* m, uint8_t* start, ProcessingUnitDescriptor* processing_unit_descriptor){
    __SIMPLE_SPECIFIC_PROCESSING_UNIT_ROUTINE__(driver, start, processing_unit_descriptor,
        ChorusProcessingUnitDescriptor, m);
}

static int dynamic_range_compressor_processing_unit_build_routine(AudioDriver* driver, 
    MemoryService_C* m, uint8_t* start, ProcessingUnitDescriptor* processing_unit_descriptor){
    __SIMPLE_SPECIFIC_PROCESSING_UNIT_ROUTINE__(driver, start, processing_unit_descriptor, 
        DynamicRangeCompressorProcessnigUnitDescriptor, m);
}

static void parse_process_specific_descriptor(AudioDriver* driver, 
    ProcessingUnitDescriptor* process_unit, uint16_t wProcessType,
    uint8_t* pos, MemoryService_C* m){
    switch(wProcessType){
        case UP_DOWNMIX_PROCESS : 
            up_down_mix_processing_unit_build_routine(driver, pos, m,
                process_unit); break;
        case DOLBY_PROLOGIC_PROCESS : 
            dolby_prologic_processing_unit_build_routine(driver, m, pos,
                process_unit); break;
        case STEREO_EXTENDER_PROCESS : 
            stereo_extender_processing_unit_build_routine(driver, m,
                pos, process_unit); break;
        case REVERBERATION_PROCESS : 
            reverberation_processing_unit_build_routine(driver, m,
                pos, process_unit); break;
        case CHORUS_PROCESS : chorus_processing_unit_build_routine(driver, m,
            pos, process_unit); break;
        case DYN_RANGE_COMP_PROCESS : 
            dynamic_range_compressor_processing_unit_build_routine(driver, m,
                pos, process_unit); break;
        default : break;
    };
}

static int processing_unit_build_routine(AudioDriver* driver, uint8_t* start,
    ACInterface* ac_interface, MemoryService_C* m){
    __IF_RET_NEG__(__UNIT_COUNT__(ac_interface, PROCESSING_UNIT));
    __ALLOC_KERNEL_MEM_S__(m, struct ProcessingUnit, processing_unit);
    ProcessingUnitDescriptor processing_unit_desc;
    __parse_common_unit_descriptor_part(driver, start, &processing_unit_desc.bLength,
        &processing_unit_desc.bDescriptorType, &processing_unit_desc.bDescriptorSubtype,
        &processing_unit_desc.bUnitID);
    processing_unit_desc.wProcessType = *(start+4);
    processing_unit_desc.bNrInPins = *(start+6);
    __parse_baSourceID(driver, start+7, processing_unit_desc.bNrInPins,
        m, &processing_unit_desc.baSourceID);
    __parse_cluster(driver, &processing_unit_desc.cluster, start, 
        7 + processing_unit_desc.bNrInPins);
    processing_unit_desc.bControlSize = *(start+11+processing_unit_desc.bNrInPins);
    processing_unit_desc.bmControls = 
        __parse_bmControls(driver, start+12+processing_unit_desc.bNrInPins, 0,
        &processing_unit_desc.bControlSize, m);
    processing_unit_desc.iProcessing = *(start+12+processing_unit_desc.bNrInPins+
        processing_unit_desc.bControlSize);
    parse_process_specific_descriptor(driver, &processing_unit_desc, 
        processing_unit_desc.wProcessType, start+13+processing_unit_desc.bNrInPins+
        processing_unit_desc.bControlSize, m);
    processing_unit->processing_unit_desc = processing_unit_desc;
    __ADD_UNIT__(ac_interface, processing_unit, PROCESSING_UNIT, processing_unit);

    return __RET_S__;
}

static int extension_unit_build_routine(AudioDriver* driver, uint8_t* start,
    ACInterface* ac_interface, MemoryService_C* m){
    __IF_RET_NEG__(__UNIT_COUNT__(ac_interface, EXTENSION_UNIT));
    __ALLOC_KERNEL_MEM_S__(m, struct ExtensionUnit, extension_unit);
    ExtensionUnitDescriptor extension_unit_descriptor;
    __parse_common_unit_descriptor_part(driver, start, &extension_unit_descriptor.bLength,
        &extension_unit_descriptor.bDescriptorType, &extension_unit_descriptor.bDescriptorSubtype,
        &extension_unit_descriptor.bUnitID);
    extension_unit_descriptor.wExtensionCode = *(start+4);
    extension_unit_descriptor.bNrInPins = *(start+6);
    __parse_baSourceID(driver, start+7, extension_unit_descriptor.bNrInPins,
        m, &extension_unit_descriptor.baSourceID);
    __parse_cluster(driver, &extension_unit_descriptor.cluster,
        start, 7+extension_unit_descriptor.bNrInPins);
    extension_unit_descriptor.bControlSize = *(start+11+extension_unit_descriptor.bNrInPins);
    extension_unit_descriptor.bmControls = 
        __parse_bmControls(driver, start+12+extension_unit_descriptor.bNrInPins, 0,
        &extension_unit_descriptor.bControlSize, m);
    extension_unit_descriptor.iExtension = *(start+12+extension_unit_descriptor.bNrInPins+
        extension_unit_descriptor.bControlSize);
    extension_unit->extension_unit_desc = extension_unit_descriptor;
    __ADD_UNIT__(ac_interface, extension_unit, EXTENSION_UNIT, extension_unit);

    return __RET_S__;
}

static int associated_interface_build_routine(AudioDriver* driver, uint8_t* start,
    ACInterface* ac_interface, MemoryService_C* m){
    AssociatedInterfaceDescriptor associated_interface_descriptor;
    __parse_common_unit_descriptor_part(driver, start, &associated_interface_descriptor.bLength,
        &associated_interface_descriptor.bDescriptorType, &associated_interface_descriptor.bDescriptorSubtype,
        &associated_interface_descriptor.bInterfaceNr);
    // unit | terminal listed here again ?
}

static uint8_t* __parse_baInterfaceNumbers(AudioDriver* driver, uint8_t* start,
    MemoryService_C* m){
    uint8_t bInCollection = *(start + 7);
    __ALLOC_KERNEL_MEM_T__(m, uint8_t, baInterfaces, bInCollection);
    __FOR_RANGE__(i, int, 0, bInCollection){
        *(baInterfaces + i) = *(start + 8 + i);
    }

    return baInterfaces;
}

static void __default_ac_interface(AudioDriver* driver, ACInterface* ac_interface){
    __FOR_RANGE__(i, int, 0, __MAX_UNITS__){
        ac_interface->in_terminal[i] = 0;
        ac_interface->out_terminal[i] = 0;
        ac_interface->mixer_unit[i] = 0;
        ac_interface->selector_unit[i] = 0;
        ac_interface->feature_unit[i] = 0;
        ac_interface->processing_unit[i] = 0;
        ac_interface->extension_unit[i] = 0;
        ac_interface->associated_interface[i] = 0;
        ac_interface->unit_count_map[i] = 0;
    }
}

static ACInterface* ac_routine(AudioDriver* driver, uint8_t* start,
    uint8_t** end, MemoryService_C* m, Alternate_Interface* alt_interface){
  AudioControlDescriptor audio_control_descriptor;
  audio_control_descriptor = *((AudioControlDescriptor*)start);
  uint8_t* baInterfaces = __parse_baInterfaceNumbers(driver, start, m);
  audio_control_descriptor.baInterfaceNr = baInterfaces;
  __ALLOC_KERNEL_MEM_S__(m, ACInterface, ac_interface);
  ac_interface->ac_desc = audio_control_descriptor;
  *end = start + (audio_control_descriptor.wTotalLength);
  __default_ac_interface(driver, ac_interface);
  alt_interface->class_specific = ac_interface;
  
  return ac_interface;
}

static void __set_class_specific_interface(AudioDriver* driver, Alternate_Interface* 
    alt_interface, void* audio_specific_interface){
    alt_interface->class_specific = audio_specific_interface;
}

static int __update_alt_interface(AudioDriver* driver, Alternate_Interface** alt_interface){
    if(__NOT_NULL__((*alt_interface)->next)){
        *alt_interface = (*alt_interface)->next;
        return __RET_S__;
    }
    return __RET_E__;
}

static int handle_ac_routine(AudioDriver* driver, uint8_t** start, 
  MemoryService_C* m, uint8_t minor){
  AudioDev audio_dev = driver->dev[minor];
  uint8_t* end = (*start) + sizeof(InterfaceDescriptor) + 1;
  Alternate_Interface* alt_interface = audio_dev.audio_control_interface->
    alternate_interfaces;
  ACInterface* ac_interface;

  while(*start < end){
    if(__is_ac_interface(driver, *start)){
        ac_interface = ac_routine(driver, *start, &end, m, alt_interface);
        alt_interface = alt_interface->next;
    }
    else if(__is_input_terminal(driver, *start)){
      __IF_RET_NEG__(__IS_NEG_ONE__(input_build_routine(driver, *start, ac_interface, m)));
    }
    else if(__is_output_terminal(driver, *start)) {
      __IF_RET_NEG__(__IS_NEG_ONE__(output_build_routine(driver, *start, ac_interface, m)));
    }
    else if(__is_mixer(driver, *start)){
      __IF_RET_NEG__(__IS_NEG_ONE__(mixer_build_routine(driver, *start, m, ac_interface)));
    }
    else if(__is_selector(driver, *start)){
      __IF_RET_NEG__(__IS_NEG_ONE__(selector_build_routine(driver, *start, ac_interface, m)));
    }
    else if(__is_feature(driver, *start)){
      __IF_RET_NEG__(__IS_NEG_ONE__(feature_build_routine(driver, *start, ac_interface, m)));
    }
    else if(__is_processing_unit(driver, *start)){
      __IF_RET_NEG__(__IS_NEG_ONE__(processing_unit_build_routine(driver, *start,
        ac_interface, m)));
    }
    else if(__is_extension_unit(driver, *start)){
      __IF_RET_NEG__(__IS_NEG_ONE__(extension_unit_build_routine(driver, *start,
        ac_interface, m)));
    }
    *start += *(*start);
  }

  return __RET_S__;
}

static ASInterface* as_routine(AudioDriver* driver, uint8_t* start,
    MemoryService_C* m, Alternate_Interface* alt_itf){
  
  __ALLOC_KERNEL_MEM_S__(m, ASInterface, as_interface);
  as_interface->as_desc = *((struct AudioStreamingDescriptor*)start);
  as_interface->bPitchEnable = 0;
  as_interface->current_freq = (SampleFrequency){0, 0};
  as_interface->format_type = 0;
  __set_class_specific_interface(driver, alt_itf, as_interface);

  return as_interface;
}

static int handle_as_routine(AudioDriver* driver, uint8_t** start, 
  uint8_t* end, MemoryService_C* m, uint8_t minor){
  AudioDev audio_dev = driver->dev[minor];
  Interface** audio_streaming_interfaces = audio_dev.audio_streaming_interfaces;
  Alternate_Interface* alt_itf = 0;
  ASInterface* as_interface;
  uint8_t as_num = 1, as_endpoint_num = 0;
  
  while(*start < end){
    if(*((*start) + 1) == INTERFACE){
        as_endpoint_num = 0;
        if(__IS_NULL__(alt_itf)){
            alt_itf = audio_streaming_interfaces[0]->alternate_interfaces;
        }
        else if(__NOT_NULL__(alt_itf) && __IS_NEG_ONE__(__update_alt_interface(driver, &alt_itf))){
            alt_itf = audio_streaming_interfaces[as_num]->alternate_interfaces;
            as_num++;
        }
    }
    if(__is_as_interface(driver, *start)){
        as_interface = as_routine(driver, *start, m, alt_itf);
    }
    else if(__is_format_type(driver, *start)){
        handle_format_routine(driver, *start, m, as_interface);
    }
    else if(__is_as_endpoint(driver, *start)){
        handle_as_endpoint_routine(driver, m, *start, alt_itf, as_endpoint_num);
        as_endpoint_num++;
    }
    *start += *(*start);
  }
  return __RET_S__;
}

static int handle_as_endpoint_routine(AudioDriver* driver, 
    MemoryService_C* m, uint8_t* start, Alternate_Interface* alt_itf, 
    uint8_t endpoint_num){
  __ALLOC_KERNEL_MEM_S__(m, ASEndpoint, as_endpoint);
  __TYPE_CAST__(struct AudioStreamingIsoEndpointDescriptor*,
    iso_endpoint, start);
  as_endpoint->as_iso_endpoint = *iso_endpoint;
  alt_itf->endpoints[endpoint_num]->class_specific = as_endpoint;

  return __RET_S__;
}

static struct ContinuousSamplingFrequency* __parse_contig_sample_freq(AudioDriver* driver, uint8_t* start,
    MemoryService_C* m){
    __TYPE_CAST__(struct ContinuousSamplingFrequency*, contig, start);
    __ALLOC_KERNEL_MEM_S__(m, struct ContinuousSamplingFrequency, contig_sample);
    *contig_sample = *contig;
    return contig_sample;
}

static struct DiscreteNumberSamplingFrequencies* __parse_discrete_sample_freq(AudioDriver* driver, uint8_t* start, 
    uint8_t discrete_sample_num, MemoryService_C* m){
    __ALLOC_KERNEL_MEM_T__(m, struct SampleFrequency, sample_freq,
        discrete_sample_num);
    __ALLOC_KERNEL_MEM_S__(m, struct DiscreteNumberSamplingFrequencies, discrete_freq);
    __FOR_RANGE__(i, int, 0, discrete_sample_num){
        __TYPE_CAST__(struct SampleFrequency*, samFreq, start);
        *(sample_freq+i) = *samFreq;
        start += 3;
    }

    discrete_freq->tSam = sample_freq;

    return discrete_freq;
}

static int handle_format_type_1(AudioDriver* driver, uint8_t* start,
    MemoryService_C* m, ASInterface* as_interface){
    uint8_t* pos = start + 8;
    void* samFreq = 0;
    __ALLOC_KERNEL_MEM_S__(m, struct Type1_FormatTypeDescriptor, type1_descriptor);
    __TYPE_CAST__(struct Type1_FormatTypeDescriptor*, type1, start);
    if(type1->bSamFreqType == 0){
        samFreq = __parse_contig_sample_freq(driver, pos, m);
    }
    else if((type1->bSamFreqType) >= 1 && (type1->bSamFreqType <= 255)){
        samFreq = __parse_discrete_sample_freq(driver, pos, type1->bSamFreqType, m);
    }
    if(__IS_NULL__(samFreq)) {
        __FREE_KERNEL_MEM__(m, type1_descriptor);
        return __RET_N__;
    }
    *type1_descriptor = *type1;
    type1_descriptor->samFreq = samFreq;
    __ALLOC_KERNEL_MEM_S__(m, struct FormatType, format_type);
    format_type->format = FORMAT_TYPE_1;
    format_type->type_descriptor = type1_descriptor;
    as_interface->format_type = format_type;
    return __RET_S__;
}

static int handle_format_type_2(AudioDriver* driver){
    return 0;
}

static int handle_format_type_3(AudioDriver* driver){
    return 0;
}

static int handle_format_routine(AudioDriver* driver, uint8_t* start, MemoryService_C* m,
    ASInterface* as_interface){
    switch(*(start+3)){
        case FORMAT_TYPE_1 : return handle_format_type_1(driver, start, m, as_interface);
        case FORMAT_TYPE_2 : return handle_format_type_2(driver);
        case FORMAT_TYPE_3 : return handle_format_type_3(driver);
        default : return __RET_N__;
    }
}

static void audio_device_routine(AudioDriver* driver, uint8_t* start,
    uint8_t* end, MemoryService_C* mem_service, uint8_t minor){
    while(start < end){
        if(*(start+6) == AUDIO_CONTROL){
            handle_ac_routine(driver, &start, mem_service, minor);
        }
        else if(*(start+6) == AUDIO_STREAMING){
            handle_as_routine(driver, &start, end, mem_service, minor);
        }
    }
}

// the default is always low/zero bandwith setting
// change to access iso endpoints
static void switch_if_zero_bandwidth(AudioDriver* driver, AudioDev* audio_dev){
    uint8_t alt_settings;
    uint8_t bSetting;
    Alternate_Interface* alt_itf;
    Interface** streaming_interfaces = audio_dev->audio_streaming_interfaces;
    UsbDev* dev = audio_dev->usb_dev;
    __FOR_RANGE__(i, int, 0, audio_dev->audio_streaming_interfaces_num){
        alt_settings = dev->__get_alternate_settings(dev, 
            streaming_interfaces[i]);
        if(alt_settings == 1) continue;
        alt_itf = streaming_interfaces[i]->alternate_interfaces->next;
        bSetting = alt_itf->alternate_interface_desc.bAlternateSetting;
        dev->request_switch_alternate_setting(dev, streaming_interfaces[i],
            bSetting);
    }
}

static void set_default_frequency(AudioDriver* driver, AudioDev* audio_dev){
    Interface** streaming_interfaces = audio_dev->audio_streaming_interfaces;
    uint8_t streaming_interfaces_num = audio_dev->audio_streaming_interfaces_num;
    SampleFrequency sample_frequencies [] = {__8000_HZ__, __16000_HZ__, __24000_HZ__, 
        __32000_HZ__, __44100_HZ__, __48000_HZ__, {}};
    __FOR_RANGE__(i, int, 0, streaming_interfaces_num){
        ASInterface* as_interface = (ASInterface*)streaming_interfaces[i]->active_interface->class_specific;
        __FOR_RANGE_COND__(j, int, 0, sample_frequencies[j].sampleFreq_lo != 0 || 
            sample_frequencies[j].sampleFreq_hi != 0, j++){
            if(as_interface->format_type->format == FORMAT_TYPE_1){
                struct Type1_FormatTypeDescriptor* type1 = (struct Type1_FormatTypeDescriptor*)as_interface->format_type->type_descriptor;
                struct DiscreteNumberSamplingFrequencies* discrete = (struct DiscreteNumberSamplingFrequencies*)type1->samFreq;
                SampleFrequency* sample_freq = discrete->tSam;
                __FOR_RANGE__(t, int, 0, type1->bSamFreqType){
                    if((sample_freq+t)->sampleFreq_lo == sample_frequencies[j].sampleFreq_lo &&
                        (sample_freq+t)->sampleFreq_hi == sample_frequencies[j].sampleFreq_hi){
                        set_sampling_frequency(driver, audio_dev, streaming_interfaces[i], *sample_freq);
                        goto label_cont;
                    }
                }
            }
        }
        label_cont:
    }
}

static int configure_audio_device(AudioDriver* driver){
    __FOR_RANGE__(i, int, 0, MAX_DEVICES_PER_USB_DRIVER){
        __IF_CONTINUE__(__IS_ZERO__(driver->audio_map[i]));
        // request again the configuration of the device and traverse now the device specific
        // interfaces/endpoints
        UsbDev* dev = driver->dev[i].usb_dev;
        MemoryService_C* mem_service = __DEV_MEMORY(dev);
        uint8_t* map_io_buffer = __MAP_IO_KERNEL__(mem_service, uint8_t, PAGE_SIZE);
        uint16_t map_io_offset = 0, total_len;
        uint8_t desc_len, active_config_val = dev->active_config->config_desc.bConfigurationValue;
        uint8_t *start, *end;
        __DEV_IO__ASSIGN__(map_io_buffer, map_io_offset, ConfigurationDescriptor, config_descriptor);
        __DEV_IO_ASSIGN_SIZE__(map_io_buffer, map_io_offset, uint8_t, config_buffer, 
            sizeof(uint8_t) * AUDIO_CONFIGURATION_BUFFER);

        __IF_RET_NEG__(__IS_NEG_ONE__(__STRUCT_CALL__(dev, 
            process_configuration_descriptor, config_descriptor, active_config_val, 4)));
        __STRUCT_CALL__(dev, __retrieve_length_from_config, &total_len, 
            &desc_len, config_descriptor);
        __IF_CONTINUE__(total_len > AUDIO_CONFIGURATION_BUFFER); // buffer might be to small for audio configuration !
        __IF_RET_NEG__(__IS_NEG_ONE__(__STRUCT_CALL__(dev, 
            process_whole_configuration, config_buffer, active_config_val, total_len)));

        __STRUCT_CALL__(dev, __set_start, config_buffer, &desc_len, &start);
        __STRUCT_CALL__(dev, __set_end, config_buffer, &total_len, &end);
        audio_device_routine(driver, start, end, mem_service, i);
        switch_if_zero_bandwidth(driver, driver->dev + i);
        set_default_frequency(driver, driver->dev + i);
    }

    return __RET_S__;
}

// specific mute control for a channel
static void feature_control_first_form(AudioDriver* driver, uint8_t control, 
    uint8_t request_form, UsbDev* dev, UsbDeviceRequest* device_req, uint8_t bRequest,
    uint8_t channel_number, uint8_t unit_id, uint8_t audio_interface,
    void* data, uint8_t len, callback_function callback, uint8_t flags){
    __IF_ELSE__(request_form == __SET_REQUEST__, 
    __request_feature_set(driver, dev, device_req, control, channel_number,
        unit_id, audio_interface, data, len, callback, flags),
    __request_feature_get(driver, dev, device_req, bRequest, control,
        channel_number, unit_id, audio_interface, data, len,
        callback, flags));
}

// entered if channel number is set to 0xFF
static void feature_control_second_form(AudioDriver* driver, uint8_t control, 
    uint8_t request_form, UsbDev* dev, UsbDeviceRequest* device_req, 
    uint8_t bRequest, uint8_t feature_unit_id, uint8_t audio_interface, 
    void* data, uint8_t len, callback_function callback, uint8_t flags){
    __IF_ELSE__(request_form == __SET_REQUEST__,
        __request_feature_set(driver, dev, device_req, MUTE_CONTROL, 0xFF, 
            feature_unit_id, audio_interface, data, len, callback, flags),
        __request_feature_get(driver, dev, device_req, bRequest, MUTE_CONTROL, 0xFF, 
            feature_unit_id, audio_interface, data, len, callback, flags));
}

static SpatialLocations __get_loc_by_channel_number(AudioDriver* driver, 
    ClusterDescriptor cluster, uint8_t channel_number){
    uint8_t channel_count = 1;

    __LOCATION_OR_INC__(driver, cluster, SPATIAL_LOC_L, channel_number, channel_count);
    __LOCATION_OR_INC__(driver, cluster, SPATIAL_LOC_R, channel_number, channel_count);
    __LOCATION_OR_INC__(driver, cluster, SPATIAL_LOC_C, channel_number, channel_count);
    __LOCATION_OR_INC__(driver, cluster, SPATIAL_LOC_LFE, channel_number, channel_count);
    __LOCATION_OR_INC__(driver, cluster, SPATIAL_LOC_LS, channel_number, channel_count);
    __LOCATION_OR_INC__(driver, cluster, SPATIAL_LOC_RS, channel_number, channel_count);
    __LOCATION_OR_INC__(driver, cluster, SPATIAL_LOC_LC, channel_number, channel_count);
    __LOCATION_OR_INC__(driver, cluster, SPATIAL_LOC_RC, channel_number, channel_count);
    __LOCATION_OR_INC__(driver, cluster, SPATIAL_LOC_S, channel_number, channel_count);
    __LOCATION_OR_INC__(driver, cluster, SPATIAL_LOC_SL, channel_number, channel_count);
    __LOCATION_OR_INC__(driver, cluster, SPATIAL_LOC_SR, channel_number, channel_count);
    __LOCATION_OR_INC__(driver, cluster, SPATIAL_LOC_T, channel_number, channel_count);

    return 0;
}

// does not support master control as input channel !
static int16_t __get_channel_number_by_loc(AudioDriver* driver, ClusterDescriptor cluster,
    SpatialLocations loc){
    uint8_t channel_number = 0;
    __CHANNEL_OR_INC__(driver, cluster, loc, SPATIAL_LOC_L, channel_number);
    __CHANNEL_OR_INC__(driver, cluster, loc, SPATIAL_LOC_R, channel_number);
    __CHANNEL_OR_INC__(driver, cluster, loc, SPATIAL_LOC_C, channel_number);
    __CHANNEL_OR_INC__(driver, cluster, loc, SPATIAL_LOC_LFE, channel_number);
    __CHANNEL_OR_INC__(driver, cluster, loc, SPATIAL_LOC_LS, channel_number);
    __CHANNEL_OR_INC__(driver, cluster, loc, SPATIAL_LOC_RS, channel_number);
    __CHANNEL_OR_INC__(driver, cluster, loc, SPATIAL_LOC_LC, channel_number);
    __CHANNEL_OR_INC__(driver, cluster, loc, SPATIAL_LOC_RC, channel_number);
    __CHANNEL_OR_INC__(driver, cluster, loc, SPATIAL_LOC_S, channel_number);
    __CHANNEL_OR_INC__(driver, cluster, loc, SPATIAL_LOC_SL, channel_number);
    __CHANNEL_OR_INC__(driver, cluster, loc, SPATIAL_LOC_SR, channel_number);
    __CHANNEL_OR_INC__(driver, cluster, loc, SPATIAL_LOC_T, channel_number);
    return __RET_E__;
}



static int8_t feature_control_sub_routine_checks(AudioDriver* driver,
    AudioDev* audio_dev, uint8_t request_form, uint8_t channel_number, 
    uint8_t feature_unit_id, uint8_t audio_interface){
    __IF_RET_NEG__(!__valid_request_form(driver, request_form));
    __IF_RET_NEG__(!__channel_present(driver, audio_dev, channel_number) && 
        channel_number != 0xFF);
    __IF_RET_NEG__(!__feature_unit_present(driver, audio_dev, feature_unit_id));
    __IF_RET_NEG__(!__audio_interface_present(driver, audio_dev, audio_interface));
    return __RET_S__;
}

static int8_t audio_mute_control(AudioDriver* driver, AudioDev* audio_dev,
    uint8_t request_form, uint8_t bRequest, uint8_t channel_number, 
    uint8_t audio_interface, uint8_t feature_unit_id, void* data, uint8_t len, 
    callback_function callback, uint8_t flags){
    
    UsbDev* dev = audio_dev->usb_dev;
    __IF_RET_NEG__(__IS_NEG_ONE__(feature_control_sub_routine_checks(driver,
        audio_dev, request_form, channel_number, feature_unit_id, audio_interface)));

    UsbDeviceRequest* device_req = dev->get_free_device_request(dev);
    __IF_IS_NULL__(device_req);

    __IF_ELSE__(channel_number == 0xFF, feature_control_second_form(driver, MUTE_CONTROL,
        request_form, dev, device_req, bRequest, feature_unit_id, audio_interface,
            data, len, callback, flags),
        feature_control_first_form(driver, MUTE_CONTROL, request_form, dev, device_req,
            bRequest, channel_number, feature_unit_id, audio_interface, data,
            len, callback, flags));

    return __RET_S__;
}

// + 127.9961 dB (0x7FFF) : -127.9961 dB (0x8001) , 1/256 dB (0x0001)step
// CUR : 0x8000 represents silence
// RES : 1/256 dB (0x0001) : +127.9961 dB (0x7FFF)
static int8_t audio_volume_control(AudioDriver* driver, AudioDev* audio_dev,
    uint8_t request_form, uint8_t bRequest, uint8_t channel_number,
    uint8_t audio_interface, uint8_t feature_unit_id, void* data, uint8_t len,
    callback_function callback, uint8_t flags){
    UsbDev* dev = audio_dev->usb_dev;

    __IF_RET_NEG__(__IS_NEG_ONE__(feature_control_sub_routine_checks(driver,
        audio_dev, request_form, channel_number, feature_unit_id, audio_interface)));
    UsbDeviceRequest* device_req = dev->get_free_device_request(dev);
    __IF_IS_NULL__(device_req);

    __IF_ELSE__(channel_number == 0xFF, feature_control_second_form(driver, VOLUME_CONTROL,
        request_form, dev, device_req, bRequest, feature_unit_id, audio_interface,
            data, len, callback, flags),
        feature_control_first_form(driver, VOLUME_CONTROL, request_form, dev, device_req,
            bRequest, channel_number, feature_unit_id, audio_interface, data,
            len, callback, flags));

    return __RET_S__;
}

// range from +127.9961 dB (0x7FFF) : -127.9961 dB (0x8001)
// 0x8000 for cur req : silence request
// res reqest range : 0x0001 - 0x7FFF
// if request is not supported the pipe must stall !!!
// i don't know exactly but the icn and ocn must be a construct of both 
// the cluster number and the channel number !
// so for now let's just assume that both bytes are just constructed out of 
// the channel numbers and exclude the channel cluster number which for now 
// are not needed !
// second for is executed when both input and output channels are 0xFF
static int8_t audio_mixer_control(AudioDriver* driver, AudioDev* audio_dev,
    uint8_t request_form, uint8_t bRequest, uint8_t icn, uint8_t ocn,
    uint8_t mixer_unit_id, uint8_t audio_interface, void* data, uint8_t len,
    callback_function callback){
    // check if icn and ocn are valid channels in the audio device 
    UsbDev* dev = audio_dev->usb_dev;

    UsbDeviceRequest* device_req = dev->get_free_device_request(dev);
    __IF_RET_NEG__(__IS_NULL__(device_req));
    __IF_ELSE__(request_form == __SET_REQUEST__,
        __request_mixer_set(driver, dev, device_req, icn, ocn, mixer_unit_id, audio_interface,
        data, len, callback, 0),
        __request_mixer_get(driver, dev, device_req, bRequest, icn, ocn, mixer_unit_id,
        audio_interface, data, len, callback, 0));
    return __RET_S__;
}

static void mixer_callback(UsbDev* dev, uint32_t status, void* data){
    // mixer transfer checks
}

static int8_t mixer_first_form_build(AudioDriver* driver, MemoryService_C* mem_service,
    uint16_t mixer_value, AudioDev* audio_dev, uint8_t request_form, uint8_t bRequest,
    uint8_t icn, uint8_t ocn, uint8_t audio_interface, uint8_t mixer_unit_id){
    MixerControlParameterBlockFirstForm* mixer_control_block = 
        __MAP_IO_KERNEL__(mem_service, MixerControlParameterBlockFirstForm, 
        sizeof(MixerControlParameterBlockFirstForm));
    __build_mixer_control_first_form(driver, mixer_control_block, mixer_value);
    return audio_mixer_control(driver, audio_dev, request_form, bRequest, 
        icn, ocn, mixer_unit_id, audio_interface, mixer_control_block, 
        sizeof(MixerControlParameterBlockFirstForm), &mixer_callback);
}

static int8_t mixer_second_form_build(AudioDriver* driver, MemoryService_C* mem_service,
    AudioDev* audio_dev, uint8_t request_form, uint8_t bRequest, uint8_t icn,
    uint8_t ocn, uint8_t mixer_unit_id, uint8_t audio_interface){
    MixerControlParameterBlockSecondForm* mixer_control_block = 
        __MAP_IO_KERNEL__(mem_service, MixerControlParameterBlockSecondForm, 
        sizeof(MixerControlParameterBlockSecondForm));
    __build_mixer_control_second_form(driver, mixer_control_block, 0, 0);
    return audio_mixer_control(driver, audio_dev, request_form, bRequest, icn, ocn,
        mixer_unit_id, audio_interface, mixer_control_block, sizeof(MixerControlParameterBlockSecondForm),
        &mixer_callback);
}

static int8_t set_mixer_unit_control(AudioDriver* driver, AudioDev* audio_dev,
    uint16_t mixer_value, uint8_t icn, uint8_t ocn, uint8_t mixer_unit_id,
    uint8_t audio_interface){
    MemoryService_C* mem_service = __DEV_MEMORY(audio_dev->usb_dev);
    __IF_ELSE__(((icn == 0xFF) && (ocn == 0xFF)), return mixer_second_form_build(driver,
        mem_service, audio_dev, __SET_REQUEST__, SET_CUR, icn, ocn, mixer_unit_id,
            audio_interface),
        return mixer_first_form_build(driver, mem_service, mixer_value, audio_dev, __SET_REQUEST__,
            SET_CUR, icn, ocn, audio_interface, mixer_unit_id));
}


// this request is via the inserted callback registered by the appropriate driver
static int8_t get_mixer_unit_control(AudioDriver* driver, AudioDev* audio_dev){
    MemoryService_C* mem_service = __DEV_MEMORY(audio_dev->usb_dev);
    MixerControlParameterBlockFirstForm* mixer_control_block = 
        __MAP_IO_KERNEL__(mem_service, MixerControlParameterBlockFirstForm, 
        sizeof(MixerControlParameterBlockFirstForm));
    
}

// the res request should not be supported 
static int8_t audio_selector_control(AudioDriver* driver, AudioDev* audio_dev,
    uint8_t request_form, uint8_t bRequest, uint8_t selector_unit_id, uint8_t audio_interface, 
    void* data, uint8_t len, callback_function callback){
    UsbDev* dev = audio_dev->usb_dev;

    UsbDeviceRequest* device_req = dev->get_free_device_request(dev);
    __IF_RET_NEG__(__IS_NULL__(device_req));
    __IF_ELSE__(request_form == __SET_REQUEST__,
        __request_selector_set(driver, dev, device_req, selector_unit_id, audio_interface,
        data, len, callback, 0),
        __request_selector_get(driver, dev, device_req, bRequest, selector_unit_id,
        audio_interface, data, len, callback, 0));
    return __RET_S__;
}


static void selector_control_callback(UsbDev* dev, uint32_t status, void* data){
    // transfer checks
    __IF_RET__(status & E_TRANSFER);
    // handling
}

static int8_t selector_set_cur(AudioDriver* driver, AudioDev* audio_dev, 
    uint8_t bSelector, uint8_t selector_unit_id, uint8_t audio_interface){
    MemoryService_C* mem_service = __DEV_MEMORY(audio_dev->usb_dev);
    SelectorControlParameterBlock* selector_block = __MAP_IO_KERNEL__(mem_service,
        SelectorControlParameterBlock, sizeof(SelectorControlParameterBlock));
    __build_selector_control_block(driver, selector_block, bSelector);
    return audio_selector_control(driver, audio_dev, __SET_REQUEST__,
        SET_CUR, selector_unit_id, audio_interface, selector_block, 
        sizeof(SelectorControlParameterBlock), &selector_control_callback);
}

static int8_t audio_processing_control(AudioDriver* driver, AudioDev* audio_dev,
    uint8_t request_form, uint8_t bRequest, uint8_t control_selector,
    uint8_t processing_unit_id, uint8_t audio_interface,
    void* data, uint8_t len, callback_function callback){
    
    __IF_RET_NEG__(!__processing_unit_present(driver, audio_dev, 
        processing_unit_id));

    UsbDev* dev = audio_dev->usb_dev;
    UsbDeviceRequest* device_req = dev->get_free_device_request(dev);
    __IF_RET_NEG__(__IS_NULL__(device_req));
    __IF_ELSE__(request_form == __SET_REQUEST__,
        __request_processing_set(driver, dev, device_req, control_selector,
            processing_unit_id, audio_interface, data, len, callback,
            0),
        __request_processing_get(driver, dev, device_req, bRequest,
            control_selector, processing_unit_id, audio_interface, data,
            len, callback, 0));

    return __RET_S__;
}

static int8_t audio_extension_control(AudioDriver* driver, AudioDev* audio_dev,
    uint8_t request_form, uint8_t bRequest, uint8_t control_selector,
    uint8_t extension_unit_id, uint8_t audio_interface, void* data, uint8_t len,
    callback_function callback){

    __IF_RET_NEG__(!__extension_unit_present(driver, audio_dev, extension_unit_id));
    UsbDev* dev = audio_dev->usb_dev;
    UsbDeviceRequest* device_req = dev->get_free_device_request(dev);
    __IF_RET_NEG__(__IS_NULL__(device_req));
    __IF_ELSE__(request_form == __SET_REQUEST__, 
        __request_extension_set(driver, dev, device_req, control_selector,
            extension_unit_id, audio_interface, data, len, callback, 0), 
        __request_extension_get(driver, dev, device_req, bRequest,
            control_selector, extension_unit_id, audio_interface,
            data, len, callback, 0));
    return __RET_S__;
}

static void processing_callback(UsbDev* dev, uint32_t status, void* data){
    __IF_RET__(status & E_TRANSFER);
}

// set cur only
// control selector high byte & 0 low byte
// processing_unit high byte & interface number low byte
static int8_t audio_enable_processing_control(AudioDriver* driver, 
    AudioDev* audio_dev, uint8_t request_form, uint8_t bRequest, 
    uint8_t processing_unit_id, uint8_t audio_interface, void* data, 
    uint8_t len, callback_function callback){

    return audio_processing_control(driver, audio_dev, request_form, 
        bRequest, UD_ENABLE_CONTROL, processing_unit_id, audio_interface,
        data, len, callback);
}

static int8_t audio_endpoint_present(AudioDriver* driver, AudioDev* audio_dev,
    uint8_t endpoint_number){
    UsbDev* dev = audio_dev->usb_dev;
    uint8_t audio_streaming_num = audio_dev->audio_streaming_interfaces_num;
    Interface** audio_streaming_itfs = audio_dev->audio_streaming_interfaces;
    __FOR_RANGE__(i, int, 0, audio_streaming_num){
        Interface* audio_streaming_itf = audio_streaming_itfs[i];
        Alternate_Interface* alt_itf = audio_streaming_itf->active_interface;
        __IF_CONTINUE__(__IS_ZERO__(alt_itf->alternate_interface_desc.bNumEndpoints));
        uint8_t endpoint_num = alt_itf->alternate_interface_desc.bNumEndpoints;
        Endpoint** endpoints = alt_itf->endpoints;
        __FOR_RANGE__(j, int, 0, endpoint_num){
            if(dev->__endpoint_number(dev, endpoints[j]) == endpoint_number){
                return __RET_S__;
            }
        }
    }
    return __RET_E__;
}

static int8_t contains_audio_streaming_interface(AudioDriver* driver, AudioDev* audio_dev,
    Interface* as_interface){
    __FOR_RANGE__(i, int, 0, audio_dev->audio_streaming_interfaces_num){
        __IF_RET_SELF__(audio_dev->audio_streaming_interfaces[i] == as_interface,
            __RET_S__);
    }
    return __RET_E__;
}

static int8_t support_sampling_freq_control(AudioDriver* driver, AudioDev* audio_dev, 
    Interface* as_interface){
    __IF_RET_NEG__(__IS_NEG_ONE__(contains_audio_streaming_interface(driver,
        audio_dev, as_interface)));
    Endpoint* endpoint = audio_dev->usb_dev->__get_first_endpoint(audio_dev->usb_dev,
        as_interface->active_interface);
    __IF_RET_NEG__(__IS_NULL__(endpoint));
    ASEndpoint* as_endpoint = (ASEndpoint*)endpoint->class_specific;
    __IF_EXT__((__IS_ZERO__((as_endpoint->as_iso_endpoint.bmAttributes & BIT_SAMPLING_FREQ))), -1, 1);
}

static int8_t support_pitch_control(AudioDriver* driver, AudioDev* audio_dev, 
    Interface* as_interface){
    __IF_RET_NEG__(__IS_NEG_ONE__(contains_audio_streaming_interface(driver,
        audio_dev, as_interface)));
    __IF_RET_NEG__(__IS_NEG_ONE__(audio_dev->usb_dev->__has_endpoints(audio_dev->usb_dev,
        as_interface->active_interface)));
    Endpoint* endpoint = audio_dev->usb_dev->__get_first_endpoint(audio_dev->usb_dev,
        as_interface->active_interface);
    __IF_RET_NEG__(__IS_NULL__(endpoint));
    ASEndpoint* as_endpoint = (ASEndpoint*)endpoint->class_specific;
    __IF_EXT__((__IS_ZERO__((as_endpoint->as_iso_endpoint.bmAttributes & BIT_PITCH))), -1, 1);
}

static int8_t audio_endpoint_controls(AudioDriver* driver, AudioDev* audio_dev,
    uint8_t request_form, uint8_t bRequest, uint8_t control_selector, 
    Interface* as_interface, void* data, uint8_t len, 
    callback_function callback){
    UsbDev* dev = audio_dev->usb_dev;
    Endpoint* e;
    uint8_t endpoint_number;
    __IF_RET_NEG__(__IS_NULL__((e = dev->__get_first_endpoint(dev, 
        as_interface->active_interface))));
    UsbDeviceRequest* device_req = dev->get_free_device_request(dev);
    __IF_RET_NEG__(__IS_NULL__(device_req));
    endpoint_number = dev->__endpoint_number(dev, e);
    __IF_ELSE__(request_form == __SET_REQUEST__, 
        __request_endpoint_set(driver, dev, device_req, control_selector,
            endpoint_number, data, len, callback, CONTROL_INITIAL_STATE), 
        __request_endpoint_get(driver, dev, device_req, bRequest,
            control_selector, endpoint_number, data, len, callback, 
                CONTROL_INITIAL_STATE));
    return __RET_S__;
}

// callback mechanism has to be changed into (UsbDev* dev, Interface* itf, uint32_t status, void* data)
// for requests that have to change interface values this is pretty important
static void frequency_control_callback(UsbDev* dev, uint32_t status, void* data){
    __IF_RET__(status & E_TRANSFER);
    __TYPE_CAST__(struct SamplingFrequencyControlParameterBlock*, 
        samp_control_block, data);
}

static void pitch_control_callback(UsbDev* dev, uint32_t status, void* data){
    __IF_RET__(status & E_TRANSFER);
}

static int8_t sampling_frequency_control(AudioDriver* driver, 
    AudioDev* audio_dev, uint8_t request_form, uint8_t bRequest, 
    Interface* as_interface, struct SampleFrequency sample_freq){
    MemoryService_C* m = __DEV_MEMORY(audio_dev->usb_dev);
    __IF_RET_NEG__(__IS_NEG_ONE__(support_sampling_freq_control(driver, audio_dev, 
        as_interface)));
    __MAP_IO_KERNEL_S__(m, struct SamplingFrequencyControlParameterBlock,
        sam_freq_block);
    sam_freq_block->control_selector = SAMPLING_FREQ_CONTROL;
    sam_freq_block->wLength = 3;
    sam_freq_block->tSampleFreq = sample_freq;
    audio_endpoint_controls(driver, audio_dev, request_form,
        bRequest, SAMPLING_FREQ_CONTROL, as_interface, sam_freq_block,
        sizeof(struct SamplingFrequencyControlParameterBlock),
        &frequency_control_callback);
    __TYPE_CAST__(ASInterface*, class_specific_as, 
        as_interface->active_interface->class_specific);
    class_specific_as->current_freq = sample_freq;

    return __RET_S__;
}

static int8_t sampling_pitch_control(AudioDriver* driver, AudioDev* audio_dev,
    uint8_t request_form, uint8_t bRequest, Interface* as_interface, 
    uint8_t bPitchEnable){
    MemoryService_C* m = __DEV_MEMORY(audio_dev->usb_dev);
    __IF_RET_NEG__(__IS_NEG_ONE__(support_pitch_control(driver, audio_dev, 
        as_interface)));
    __MAP_IO_KERNEL_S__(m, struct PitchControlParameterBlock,
        pitch_control_block);
    pitch_control_block->control_selector = PITCH_CONTROL;
    pitch_control_block->wLength = 1;
    pitch_control_block->bPitchEnable = bPitchEnable;
    audio_endpoint_controls(driver, audio_dev, request_form, bRequest,
        PITCH_CONTROL, as_interface, pitch_control_block, 
        sizeof(struct PitchControlParameterBlock), &pitch_control_callback);
    __TYPE_CAST__(ASInterface*, class_specific_as, 
        as_interface->active_interface->class_specific);
    class_specific_as->bPitchEnable = bPitchEnable;

    return __RET_S__;
}

static int8_t set_sampling_frequency(AudioDriver* driver, AudioDev* audio_dev,
    Interface* as_interface, struct SampleFrequency sample_freq){
    return sampling_frequency_control(driver, audio_dev, __SET_REQUEST__,
        SET_CUR, as_interface, sample_freq);
}

static struct SampleFrequency get_sampling_frequency(AudioDriver* driver,
    AudioDev* audio_dev, Interface* as_interface){
    sampling_frequency_control(driver, audio_dev, __GET_REQUEST__,
        GET_CUR, as_interface, (struct SampleFrequency){});
}

static int8_t enable_processing_control(AudioDriver* driver, AudioDev* audio_dev,
    uint8_t processing_unit_id, uint8_t audio_interface){
    MemoryService_C* mem_service = __DEV_MEMORY(audio_dev->usb_dev);
    struct EnableProcessingControlParameterBlock* enable_processing_control_block = 
        __MAP_IO_KERNEL__(mem_service, struct EnableProcessingControlParameterBlock,
        sizeof(struct EnableProcessingControlParameterBlock));
    __build_enable_processing_control_block(driver, enable_processing_control_block,
        1);
    return audio_enable_processing_control(driver, audio_dev, __SET_REQUEST__,
        SET_CUR, processing_unit_id, audio_interface, enable_processing_control_block, 
        sizeof(struct EnableProcessingControlParameterBlock), &processing_callback);
}

static int8_t disable_processing_control(AudioDriver* driver, AudioDev* audio_dev,
    uint8_t processing_unit_id, uint8_t audio_interface){
    MemoryService_C* mem_service = __DEV_MEMORY(audio_dev->usb_dev);
    __ALLOC_KERNEL_MEM_S__(mem_service, struct EnableProcessingControlParameterBlock,
        enable_processing_control_block);
    __build_enable_processing_control_block(driver, enable_processing_control_block,
        0);
    return audio_enable_processing_control(driver, audio_dev, __SET_REQUEST__,
        SET_CUR, processing_unit_id, audio_interface, enable_processing_control_block,
        sizeof(struct EnableProcessingControlParameterBlock), &processing_callback);
}

static int8_t audio_mode_select_control(AudioDriver* driver, AudioDev* audio_dev,
    uint8_t request_form, uint8_t bRequest, uint8_t processing_unit_id,
    uint8_t audio_interface, void* data, uint8_t len){

    return audio_processing_control(driver, audio_dev, request_form,
        bRequest, UD_MODE_SELECT_CONTROL, processing_unit_id, audio_interface,
        data, len, &processing_callback);
}

static int8_t audio_spaciousness_control(AudioDriver* driver, AudioDev* audio_dev,
    uint8_t request_form, uint8_t bRequest, uint8_t processing_unit_id,
    uint8_t audio_interface, void* data, uint8_t len){

    return audio_processing_control(driver, audio_dev, request_form,
        bRequest, STEREO_SPACIOUSNESS_CONTROL, processing_unit_id,
        audio_interface, data, len, &processing_callback);
}

static int8_t audio_reverb_type_control(AudioDriver* driver, AudioDev* audio_dev,
    uint8_t request_form, uint8_t bRequest, uint8_t processing_unit_id,
    uint8_t audio_interface, void* data, uint8_t len){

}

static int8_t audio_reverb_level_control(AudioDriver* driver, AudioDev* audio_dev,
    uint8_t request_form, uint8_t bRequest, uint8_t processing_unit_id,
    uint8_t audio_interface, void* data, uint8_t len){
    
    return audio_processing_control(driver, audio_dev, request_form,
        bRequest, REVERB_LEVEL_CONTROL, processing_unit_id, audio_interface,
        data, len, &processing_callback);
}

static int8_t audio_chorus_level_control(AudioDriver* driver, AudioDev* audio_dev,
    uint8_t request_form, uint8_t bRequest, uint8_t processing_unit_id,
    uint8_t audio_interface, void* data, uint8_t len){
    
    return audio_processing_control(driver, audio_dev, request_form, bRequest,
        CHORUS_LEVEL_CONTROL, processing_unit_id, audio_interface, data,
        len, &processing_callback);
}

// all following request will just support the bRequest of SET_CUR allowing 
// only to request the current setting in the unit or terminal

// valid range from 0 to 255 % 
static int8_t audio_set_cur_spaciousness_control(AudioDriver* driver, AudioDev* audio_dev,
    uint8_t request_form, uint8_t bRequest, uint8_t processing_unit_id,
    uint8_t audio_interface, void* data, uint8_t len, uint8_t bSpaciousness){
    __IF_RET_NEG__(bSpaciousness > __SPACIOUSNESS_UPPER_BOUND__ || 
        bSpaciousness < __SPACIOUSNESS_LOWER_BOUND__);
    MemoryService_C* mem_service = __DEV_MEMORY(audio_dev->usb_dev);
    struct SpaciousnessControlParameterBlock* mode_select_block = 
    __MAP_IO_KERNEL__(mem_service, struct SpaciousnessControlParameterBlock, 
        sizeof(struct SpaciousnessControlParameterBlock));
    __build_spaciousness_control_block(driver, mode_select_block, bSpaciousness);
    return audio_spaciousness_control(driver, audio_dev, __SET_REQUEST__,
        SET_CUR, processing_unit_id, audio_interface, mode_select_block,
        sizeof(struct ModeSelectControlParameterBlock));
}

static enum ProcessingUnitProcessTypes retrieve_process_type(AudioDriver* driver,
    uint8_t* pos){
    uint8_t process_type_offset = 5, descriptor_type = 2, descriptor_subtype = 3;
    __IF_RET_SELF__(*(pos + descriptor_type) != CS_INTERFACE && 
        *(pos + descriptor_subtype) != PROCESSING_UNIT, PROCESS_UNDEFINED);
    return *((enum ProcessingUnitProcessTypes*)(pos + process_type_offset));
}

static int8_t supports_audio_mode(AudioDriver* driver, AudioDev* audio_dev,
    uint8_t processing_unit_id, uint8_t mode_value){
    // up_down_mix, dolby_prologic, 
    //__IF_RET_NEG__()
}

static int8_t set_audio_mode_attribute(AudioDriver* driver, AudioDev* audio_dev,
    uint8_t processing_unit_id, uint8_t audio_interface, uint8_t bMode){
    MemoryService_C* mem_service = __DEV_MEMORY(audio_dev->usb_dev);
    struct ModeSelectControlParameterBlock* mode_select_block = 
    __MAP_IO_KERNEL__(mem_service, struct ModeSelectControlParameterBlock, 
        sizeof(struct ModeSelectControlParameterBlock));
    __build_mode_select_control_block(driver, mode_select_block, bMode);
    return audio_mode_select_control(driver, audio_dev, __SET_REQUEST__,
        SET_CUR, processing_unit_id, audio_interface, mode_select_block,
        sizeof(struct ModeSelectControlParameterBlock));
}

static int8_t enable_extension_unit(AudioDriver* driver, AudioDev* audio_dev,
    uint8_t extension_unit_id, uint8_t audio_interface){
    MemoryService_C* mem_service = __DEV_MEMORY(audio_dev->usb_dev);
    struct EnableProcessingControlParameterBlock* enable_processing_block = 
    __MAP_IO_KERNEL__(mem_service, struct EnableProcessingControlParameterBlock, 
        sizeof(struct EnableProcessingControlParameterBlock));
    __build_enable_extension_processing_control_block(driver, enable_processing_block,
        1);
    return audio_extension_control(driver, audio_dev, __SET_REQUEST__,
        SET_CUR, XU_ENABLE_CONTROL, extension_unit_id, audio_interface,
        enable_processing_block, sizeof(struct EnableProcessingControlParameterBlock),
        &extension_callback);
}

static int8_t disable_extension_unit(AudioDriver* driver, AudioDev* audio_dev,
    uint8_t extension_unit_id, uint8_t audio_interface){
    MemoryService_C* mem_service = __DEV_MEMORY(audio_dev->usb_dev);
    struct EnableProcessingControlParameterBlock* enable_processing_block = 
    __MAP_IO_KERNEL__(mem_service, struct EnableProcessingControlParameterBlock, 
        sizeof(struct EnableProcessingControlParameterBlock));
    __build_enable_extension_processing_control_block(driver, enable_processing_block,
        0);
    return audio_extension_control(driver, audio_dev, __SET_REQUEST__,
        SET_CUR, XU_ENABLE_CONTROL, extension_unit_id, audio_interface,
        enable_processing_block, sizeof(struct EnableProcessingControlParameterBlock),
        &extension_callback);
}

// input terminal should only support get terminal copy protect control req
// output terminal should only support set terminal copy protect control req
static int8_t audio_copy_protect_control(AudioDriver* driver, AudioDev* audio_dev,
    uint8_t request_form, uint8_t bRequest, uint8_t channel_number,
    uint8_t audio_interface, uint8_t terminal_unit_id, void* data, uint8_t len,
    callback_function callback, uint8_t flags){

    UsbDev* dev = audio_dev->usb_dev;

    UsbDeviceRequest* device_req = dev->get_free_device_request(dev);
    __IF_RET_NEG__(__IS_NULL__(device_req));
    __IF_ELSE__(request_form == __SET_REQUEST__, 
        __request_terminal_set(driver, dev, device_req, COPY_PROTECT_CONTROL, 
        audio_interface, terminal_unit_id, data, len, callback, flags),
        __request_terminal_get(driver, dev, device_req, bRequest, COPY_PROTECT_CONTROL,
            audio_interface, terminal_unit_id, data, len, callback, flags));
    return __RET_S__;
}

static int8_t set_copy_protect(AudioDriver* driver, AudioDev* audio_dev,
    uint8_t output_terminal_id, CPL copy_protect_value, uint8_t channel_number,
    uint8_t audio_interface){
    __IF_RET_NEG__(__IS_NEG_ONE__(__output_terminal_present(driver, audio_dev, 
        output_terminal_id)));
    MemoryService_C* mem_service = __DEV_MEMORY(audio_dev->usb_dev);
    CopyProtectControlParameterBlock* copy_protect_block = 
        __MAP_IO_KERNEL__(mem_service, CopyProtectControlParameterBlock, 
        sizeof(CopyProtectControlParameterBlock));
    __build_copy_protect_control(driver, copy_protect_block, copy_protect_value);
    return audio_copy_protect_control(driver, audio_dev, __SET_REQUEST__, 
        COPY_PROTECT_CONTROL, channel_number, audio_interface, output_terminal_id,
            copy_protect_block, sizeof(CopyProtectControlParameterBlock), 
            &copy_protect_control_callback, 0);
}

static int8_t get_copy_protect(AudioDriver* driver, AudioDev* audio_dev,
    uint8_t input_terminal_id, CPL copy_protect_value, uint8_t channel_number,
    uint8_t audio_interface){
    __IF_RET_NEG__(__IS_NEG_ONE__(__input_terminal_present(driver, audio_dev, 
        input_terminal_id)));
    MemoryService_C* mem_service = __DEV_MEMORY(audio_dev->usb_dev);
    CopyProtectControlParameterBlock* copy_protect_block = 
        __MAP_IO_KERNEL__(mem_service, CopyProtectControlParameterBlock, 
        sizeof(CopyProtectControlParameterBlock));
    return audio_copy_protect_control(driver, audio_dev, __GET_REQUEST__, 
        COPY_PROTECT_CONTROL, channel_number, audio_interface, input_terminal_id,
            copy_protect_block, sizeof(CopyProtectControlParameterBlock), 
            &copy_protect_control_callback, 0);
}

static int8_t set_sound_value(AudioDriver* driver, int16_t value, AudioDev* audio_dev,
    uint8_t channel_number, uint8_t audio_interface, uint8_t feature_unit_id){
    __IF_RET_NEG__(value > audio_dev->max_volume || value < audio_dev->min_volume);
    MemoryService_C* mem_service = __DEV_MEMORY(audio_dev->usb_dev);
    VolumeControlParameterBlockFirstForm* volume_control_block = 
        __MAP_IO_KERNEL__(mem_service, VolumeControlParameterBlockFirstForm, 
        sizeof(VolumeControlParameterBlockFirstForm));
    
    __build_volume_control_block(driver, volume_control_block, value);
    __IF_RET_NEG__(__IS_NEG_ONE__(audio_volume_control(driver, audio_dev,
        __SET_REQUEST__, SET_CUR, channel_number, audio_interface, feature_unit_id,
        volume_control_block, sizeof(volume_control_block), &volume_control_block, 0)));
    audio_dev->curr_volume = value;
    return __RET_S__;
}

static int8_t set_max_sound_value(AudioDriver* driver, int16_t value, 
    AudioDev* audio_dev, uint8_t channel_number, uint8_t audio_interface,
    uint8_t feature_unit_id){
    __IF_RET_NEG__(value < audio_dev->min_volume);
    MemoryService_C* mem_service = __DEV_MEMORY(audio_dev->usb_dev);
    VolumeControlParameterBlockFirstForm* volume_control_block = 
        __MAP_IO_KERNEL__(mem_service, VolumeControlParameterBlockFirstForm, 
        sizeof(VolumeControlParameterBlockFirstForm));

    __build_volume_control_block(driver, volume_control_block, value);
    __IF_RET_NEG__(__IS_NEG_ONE__(audio_volume_control(driver, audio_dev,
        __SET_REQUEST__, SET_MAX, channel_number, audio_interface, feature_unit_id,
        volume_control_block, sizeof(volume_control_block), &volume_control_block, 0)));
    audio_dev->max_volume = value;
    return __RET_S__;
}

static int8_t set_min_sound_value(AudioDriver* driver, int16_t value,
    AudioDev* audio_dev, uint8_t channel_number, uint8_t audio_interface, 
    uint8_t feature_unit_id){
    __IF_RET_NEG__(value > audio_dev->max_volume);
    MemoryService_C* mem_service = __DEV_MEMORY(audio_dev->usb_dev);
    VolumeControlParameterBlockFirstForm* volume_control_block = 
        __MAP_IO_KERNEL__(mem_service, VolumeControlParameterBlockFirstForm, 
        sizeof(VolumeControlParameterBlockFirstForm));
     __build_volume_control_block(driver, volume_control_block, value);
    __IF_RET_NEG__(__IS_NEG_ONE__(audio_volume_control(driver, audio_dev,
        __SET_REQUEST__, SET_MIN, channel_number, audio_interface, feature_unit_id,
        volume_control_block, sizeof(volume_control_block), &volume_control_block, 0)));
    audio_dev->min_volume = value;
    return __RET_S__;
}

static int8_t set_volume_resolution(AudioDriver* driver, AudioDev* audio_dev,
    int16_t resolution_value, uint8_t channel_number, uint8_t audio_interface,
    uint8_t feature_unit_id){
    __IF_RET_NEG__(resolution_value <= 0);
    MemoryService_C* mem_service = __DEV_MEMORY(audio_dev->usb_dev);
    VolumeControlParameterBlockFirstForm* volume_control_block = 
        __MAP_IO_KERNEL__(mem_service, VolumeControlParameterBlockFirstForm, 
        sizeof(VolumeControlParameterBlockFirstForm));
    __build_volume_control_block(driver, volume_control_block, resolution_value);
    
    __IF_RET_NEG__(__IS_NEG_ONE__(audio_volume_control(driver, audio_dev, __SET_REQUEST__, 
        SET_RES, channel_number, audio_interface, feature_unit_id, volume_control_block,
        sizeof(VolumeControlParameterBlockFirstForm), &volume_control_callback, 0)));
    return __RET_S__;
}

static int8_t increment_sound(AudioDriver* driver, AudioDev* audio_dev,
    uint8_t channel_number, uint8_t audio_interface, uint8_t feature_unit_id){ 
    __IF_RET_NEG__(audio_dev->curr_volume == audio_dev->max_volume);
    int16_t wVolume = audio_dev->curr_volume + 0x0001;

    MemoryService_C* mem_service = __DEV_MEMORY(audio_dev->usb_dev);
    VolumeControlParameterBlockFirstForm* volume_control_block = 
        __MAP_IO_KERNEL__(mem_service, VolumeControlParameterBlockFirstForm, 
        sizeof(VolumeControlParameterBlockFirstForm));

    __build_volume_control_block(driver, volume_control_block, wVolume);

    __IF_RET_NEG__(__IS_NEG_ONE__(audio_volume_control(driver, audio_dev, __SET_REQUEST__, 
    SET_CUR, channel_number, audio_interface, feature_unit_id, volume_control_block, 
        sizeof(VolumeControlParameterBlockFirstForm), &volume_control_callback, 0)));
    audio_dev->curr_volume = wVolume;
    return __RET_S__;
}

// for now we just are doing 0x0001 steps
// implement it in such a way that there are just a fixed number of steps in total !
static int8_t decrement_sound(AudioDriver* driver, AudioDev* audio_dev,
    uint8_t channel_number, uint8_t audio_interface, uint8_t feature_unit_id){
    int16_t wVolume;
    __IF_RET_NEG__(audio_dev->curr_volume == audio_dev->min_volume);
    __IF_ELSE__((audio_dev->curr_volume - 0x0001) == audio_dev->min_volume, 
        wVolume = __SILENCE_VOL__, wVolume = audio_dev->curr_volume - 0x0001);

    MemoryService_C* mem_service = __DEV_MEMORY(audio_dev->usb_dev);
    VolumeControlParameterBlockFirstForm* volume_control_block = 
        __MAP_IO_KERNEL__(mem_service, VolumeControlParameterBlockFirstForm, 
        sizeof(VolumeControlParameterBlockFirstForm));

    __build_volume_control_block(driver, volume_control_block, wVolume);

    __IF_RET_NEG__(__IS_NEG_ONE__(audio_volume_control(driver, audio_dev, __SET_REQUEST__,
        SET_CUR, channel_number, audio_interface, feature_unit_id, volume_control_block, 
        sizeof(VolumeControlParameterBlockFirstForm), &volume_control_callback, 0)));
    audio_dev->curr_volume = wVolume;
    return __RET_S__;
}

// just using the first form
static int8_t mute(AudioDriver* driver, AudioDev* audio_dev, uint8_t channel_number,
    uint8_t audio_interface, uint8_t feature_unit_id){
    MemoryService_C* mem_service = __DEV_MEMORY(audio_dev->usb_dev);
    MuteControlParameterBlockFirstForm* mute_control_block = 
    __MAP_IO_KERNEL__(mem_service, MuteControlParameterBlockFirstForm, 
        sizeof(MuteControlParameterBlockFirstForm));
    __build_mute_control_block(driver, mute_control_block, 1);
    return audio_mute_control(driver, audio_dev, __SET_REQUEST__, SET_CUR, 
        channel_number, audio_interface, feature_unit_id, mute_control_block,
        sizeof(MuteControlParameterBlockFirstForm), &mute_control_callback,
        0);
}

// just using the first form
static int8_t unmute(AudioDriver* driver, AudioDev* audio_dev,
    uint8_t channel_number, uint8_t audio_interface, uint8_t feature_unit_id){
    MemoryService_C* mem_service = __DEV_MEMORY(audio_dev->usb_dev);
    MuteControlParameterBlockFirstForm* mute_control_block = 
    __MAP_IO_KERNEL__(mem_service, MuteControlParameterBlockFirstForm, 
        sizeof(MuteControlParameterBlockFirstForm));
    __build_mute_control_block(driver, mute_control_block, 0);
    return audio_mute_control(driver, audio_dev, __SET_REQUEST__, SET_CUR, 
        channel_number, audio_interface, feature_unit_id, mute_control_block,
        sizeof(MuteControlParameterBlockFirstForm), &mute_control_callback,
        0);
}