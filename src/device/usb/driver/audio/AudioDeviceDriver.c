#include "AudioDeviceDriver.h"
#include "../../controller/UsbControllerFlags.h"
#include "../../include/UsbErrors.h"


static inline uint8_t __is_cs_interface(AudioDriver* driver, UsbDev* dev, uint8_t* start){
  return __IF_EXT__((*(start + 1) == CS_INTERFACE), 1, 0);
}

static inline uint8_t __is_as_endpoint(AudioDriver* driver, UsbDev* dev, uint8_t* start){
  return __IF_EXT__((*(start + 1) == CS_ENDPOINT) &&
    (*(start + 2) == EP_GENERAL), 1, 0);
}

static inline uint8_t __is_ac_interface(AudioDriver* driver, UsbDev* dev, uint8_t* start){
  return __IF_EXT__(__is_cs_interface(driver, dev, start) && 
    (*(start + 2) == HEADER), 1, 0);
}

static inline uint8_t __is_as_interface(AudioDriver* driver, UsbDev* dev, uint8_t* start){
  return __IF_EXT__(__is_cs_interface(driver, dev, start) && 
    (*(start + 2) == AS_GENERAL), 1, 0);
}

static inline uint8_t __is_input_terminal(AudioDriver* driver, UsbDev* dev, uint8_t* start){
  return __IF_EXT__((*(start + 2) == INPUT_TERMINAL), 1, 0);
}

static inline uint8_t __is_output_terminal(AudioDriver* driver, UsbDev* dev, uint8_t* start){
  return __IF_EXT__((*(start + 2) == OUTPUT_TERMINAL), 1, 0);
}

static inline uint8_t __is_mixer(AudioDriver* driver, UsbDev* dev, uint8_t* start){
  return __IF_EXT__((*(start + 2) == MIXER_UNIT), 1, 0);
}

static inline uint8_t __is_selector(AudioDriver* driver, UsbDev* dev, uint8_t* start){
  return __IF_EXT__((*(start + 2) == SELECTOR_UNIT), 1, 0);
}

static inline uint8_t __is_feature(AudioDriver* driver, UsbDev* dev, uint8_t* start){
  return __IF_EXT__((*(start + 2) == FEATURE_UNIT), 1, 0);
}

static inline uint8_t __is_processing_unit(AudioDriver* driver, uint8_t* start){
  return __IF_EXT__((*(start + 2) == PROCESSING_UNIT), 1, 0);
}

static inline uint8_t __is_extension_unit(AudioDriver* driver, uint8_t* start){
  return __IF_EXT__((*(start + 2) == EXTENSION_UNIT), 1, 0);
}

static void audio_callback(UsbDev *dev, uint32_t status, void *data) {
  if (status & E_TRANSFER) {
    dev->error_while_transfering = 1;
  } else if (status & S_TRANSFER) {
    dev->error_while_transfering = 0;
  }
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

static int input_build_routine(AudioDriver* driver, UsbDev* dev, uint8_t* start,
  ACInterface* ac_interface){
  __TYPE_CAST__(InputTerminalDescriptor*, audio_in_desc, start);
      ac_interface->in_terminal_desc = *audio_in_desc;
}

static int output_build_routine(AudioDriver* driver, UsbDev* dev, uint8_t* start,
  ACInterface* ac_interface){
  __TYPE_CAST__(OutputTerminalDescriptor*, audio_out_desc, start);
      ac_interface->out_terminal_desc = *audio_out_desc;
}

static inline void __parse_baSourceID(AudioDriver* driver, uint8_t* start,
    uint8_t offset, uint8_t number_pins, MemoryService_C* mem_service,
    uint8_t** member){
    __ALLOC_KERNEL_MEM_T__(mem_service, uint8_t, baSource, number_pins);
    uint8_t* start = start + offset;
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

static int mixer_build_routine(AudioDriver* driver, UsbDev* dev, uint8_t* start,
  MemoryService_C* m, ACInterface* ac_interface, uint8_t upstream_channels){
  MixerUnitDescriptor mixer_unit_desc;
  uint8_t bNrInPins = *(start + 4);
  mixer_unit_desc.bLength = *start;
  mixer_unit_desc.bDescriptorType = *(start+1);
  mixer_unit_desc.bDescriptorSubtype = *(start+2);
  mixer_unit_desc.bUnitID = *(start+3);
  mixer_unit_desc.bNrInPins = bNrInPins;
  __parse_baSourceID(driver, start, 5, bNrInPins, m, &mixer_unit_desc.baSourceID);
  __parse_cluster(driver, &mixer_unit_desc.cluster_desc, start, 6 + bNrInPins);
  uint8_t output_channels = __get_total_channels_in_cluster(driver, mixer_unit_desc.cluster_desc);
  uint8_t bmControlLen = mixer_parse_bmControlLength(driver, upstream_channels, output_channels);
  mixer_unit_desc.bmControlsLength = bmControlLen;
  mixer_unit_desc.bmControls = *(start+5+bNrInPins+4);
  mixer_unit_desc.iMixer = *(start+5+bNrInPins+4+bmControlLen);
  ac_interface->mixer_unit_desc = mixer_unit_desc;
}

static uint8_t find_total_channels_by_source_id(AudioDriver* driver, 
    AudioInterfaceCollection* audio_collection, uint8_t bSourceID){
    ACInterface* ac_interface = audio_collection->ac_itf;
    uint8_t total_channels = 0;
    int16_t unit_id = -1;
    __IF_CUSTOM__(ac_interface->in_terminal_desc.bTerminalID == bSourceID, 
        unit_id = ac_interface->in_terminal_desc.bTerminalID);
    __IF_CUSTOM__(ac_interface->out_terminal_desc.bTerminalID == bSourceID,
        unit_id = ac_interface->out_terminal_desc.bTerminalID);
    __IF_CUSTOM__(__IS_NEG_ONE__(unit_id), return total_channels);
    
    return 0;
}

static int8_t is_bm_control_programmable(AudioDriver* driver, 
    MixerUnitDescriptor* mixer_unit, uint8_t input_channel, 
    uint8_t output_channel, uint8_t output_channels, uint8_t input_channels){
    uint8_t N = mixer_unit->bmControlsLength;
    uint8_t shift;
    __IF_ELSE__(((output_channels % 8) == 0), shift = output_channels / 8,
        shift = (output_channels / 8) + 1);
    uint8_t index = input_channel * shift;
    uint8_t* row = mixer_unit->bmControls + index;
    return ((*row) & output_channel);
}

static uint8_t mixer_parse_bmControlLength(AudioDriver* driver,
    uint8_t upstream_channels, uint8_t output_channels){
    // bmControls consists of each row corresponding to an input channel and each 
    // column corresponding to an output channel
    // total count of bmControls is ((n * m) / 8) if mod 8 == 0; otherwise ((n * m) / 8) + 1
    uint8_t N;
    __IF_ELSE__(((upstream_channels * output_channels) / 8) % 8 != 0, 
        N = ((upstream_channels * output_channels) / 8) + 1,
        N = ((upstream_channels * output_channels) / 8));
    return N;
}

static int selector_build_routine(AudioDriver* driver, UsbDev* dev, uint8_t* start,
  ACInterface* ac_interface, MemoryService_C* m){
  SelectorUnitDescriptor selector_unit_desc;
  uint8_t bNrInPins = *(start + 4);
  selector_unit_desc.bLength = *start;
  selector_unit_desc.bDescriptorType = *(start+1);
  selector_unit_desc.bDescriptorSubtype = *(start+2);
  selector_unit_desc.bUnitID = *(start+3);
  selector_unit_desc.bNrInPins = bNrInPins;
  __parse_baSourceID(driver, start, 5, bNrInPins, m, 
    &selector_unit_desc.baSourceID);
  selector_unit_desc.iSelector = *(start+5+bNrInPins);
  ac_interface->selector_unit_desc = selector_unit_desc;
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
    uint8_t* start, uint8_t offset){
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

static int feature_build_routine(AudioDriver* driver, UsbDev* dev, uint8_t* start,
  ACInterface* ac_interface, MemoryService_C* m, uint8_t upstream_channels){
  FeatureUnitDescriptor feature_unit_desc;
  uint8_t bmaControl_offset = 6;
  uint8_t control_size = *(start+5);
  feature_unit_desc.bLength = *start;
  feature_unit_desc.bDescriptorType = *(start+1);
  feature_unit_desc.bDescriptorSubtype = *(start+2);
  feature_unit_desc.bUnitID = *(start+3);
  feature_unit_desc.bSourceID = *(start+4);
  feature_unit_desc.bControlSize = control_size;
  __parse_bmaControls(driver, &feature_unit_desc.bmaControls, control_size,
    m, upstream_channels, start, bmaControl_offset);
  feature_unit_desc.iFeature = *(start + bmaControl_offset + 
    ((upstream_channels+1) * 
    control_size));
  ac_interface->feature_unit_desc = feature_unit_desc;
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

static int up_dow_mix_processing_unit_build_routine(AudioDriver* driver,
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

static inline void __parse_common_unit_descriptor_part(AudioDriver* driver, uint8_t* start,
    uint8_t* len_member, uint8_t* type_member, uint8_t* subtype_member,
    uint8_t* unit_id){
    *len_member = *start;
    *type_member = *(start+1);
    *subtype_member = *(start+2);
    *unit_id = *(start+3);
}

static int processing_unit_build_routine(AudioDriver* driver, UsbDev* dev, uint8_t* start,
    ACInterface* ac_interface, MemoryService_C* m){
    ProcessingUnitDescriptor processing_unit_desc;
    __parse_common_unit_descriptor_part(driver, start, &processing_unit_desc.bLength,
        &processing_unit_desc.bDescriptorType, &processing_unit_desc.bDescriptorSubtype,
        &processing_unit_desc.bUnitID);
    processing_unit_desc.wProcessType = *(start+4);
    processing_unit_desc.bNrInPins = *(start+6);
    __parse_baSourceID(driver, start, 7, processing_unit_desc.bNrInPins,
        m, &processing_unit_desc.baSource);
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
    ac_interface->processing_unit_desc = processing_unit_desc;
}

static int extension_unit_build_routine(AudioDriver* driver, UsbDev* dev, uint8_t* start,
    ACInterface* ac_interface, MemoryService_C* m){
    ExtensionUnitDescriptor extension_unit_descriptor;
    __parse_common_unit_descriptor_part(driver, start, &extension_unit_descriptor.bLength,
        &extension_unit_descriptor.bDescriptorType, &extension_unit_descriptor.bDescriptorSubtype,
        &extension_unit_descriptor.bUnitID);
    extension_unit_descriptor.wExtensionCode = *(start+4);
    extension_unit_descriptor.bNrInPins = *(start+6);
    __parse_baSourceID(driver, start, 7, extension_unit_descriptor.bNrInPins,
        m, &extension_unit_descriptor.baSource);
    __parse_cluster(driver, &extension_unit_descriptor.cluster,
        start, 7+extension_unit_descriptor.bNrInPins);
    extension_unit_descriptor.bControlSize = *(start+11+extension_unit_descriptor.bNrInPins);
    extension_unit_descriptor.bmControls = 
        __parse_bmControls(driver, start+12+extension_unit_descriptor.bNrInPins, 0,
        &extension_unit_descriptor.bControlSize, m);
    extension_unit_descriptor.iExtension = *(start+12+extension_unit_descriptor.bNrInPins+
        extension_unit_descriptor.bControlSize);
    ac_interface->extension_unit_desc = extension_unit_descriptor;
}

static int associated_interface_build_routine(AudioDriver* driver, uint8_t* start,
    ACInterface* ac_interface, MemoryService_C* m){
    AssociatedInterfaceDescriptor associated_interface_descriptor;
    __parse_common_unit_descriptor_part(driver, start, &associated_interface_descriptor.bLength,
        &associated_interface_descriptor.bDescriptorType, &associated_interface_descriptor.bDescriptorSubtype,
        &associated_interface_descriptor.bInterfaceNr);
    // unit | terminal listed here again ?
}

static int handle_ac_routine(AudioDriver* driver, UsbDev* dev, uint8_t* start, 
  MemoryService_C* m, uint8_t minor){
  uint8_t* end = 0;
  __ALLOC_KERNEL_MEM_S__(m, AudioInterfaceCollection, audio_itf_c);
  __TYPE_CAST__(AudioControlDescriptor*, ac_desc, start);
  __ALLOC_KERNEL_MEM_S__(m, ACInterface, ac_interface);
  __ALLOC_KERNEL_MEM_T__(m, ASInterface*, as_interfaces, 
    ac_desc->bInCollection);
  audio_itf_c->ac_itf = ac_interface;
  audio_itf_c->as_itf = as_interfaces;
  audio_itf_c->num_streaming_interfaces = ac_desc->bInCollection;
  ac_interface->ac_desc = *ac_desc;
  end = start + (ac_desc->wTotalLength);
  
  while(start < end){
    if(__is_input_terminal(driver, dev, start)){
      __IF_RET_NEG__(__IS_NEG_ONE__(input_build_routine(driver, dev, start, ac_interface)));
    }
    else if(__is_output_terminal(driver, dev, start)) {
      __IF_RET_NEG__(__IS_NEG_ONE__(output_build_routine(driver, dev, start, ac_interface)));
    }
    else if(__is_mixer(driver, dev, start)){
      __IF_RET_NEG__(__IS_NEG_ONE__(mixer_build_routine(driver, dev, start, m, ac_interface)));
    }
    else if(__is_selector(driver, dev, start)){
      __IF_RET_NEG__(__IS_NEG_ONE__(selector_build_routine(driver, dev, start, ac_interface, m)));
    }
    else if(__is_feature(driver, dev, start)){
      __IF_RET_NEG__(__IS_NEG_ONE__(feature_build_routine(driver, dev, start, ac_interface, m)));
    }
    else if(__is_processing_unit(driver, start)){

    }
    else if(__is_extension_unit(driver, start)){

    }
    start += *start;
  }
  driver->dev[minor].audio_collection = audio_itf_c;

  return __RET_S__;
}

static int handle_as_routine(AudioDriver* driver, UsbDev* dev, uint8_t* start, 
  MemoryService_C* m, uint8_t as_num, uint8_t minor){
  AudioInterfaceCollection* audio_itf_collection = driver->dev[minor].audio_collection;
  __ALLOC_KERNEL_MEM_S__(m, ASInterface, as_interface);
  __ALLOC_KERNEL_MEM_S__(m, ASEndpoint, as_endpoint);
  audio_itf_collection->as_itf[as_num] = as_interface;
  __TYPE_CAST__(struct AudioStreamingDescriptor*, as_desc, start);
  as_interface->as_desc = *as_desc;
  as_interface->as_endpoint = as_endpoint;
}

static int handle_as_endpoint_routine(AudioDriver* driver, UsbDev* dev, uint8_t* start,
  ASInterface* audio_streaming_interface){
  // not ready
}

static void audio_device_routine(AudioDriver* driver, UsbDev* dev, uint8_t* start,
    uint8_t* end, MemoryService_C* mem_service, uint8_t minor){
    uint8_t as_num = 0;
    while(start < end){
        if(__is_ac_interface(driver, dev, start)){
            handle_ac_routine(driver, dev, start, mem_service, minor);
        }
        else if(__is_as_interface(driver, dev, start)){
            handle_as_routine(driver, dev, start, mem_service,
                as_num, minor);
        }
        else if(__is_as_endpoint(driver, dev, start)){
            // 1:1 association between audio streaming interface and iso endpoint
            ASInterface* as_interface = driver->dev[minor].audio_collection->as_itf + as_num++;
            handle_as_endpoint_routine(driver, dev, start, as_interface);
        }
        start += *(start);
    }
}

static void configure_audio_device(AudioDriver* driver){
    __FOR_RANGE__(i, int, 0, MAX_DEVICES_PER_USB_DRIVER){
        __IF_CONTINUE__(__IS_ZERO__(driver->audio_map[i]));
        // request again the configuration of the device and traverse now the device specific
        // interfaces/endpoints
        UsbDev* dev = driver->dev[i].usb_dev;
        MemoryService_C* mem_service = __DEV_MEMORY(dev);
        uint8_t* map_io_buffer = __MAP_IO_KERNEL__(mem_service, uint8_t, PAGE_SIZE);
        uint8_t map_io_offset = 0;
        uint16_t total_len;
        uint8_t desc_len;
        uint8_t *start, *end;
        uint8_t active_config_val = dev->active_config->config_desc.bConfigurationValue;
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
        audio_device_routine(driver, dev, start, end, mem_service, i);
    }
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

static inline uint8_t __supports_bmaControl(AudioDriver* driver, uint8_t channel_number,
    AudioInterfaceCollection* audio_collection){
    FeatureUnitDescriptor feature_desc = audio_collection->ac_itf->feature_unit_desc;
    uint8_t bControlSize = feature_desc.bControlSize;

}

static uint8_t __input_terminal_present(AudioDriver* driver, AudioDev* dev,
    uint8_t input_terminal_id){
    return __RET_S__;
}

static uint8_t __output_terminal_present(AudioDriver* driver, AudioDev* dev,
    uint8_t output_terminal_id){
    return __RET_S__;
}

static uint8_t __feature_unit_present(AudioDriver* driver, AudioDev* dev, 
    uint8_t feature_unit_id){
    FeatureUnitDescriptor feature_desc = 
        dev->audio_collection->ac_itf->feature_unit_desc;

    return __RET_S__;
}

static uint8_t __processing_unit_present(AudioDriver* driver, AudioDev* dev,
    uint8_t processing_unit_id){
    
}

static uint8_t __extension_unit_present(AudioDriver* driver, AudioDev* dev,
    uint8_t extension_unit_id){
    
}

static uint8_t __selector_unit_present(AudioDriver* driver, AudioDev* dev,
    uint8_t selector_unit_id){
    SelectorUnitDescriptor sel_desc = 
        dev->audio_collection->ac_itf->selector_unit_desc;
    return __RET_S__;
}

static uint8_t __mixer_unit_present(AudioDriver* driver, AudioDev* dev,
    uint8_t mixer_unit_id){
    MixerUnitDescriptor mixer_desc = 
        dev->audio_collection->ac_itf->mixer_unit_desc;
    return __RET_S__;
}

static uint8_t __channel_present(AudioDriver* driver, AudioDev* audio_dev,
    uint8_t channel_number){
    return __RET_S__;
}

static uint8_t __audio_interface_present(AudioDriver* driver, AudioDev* dev, 
    uint8_t audio_interface){
    // each audio control interface resides in an interface
    uint8_t num_interfaces = 
        dev->usb_dev->active_config->config_desc.bNumInterfaces;
    return __IF_EXT__(audio_interface < num_interfaces, 1, 0);
}

static uint8_t __valid_request_form(AudioDriver* driver, uint8_t request_form){
    return __IF_EXT__((request_form == __SET_REQUEST__ || 
        request_form == __GET_REQUEST__), 1, 0);
}

static uint8_t __get_total_channels_in_cluster(AudioDriver* driver, 
    ClusterDescriptor cluster){
    return cluster.bNrChannels;
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

    return (void*)0;
}

// does not support master control as input channel !
static int16_t __get_channel_number_by_loc(AudioDriver* driver, ClusterDescriptor cluster,
    SpatialLocations loc){
    uint8_t channel_number = 0;
    uint16_t wChannelConfig = cluster.wChannelConfig;
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

static int8_t __spatial_loc_l(AudioDriver* driver, SpatialLocations loc){
    return __IF_EXT__(__NOT_ZERO__((loc == SPATIAL_LOC_L)), 1, -1);
}

static int8_t __spatial_loc_r(AudioDriver* driver, SpatialLocations loc){
    return __IF_EXT__(__NOT_ZERO__((loc == SPATIAL_LOC_R)), 1, -1);
}

static int8_t __spatial_loc_c(AudioDriver* driver, SpatialLocations loc){
    return __IF_EXT__(__NOT_ZERO__((loc == SPATIAL_LOC_C)), 1, -1);
}

static int8_t __spatial_loc_lfe(AudioDriver* driver, SpatialLocations loc){
    return __IF_EXT__(__NOT_ZERO__((loc == SPATIAL_LOC_LFE)), 1, -1);
}

static int8_t __spatial_loc_ls(AudioDriver* driver, SpatialLocations loc){
    return __IF_EXT__(__NOT_ZERO__((loc == SPATIAL_LOC_LS)), 1, -1);
}

static int8_t __spatial_loc_rs(AudioDriver* driver, SpatialLocations loc){
    return __IF_EXT__(__NOT_ZERO__((loc == SPATIAL_LOC_RS)), 1, -1);
}

static int8_t __spatial_loc_lc(AudioDriver* driver, SpatialLocations loc){
    return __IF_EXT__(__NOT_ZERO__((loc == SPATIAL_LOC_LC)), 1, -1);
}

static int8_t __spatial_loc_rc(AudioDriver* driver, SpatialLocations loc){
    return __IF_EXT__(__NOT_ZERO__((loc == SPATIAL_LOC_RC)), 1, -1);
}

static int8_t __spatial_loc_s(AudioDriver* driver, SpatialLocations loc){
    return __IF_EXT__(__NOT_ZERO__((loc == SPATIAL_LOC_S)), 1, -1);
}

static int8_t __spatial_loc_sl(AudioDriver* driver, SpatialLocations loc){
    return __IF_EXT__(__NOT_ZERO__((loc == SPATIAL_LOC_SL)), 1, -1);
}

static int8_t __spatial_loc_sr(AudioDriver* driver, SpatialLocations loc){
    return __IF_EXT__(__NOT_ZERO__((loc == SPATIAL_LOC_SR)), 1, -1);
}

static int8_t __spatial_loc_t(AudioDriver* driver, SpatialLocations loc){
    return __IF_EXT__(__NOT_ZERO__((loc == SPATIAL_LOC_T)), 1, -1);
}

static int8_t __is_channel_in_cluster(AudioDriver* driver, ClusterDescriptor cluster,
    SpatialLocations loc){
    return __IF_EXT__(__NOT_ZERO__(cluster.wChannelConfig & loc), 1, -1);
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
    __build_mixer_control(driver, mixer_control_block, mixer_value);
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

static inline void __build_selector_control_block(AudioDriver* driver, 
    SelectorControlParameterBlock* selector_control_block, uint8_t selector_value){
    selector_control_block->wLength = 1;
    selector_control_block->bSelector = selector_value;
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

// first form
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

static void extension_callback(UsbDev* dev, uint32_t status, void* data){

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

    