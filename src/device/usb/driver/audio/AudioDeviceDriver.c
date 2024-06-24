#include "AudioDeviceDriver.h"
#include "../../controller/UsbControllerFlags.h"
#include "../../include/UsbErrors.h"
#include "../../include/UsbGeneral.h"
#include "../../interfaces/TimeInterface.h"
#include "../../events/event/audio/AudioSampleEvent.h"
#include "lib/util/usb/input/InputEvents.h"
#include "../../controller/UsbController.h"
#include "../../interfaces/ThreadInterface.h"

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
    uint8_t output_channel);
static uint8_t mixer_bmControlLength(AudioDriver* driver,
    uint8_t input_channels, uint8_t output_channels);
static int selector_build_routine(AudioDriver* driver, uint8_t* start,
  ACInterface* ac_interface, MemoryService_C* m);
static int feature_build_routine(AudioDriver* driver, uint8_t* start,
  ACInterface* ac_interface, MemoryService_C* m);
static void* up_down_mix_processing_unit_build_routine(AudioDriver* driver,
    uint8_t* start, MemoryService_C* m);
static void* dolby_prologic_processing_unit_build_routine(AudioDriver* driver,
    MemoryService_C* m, uint8_t* start);
static void* stereo_extender_processing_unit_build_routine(AudioDriver* driver,
    MemoryService_C* m, uint8_t* start);
static void* reverberation_processing_unit_build_routine(AudioDriver* driver,
    MemoryService_C* m, uint8_t* start);
static void* chorus_processing_unit_build_routine(AudioDriver* driver, 
    MemoryService_C* m, uint8_t* start);
static void* dynamic_range_compressor_processing_unit_build_routine(AudioDriver* driver, 
    MemoryService_C* m, uint8_t* start);
static void parse_process_specific_descriptor(AudioDriver* driver, 
    struct ProcessingUnit* process_unit, enum ProcessingUnitProcessTypes wProcessType,
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
static SpatialLocations get_loc_by_channel_number(AudioDriver* driver, 
    ClusterDescriptor cluster, uint8_t channel_number);
static int16_t get_channel_number_by_loc(AudioDriver* driver, ClusterDescriptor cluster,
    SpatialLocations loc);
static int8_t audio_mute_control(AudioDriver* driver, AudioDev* audio_dev,
    Interface* interface, uint8_t request_form, uint8_t bRequest, 
    uint8_t channel_number, callback_function callback, uint8_t flags, uint8_t bMute);
static int8_t audio_volume_control(AudioDriver* driver, AudioDev* audio_dev,
    Interface* interface, uint8_t request_form, uint8_t bRequest, 
    uint8_t channel_number, callback_function callback, uint8_t flags, int16_t wVolume);
static int8_t audio_mixer_control(AudioDriver* driver, AudioDev* audio_dev,
    Interface* interface, uint8_t request_form, uint8_t bRequest, 
    uint8_t icn, uint8_t ocn, int16_t wMixer);
static int8_t set_mixer_unit_control(AudioDriver* driver, AudioDev* audio_dev,
    Interface* interface, uint8_t icn, uint8_t ocn, int16_t wMixer);
static int8_t get_mixer_unit_control(AudioDriver* driver, AudioDev* audio_dev,
    Interface* interface, uint8_t icn, uint8_t ocn);
static int8_t audio_selector_control(AudioDriver* driver, AudioDev* audio_dev,
    Interface* interface, uint8_t request_form, uint8_t bRequest, uint8_t bSelector);
static int8_t selector_set_cur(AudioDriver* driver, AudioDev* audio_dev, 
    Interface* interface, uint8_t bSelector);
static int8_t audio_processing_control(AudioDriver* driver, AudioDev* audio_dev,
    Interface* interface, uint8_t unit_id, uint8_t request_form, uint8_t bRequest, 
    uint8_t control, void* data, uint8_t len, callback_function callback,
    uint8_t flags);
static int8_t audio_extension_control(AudioDriver* driver, AudioDev* audio_dev,
    Interface* interface, uint8_t request_form, uint8_t bRequest, uint8_t bOn);
static int8_t audio_enable_processing_control(AudioDriver* driver, 
    AudioDev* audio_dev, Interface* interface, uint8_t request_form, 
    uint8_t bRequest, uint8_t bEnable);
static int8_t enable_processing_control(AudioDriver* driver, AudioDev* audio_dev,
    Interface* interface);
static int8_t disable_processing_control(AudioDriver* driver, AudioDev* audio_dev,
    Interface* interface);
static int8_t audio_mode_select_control(AudioDriver* driver, AudioDev* audio_dev,
    Interface* interface, uint8_t request_form, uint8_t bRequest, uint8_t bMode);
static int8_t audio_spaciousness_control(AudioDriver* driver, AudioDev* audio_dev,
    Interface* interface, uint8_t request_form, uint8_t bRequest, uint8_t bSpaciousness);
static int8_t audio_reverb_type_control(AudioDriver* driver, AudioDev* audio_dev,
    Interface* interface, uint8_t request_form, uint8_t bRequest, enum ReverbTypes type);
static int8_t audio_reverb_level_control(AudioDriver* driver, AudioDev* audio_dev,
    Interface* interface, uint8_t request_form, uint8_t bRequest, uint8_t bReverbLevel);
static int8_t audio_chorus_level_control(AudioDriver* driver, AudioDev* audio_dev,
    Interface* interface, uint8_t request_form, uint8_t bRequest, uint8_t bChorusLevel);
static int8_t audio_set_cur_spaciousness_control(AudioDriver* driver, AudioDev* audio_dev,
    Interface* interface, uint8_t bSpaciousness);
static enum ProcessingUnitProcessTypes retrieve_process_type(AudioDriver* driver,
    uint8_t* pos);
static int8_t set_audio_mode_attribute(AudioDriver* driver, AudioDev* audio_dev,
    Interface* interface, uint8_t bMode);
static int8_t enable_extension_unit(AudioDriver* driver, AudioDev* audio_dev,
    Interface* interface);
static int8_t disable_extension_unit(AudioDriver* driver, AudioDev* audio_dev,
    Interface* interface);
static int8_t audio_copy_protect_control(AudioDriver* driver, AudioDev* audio_dev,
    Interface* interface, uint8_t request_form, uint8_t bRequest, CPL cpl);
static int8_t set_copy_protect(AudioDriver* driver, AudioDev* audio_dev,
    Interface* interface, CPL cpl);
static int8_t get_copy_protect(AudioDriver* driver, AudioDev* audio_dev,
    Interface* interface, CPL cpl);
static int8_t set_sound_value(AudioDriver* driver, int16_t value, AudioDev* audio_dev,
    Interface* as_interface);
static int8_t set_max_sound_value(AudioDriver* driver, int16_t value, 
    AudioDev* audio_dev, Interface* as_interface);
static int8_t set_min_sound_value(AudioDriver* driver, int16_t value,
    AudioDev* audio_dev, Interface* as_interface);
static int8_t set_volume_resolution(AudioDriver* driver, AudioDev* audio_dev,
    int16_t resolution_value, Interface* as_interface);
static int8_t mute(AudioDriver* driver, AudioDev* audio_dev, Interface* interface);
static int8_t unmute(AudioDriver* driver, AudioDev* audio_dev, Interface* interface);
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
static int8_t get_sampling_frequency(AudioDriver* driver,
    AudioDev* audio_dev, Interface* as_interface);
static uint8_t __get_terminal(AudioDriver* driver, AudioDev* audio_dev, ASInterface* as_interface);
/*static AudioSampleEvent generate_audio_event(AudioDriver* driver,
    ASInterface* as_interface, uint8_t** raw_data);
static void trigger_audio_event(AudioDriver* driver, GenericEvent* event); */
static int8_t sync_streaming_interface(AudioDriver* driver, UsbDev* dev,
    Interface* interface);
static void audio_device_routine(AudioDriver* driver, uint8_t* start,
    uint8_t* end, MemoryService_C* mem_service, uint8_t minor);
static uint8_t get_available_controls(AudioDriver* driver,
    struct FeatureUnit* feature_unit);
static uint8_t get_upstream_channels_by_source_id_depth(AudioDriver* driver,
    uint8_t bSourceID, ACInterface* ac_interface, uint8_t depth);
static uint8_t get_upstream_channels_by_source_id(AudioDriver* driver, 
    uint8_t bSourceID, ACInterface* ac_interface);
static int8_t get_max_volume(AudioDriver* driver, AudioDev* audio_dev, 
    Interface* as_interface);
static int8_t get_min_volume(AudioDriver* driver,
    AudioDev* audio_dev, Interface* as_interface);
static int8_t get_current_volume(AudioDriver* driver,
    AudioDev* audio_dev, Interface* as_interface);
static int8_t get_mute_attribute(AudioDriver* driver, AudioDev* audio_dev, Interface* interface);
static void switch_if_zero_bandwidth(AudioDriver* driver, AudioDev* audio_dev);//, Interface* interface);
static void request_device_attributes(AudioDriver* driver, AudioDev* audio_dev,
    Interface* itf);
static void configure_interfaces(AudioDriver* driver, AudioDev* audio_dev);
static void* get_top_layer_buffer(AudioDriver* driver, Interface* interface);
static Endpoint* get_iso_endpoint(AudioDriver* driver, AudioDev* audio_dev,
    Interface* as_interface);
static void* recover_unit_from_as_interface(AudioDriver* driver,
    AudioDev* audio_dev, Interface* interface, 
    enum ACInterfaceDescriptorSubtypes search, 
    enum ProcessingUnitProcessTypes process_type);
static int8_t feature_control(AudioDriver* driver, uint8_t control, 
    uint8_t request_form, AudioDev* audio_dev, Interface* interface, 
    uint8_t bRequest, uint8_t channel_number, void* data, 
    uint8_t len, callback_function callback, uint8_t flags);
static int8_t support_pitch_control(AudioDriver* driver, AudioDev* audio_dev, 
    Interface* as_interface);
static int8_t audio_endpoint_controls(AudioDriver* driver, AudioDev* audio_dev,
    uint8_t request_form, uint8_t bRequest, uint8_t control_selector, 
    Interface* as_interface, void* data, uint8_t len, 
    callback_function callback, uint8_t flags);
static int8_t sampling_frequency_control(AudioDriver* driver, 
    AudioDev* audio_dev, uint8_t request_form, uint8_t bRequest, 
    Interface* as_interface, struct SampleFrequency sample_freq,
    uint8_t flags);
static int8_t sampling_pitch_control(AudioDriver* driver, AudioDev* audio_dev,
    uint8_t request_form, uint8_t bRequest, Interface* as_interface, 
    uint8_t bPitchEnable);
static void* back_propagate_input(AudioDriver* driver, ACInterface* ac_interface,
    struct InputTerminal* input_terminal, enum ACInterfaceDescriptorSubtypes start,
    enum ProcessingUnitProcessTypes subtype);
static void* back_propagate_output(AudioDriver* driver, ACInterface* ac_interface,
    struct OutputTerminal* output_terminal, enum ACInterfaceDescriptorSubtypes search);
static void* back_propagate_output_processing(AudioDriver* driver, ACInterface* ac_interface,
    struct OutputTerminal* output_terminal, enum ProcessingUnitProcessTypes search);

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

static inline int8_t __is_zero_frequency(AudioDriver* driver, SampleFrequency sample_freq){
    return __IF_EXT__(((sample_freq.sampleFreq_lo == 0) && (sample_freq.sampleFreq_hi) == 0), 1, -1);
}

static inline ASInterface* __convert_to_class_specific_as_interface(AudioDriver* driver,
    Interface* interface){
    __TYPE_CAST__(ASInterface*, as_interface, interface->active_interface->class_specific);
    return as_interface;
}

static inline ACInterface* __convert_to_class_specific_ac_interface(AudioDriver* driver,
    Interface* interface){
    __TYPE_CAST__(ACInterface*, ac_interface, interface->active_interface->class_specific);
    return ac_interface;
}

static inline ASEndpoint* __convert_to_class_specific_as_endpoint(AudioDriver* driver,
    Endpoint* endpoint){
    __TYPE_CAST__(ASEndpoint*, as_endpoint, endpoint->class_specific);
    return as_endpoint;
}

static inline uint32_t __get_total_supported_frequencies(AudioDriver* driver, ASInterface* as_interface){
    uint8_t freq_type = as_interface->format_type->get_sam_freq_type(
        as_interface->format_type->type_descriptor);
    return __IF_EXT__((__IS_ZERO__(freq_type)), 2, freq_type);
}

static inline void __get_supported_frequencies(AudioDriver* driver, ASInterface* as_interface,
    SampleFrequency frequencies[], uint8_t* num_supported_freq){
    //uint8_t num_support = __get_total_supported_frequencies(driver, as_interface);
}

static inline uint32_t __get_sample_frequency(AudioDriver* driver, ASInterface* as_interface){
    uint16_t s_low = as_interface->current_freq.sampleFreq_lo;
    uint8_t s_high = as_interface->current_freq.sampleFreq_hi;
    return (s_low | __16_BIT_H_SHIFT__(s_high));
}

static inline uint32_t __get_sub_frame_size(AudioDriver* driver, ASInterface* as_interface){
    return as_interface->format_type->get_subframe_size(
        as_interface->format_type->type_descriptor);
}

static inline uint32_t __get_num_channels(AudioDriver* driver, ASInterface* as_interface){
    return as_interface->format_type->get_total_channels(
        as_interface->format_type->type_descriptor);
}

static inline uint16_t __get_1ms_size(AudioDriver* driver, ASInterface* as_interface){
    return ((__get_sample_frequency(driver, as_interface) / 1000)) * 
    __get_sub_frame_size(driver, as_interface) * __get_num_channels(driver, as_interface);
}

static inline uint32_t __get_bit_depth(AudioDriver* driver, ASInterface* as_interface){
    return as_interface->format_type->get_bit_depth(
        as_interface->format_type->type_descriptor);
}

static inline uint32_t __get_frame_size(AudioDriver* driver, ASInterface* as_interface){
    return __get_sub_frame_size(driver, as_interface) * __get_sample_frequency(driver, as_interface) * 
        __get_num_channels(driver, as_interface);
}

static inline int8_t __is_freq_set(AudioDriver* driver, ASInterface* as_interface){
    return __IF_EXT__((__is_zero_frequency(driver, as_interface->current_freq) == 1), -1, 1);
}

static inline uint32_t __get_volume(AudioDriver* audio_driver, AudioDev* audio_dev){
    return audio_dev->curr_volume;
}

static inline uint32_t __get_mute_attribute(AudioDriver* audio_driver, AudioDev* audio_dev){
    return audio_dev->muted;
}

static inline uint32_t __get_max_volume(AudioDriver* audio_driver, AudioDev* audio_dev){
    return audio_dev->max_volume;
}

static inline uint32_t __get_min_volume(AudioDriver* audio_driver, AudioDev* audio_dev){
    return audio_dev->min_volume;
}

static inline AudioDev* __get_audio_dev(AudioDriver* driver, uint8_t minor){
    __IF_RET_NULL__(minor >= MAX_DEVICES_PER_USB_DRIVER);
    return driver->dev + minor;
}

static inline int8_t __match_terminal_type(AudioDriver* driver, 
    ASInterface* as_interface, uint8_t terminal_type){
    return __IF_EXT__((as_interface->terminal_type == terminal_type), 1, -1);
}

static inline int8_t __is_as_output_terminal(AudioDriver* driver,
    ASInterface* as_interface){
    return __match_terminal_type(driver, as_interface, OUTPUT_TERMINAL);
}

static inline int8_t __is_as_input_terminal(AudioDriver* driver,
    ASInterface* as_interface){
    return __match_terminal_type(driver, as_interface, INPUT_TERMINAL);
}

static inline int8_t __is_zero_bandwidth_active(AudioDriver* driver,
    UsbDev* dev, Interface* as_interface){
    __IF_RET_NEG__(__IS_NEG_ONE__(__STRUCT_CALL__(driver, 
        __has_zero_bandwidth_setting, dev, as_interface)));
    return __IF_EXT__(__IS_NEG_ONE__(__STRUCT_CALL__(dev, 
        __is_class_specific_interface_set, as_interface->active_interface)),
        1, -1);
}

static inline ASInterface* __get_class_specific_as_interface(
    AudioDriver* driver, UsbDev* dev, Interface* interface){
    if(__IS_NEG_ONE__(__STRUCT_CALL__(driver,
        __is_zero_bandwidth_active, dev, interface))){
        __TYPE_CAST__(ASInterface*, as_interface, 
            interface->active_interface->class_specific);
        return as_interface;
    }
    Alternate_Interface* alt_itf = __STRUCT_CALL__(dev, 
        __get_alternate_interface_by_setting, interface, 1);
    return (ASInterface*)alt_itf->class_specific;
}

static inline Interface* __get_as_interface_by_terminal(AudioDriver* driver, 
    AudioDev* audio_dev, uint8_t terminal_type){
    __FOR_RANGE__(i, int, 0, audio_dev->audio_streaming_interfaces_num){
        ASInterface* as_interface;
        if(__is_zero_bandwidth_active(driver, audio_dev->usb_dev, 
            audio_dev->audio_streaming_interfaces[i]) == 1){
            Alternate_Interface* alt_itf = __STRUCT_CALL__(audio_dev->usb_dev, 
                __get_alternate_interface_by_setting, 
                audio_dev->audio_streaming_interfaces[i], 1); 
            as_interface = (ASInterface*)alt_itf->class_specific;
        }
        else{
            as_interface = __convert_to_class_specific_as_interface(
            driver, audio_dev->audio_streaming_interfaces[i]);
        }
        if(__match_terminal_type(driver, as_interface, terminal_type) == 1){
            return audio_dev->audio_streaming_interfaces[i];
        }
    }
    return (void*)0;
}

static inline uint16_t __get_sync_delay(AudioDriver* driver, Endpoint* endpoint){
    ASEndpoint* as_endpoint = __convert_to_class_specific_as_endpoint(driver, endpoint);
    return as_endpoint->as_iso_endpoint.wLockDelay;
}

static inline LockDelayUnit __get_sync_unit(AudioDriver* driver, Endpoint* endpoint){
    ASEndpoint* as_endpoint = __convert_to_class_specific_as_endpoint(driver, endpoint);
    return as_endpoint->as_iso_endpoint.bLockDelayUnits;
}

static inline uint8_t __match_frequency(AudioDriver* driver, SampleFrequency* as_freq,
    SampleFrequency* target){
    return __IF_EXT__(as_freq->sampleFreq_lo == target->sampleFreq_lo &&
        as_freq->sampleFreq_hi == target->sampleFreq_hi, 1, 0);
}

static inline int8_t __has_zero_bandwidth_setting(AudioDriver* driver,
    UsbDev* dev, Interface* as_interface){
    uint8_t settings = __STRUCT_CALL__(dev, 
        __get_alternate_settings, as_interface);
    return __IF_EXT__(settings <= 1, -1, 1);
}

static inline void __clear_low_level_buffers(AudioDriver* driver, 
    AudioDev* audio_dev){
    __mem_set(audio_dev->buffer_first, audio_dev->buffer_size,
        0);
    __mem_set(audio_dev->buffer_second, audio_dev->buffer_size,
        0);
}

static inline void __set_active_and_passive_buffers(AudioDriver* driver,
    ASInterface* as_interface, uint8_t** active_buffer,
    uint8_t** passive_buffer,
    uint8_t* as_interface_active_buffer,
    uint8_t* as_interface_passive_buffer){
    *passive_buffer = as_interface_active_buffer;
    *active_buffer  = as_interface_passive_buffer;
    as_interface->active_buffer = (as_interface->active_buffer % 2) + 0x01;
}

static inline int8_t __is_channel_in_cluster(AudioDriver* driver, ClusterDescriptor cluster,
    SpatialLocations loc){
    return __IF_EXT__(__NOT_ZERO__((cluster.wChannelConfig & loc)), 1, -1);
}

static inline void __set_default_audio_properties(AudioDriver* driver, 
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

static void callback_audio(UsbDev* dev, Interface* interface, uint32_t status, void* data){
    __IF_RET__(status & E_TRANSFER);
    UsbController* controller = (UsbController*)dev->controller;
    ASInterface* as_interface = internal_audio_driver->__convert_to_class_specific_as_interface(internal_audio_driver,
        interface);
    void* write_buffer = internal_audio_driver->super.interface_buffer_map->get_c(
           internal_audio_driver->super.interface_buffer_map, interface);
    write_callback write_callback = internal_audio_driver->super.interface_write_callback_map->get_c(
        internal_audio_driver->super.interface_write_callback_map, interface);
    if(as_interface->terminal_type == INPUT_TERMINAL){
        uint8_t* active_buffer;
        uint8_t* passive_buffer;
        if(as_interface->active_buffer == 0x01){
            __set_active_and_passive_buffers(internal_audio_driver,
                as_interface, &active_buffer, &passive_buffer, 
                as_interface->buffer_first, as_interface->buffer_second);
        }
        else if(as_interface->active_buffer == 0x02){
            __set_active_and_passive_buffers(internal_audio_driver,
                as_interface, &active_buffer, &passive_buffer,
                as_interface->buffer_second, as_interface->buffer_first);
        }
        controller->fast_buffer_change(controller, dev, 
            dev->__get_first_endpoint(dev, interface->active_interface),
            as_interface->qh_id, active_buffer);
        write_callback(passive_buffer, as_interface->buffer_size, write_buffer);
    }

    else if(as_interface->terminal_type == OUTPUT_TERMINAL){
        write_callback(as_interface->buffer_first, as_interface->buffer_size, write_buffer);
    }
}

static void* get_top_layer_buffer(AudioDriver* driver, Interface* interface){
   return ((UsbDriver*)driver)->interface_buffer_map->get_c(
           internal_audio_driver->super.interface_buffer_map, interface);
}

/*static AudioSampleEvent generate_audio_event(AudioDriver* driver,
    ASInterface* as_interface, uint8_t** raw_data){
    AudioSampleEvent audio_event;
    audio_event.super.event_type = AUDIO_EVENT;
    audio_event.super.event_value = AUDIO_IN;
    audio_event.sub_frame = driver->__get_sub_frame_size(driver, as_interface);
    if(audio_event.sub_frame == 0x01){
        audio_event.super.event_code = **raw_data;
        (*raw_data)++;
    }
    else if(audio_event.sub_frame == 0x02){
        audio_event.super.event_code = *((uint16_t*)*raw_data);
        (*raw_data)+=2;
    }
    else if(audio_event.sub_frame == 0x03){
        audio_event.super.event_code = 
            *((uint16_t*)*raw_data) | 
            __16_BIT_H_SHIFT__(*((uint8_t*)((*raw_data)+2)));
        (*raw_data)+=3;
    }
    else if(audio_event.sub_frame == 0x04){
        audio_event.super.event_code = *((uint32_t*)*raw_data);
        (*raw_data)+=4;
    }

    return audio_event;
} 

static void trigger_audio_event(AudioDriver* driver, GenericEvent* event){
    __STRUCT_CALL__(((UsbDriver*)driver)->dispatcher, publish_event,
        event, ((UsbDriver*)driver)->listener_id);
} */

static AudioDev *get_free_audio_dev(AudioDriver *driver) {
  __GET_FREE_DEV__(AudioDev, driver->dev, driver->audio_map);
}

static AudioDev *match_audio_dev(AudioDriver *driver, UsbDev *dev) {
  __MATCH_DEV__(AudioDev, driver->dev, usb_dev, dev);
}

static void free_audio_dev(AudioDriver *driver, AudioDev *audio_dev) {
  __FREE_DEV__(audio_dev, driver->dev, driver->audio_map);
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
    Interface* interface, UsbDeviceRequest* device_req, uint8_t recipient, uint8_t bRequest, 
    uint8_t wValueHigh, uint8_t wValueLow, uint8_t wIndexHigh,
    uint8_t wIndexLow, void* data, uint8_t len, callback_function callback, 
    uint8_t flags){
    __STRUCT_CALL__(dev, request_build, device_req, 
        HOST_TO_DEVICE | TYPE_REQUEST_CLASS | recipient, 
        bRequest, wValueHigh, wValueLow, 8, 
        __8_BIT_H_SHIFT__(wIndexHigh) | wIndexLow, len);
    __STRUCT_CALL__(dev, request, device_req, data, PRIORITY_QH_8, interface, 0,
         callback, flags);
}

static inline void __get_requests(AudioDriver* driver, UsbDev* dev, 
    Interface* interface, UsbDeviceRequest* device_req, uint8_t recipient, uint8_t bRequest, 
    uint8_t wValueHigh, uint8_t wValueLow, uint8_t wIndexHigh, 
    uint8_t wIndexLow, void* data, uint8_t len, callback_function callback,
    uint8_t flags){
    __STRUCT_CALL__(dev, request_build, device_req, 
        DEVICE_TO_HOST | TYPE_REQUEST_CLASS | recipient, 
        bRequest, wValueHigh, wValueLow, 8, 
        __8_BIT_H_SHIFT__(wIndexHigh) | wIndexLow, len);
    __STRUCT_CALL__(dev, request, device_req, data, PRIORITY_QH_8, interface, 0,
         callback, flags);
}

static inline void __request_common_get(AudioDriver* driver, UsbDev* dev,
    Interface* interface, UsbDeviceRequest* device_req, uint8_t bRequest, 
    uint8_t control_selector, uint8_t audio_interface, uint8_t unit_id, 
    void* data, uint8_t len, callback_function callback, uint8_t flags){
    __IF_RET__(bRequest != GET_CUR && bRequest != GET_MIN
        && bRequest != GET_MAX && bRequest != GET_RES);
    __get_requests(driver, dev, interface, device_req, RECIPIENT_INTERFACE, bRequest,
        control_selector, 0, unit_id, audio_interface, data, len, 
        callback, flags);
}

static inline void __request_common_set(AudioDriver* driver, UsbDev* dev, 
    Interface* interface, UsbDeviceRequest* device_req, uint8_t bRequest, 
    uint8_t control_selector, uint8_t audio_interface, uint8_t unit_id, 
    void* data, uint8_t len, callback_function callback, uint8_t flags){
    __IF_RET__(bRequest != SET_CUR && bRequest != SET_MIN
        && bRequest != SET_MAX && bRequest != SET_RES);
    __set_requests(driver, dev, interface, device_req, RECIPIENT_INTERFACE, bRequest,
        control_selector, 0, unit_id, audio_interface, data, len, 
        callback, flags);
}

static inline void __request_mixer_set(AudioDriver* driver, UsbDev* dev,
    Interface* interface, UsbDeviceRequest* device_req, uint8_t bRequest, uint8_t icn, uint8_t ocn,
    uint8_t mixer_unit_id, uint8_t audio_interface, void* data, uint8_t len,
    callback_function callback, uint8_t flags){
    __IF_RET__(bRequest != SET_CUR && bRequest != SET_MIN
        && bRequest != SET_MAX && bRequest != SET_RES);
    __set_requests(driver, dev, interface, device_req, RECIPIENT_INTERFACE, bRequest,
        icn, ocn, mixer_unit_id, audio_interface, data, len, callback, flags);
}

static inline void __request_mixer_get(AudioDriver* driver, UsbDev* dev,
    Interface* interface, UsbDeviceRequest* device_req, uint8_t bRequest, uint8_t icn, uint8_t ocn,
    uint8_t mixer_unit_id, uint8_t audio_interface, void* data, uint8_t len,
    callback_function callback, uint8_t flags){
    __IF_RET__(bRequest != GET_CUR && bRequest != GET_MIN && 
        bRequest != GET_MAX && bRequest != GET_RES);
    __get_requests(driver, dev, interface, device_req, RECIPIENT_INTERFACE, bRequest,
        icn, ocn, mixer_unit_id, audio_interface, data, len, callback, flags);
}

static inline void __request_feature_set(AudioDriver* driver, UsbDev* dev,
    Interface* interface, UsbDeviceRequest* device_req, uint8_t bRequest, uint8_t control_selector, 
    uint8_t channel_number, uint8_t feature_unit_id,
    uint8_t audio_interface, void* data, uint8_t len, 
    callback_function callback, uint8_t flags){
    __IF_RET__(bRequest != SET_CUR && bRequest != SET_MIN
        && bRequest != SET_MAX && bRequest != SET_RES);
    __set_requests(driver, dev, interface, device_req, RECIPIENT_INTERFACE, bRequest,
        control_selector, channel_number, feature_unit_id, audio_interface,
        data, len, callback, flags);
}

static inline void __request_feature_get(AudioDriver* driver, UsbDev* dev,
    Interface* interface, UsbDeviceRequest* device_req, uint8_t bRequest, uint8_t control_selector, 
    uint8_t channel_number, uint8_t feature_unit_id,
    uint8_t audio_interface, void* data, uint8_t len,
    callback_function callback, uint8_t flags){
    __IF_RET__(bRequest != GET_CUR && bRequest != GET_MIN && 
        bRequest != GET_MAX && bRequest != GET_RES);
    __get_requests(driver, dev, interface, device_req, RECIPIENT_INTERFACE, bRequest,
        control_selector, channel_number, feature_unit_id, audio_interface,
        data, len, callback, flags);
}

static inline void __request_endpoint_set(AudioDriver* driver, UsbDev* dev,
    Interface* interface, UsbDeviceRequest* device_req, uint8_t bRequest, uint8_t control_selector, uint8_t endpoint,
    void* data, uint8_t len, callback_function callback, uint8_t flags){
    __IF_RET__(bRequest != SET_CUR && bRequest != SET_MIN
        && bRequest != SET_MAX && bRequest != SET_RES);
    __set_requests(driver, dev, interface, device_req, RECIPIENT_ENDPOINT, bRequest,
        control_selector, 0, 0, endpoint, data, len, callback, flags);
}

static inline void __request_endpoint_get(AudioDriver* driver, UsbDev* dev,
    Interface* interface, UsbDeviceRequest* device_req, uint8_t bRequest, uint8_t control_selector, 
    uint8_t endpoint, void* data, uint8_t len, callback_function callback, 
    uint8_t flags){
    __IF_RET__(bRequest != GET_CUR && bRequest != GET_MIN && 
        bRequest != GET_MAX && bRequest != GET_RES);
    __get_requests(driver, dev, interface, device_req, RECIPIENT_ENDPOINT, bRequest,
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
    uint8_t* control_size, MemoryService_C* m, uint8_t available_controls,
    uint8_t* start){
    __ALLOC_KERNEL_MEM_T__(m, uint8_t*, bmaControls, sizeof(uint8_t*) * 
        available_controls);
    uint8_t* bmaControl_offset = start+6;
    __FOR_RANGE__(j, int, 0, available_controls){
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

static inline void* __search_for_unit_by_id(AudioDriver* driver, ACInterface* ac_interface, 
    uint8_t id, enum ACInterfaceDescriptorSubtypes* unit_type){
    __TRAVERSE_UNIT__ {
        __INPUT_TERMINAL_MATCH__(driver, ac_interface, id, i, 
            *unit_type=INPUT_TERMINAL;return ac_interface->in_terminal[i]);
        __OUTPUT_TERMINAL_MATCH__(driver, ac_interface, id, i,
            *unit_type=OUTPUT_TERMINAL;return ac_interface->out_terminal[i]);
        __FEATURE_UNIT_MATCH__(driver, ac_interface, id, i,
            *unit_type=FEATURE_UNIT;return ac_interface->feature_unit[i]);
        __MIXER_UNIT_MATCH__(driver, ac_interface, id, i,
            *unit_type=MIXER_UNIT;return ac_interface->mixer_unit[i]);
        __SELECTOR_UNIT_MATCH__(driver, ac_interface, id, i,
            *unit_type=SELECTOR_UNIT;return ac_interface->selector_unit[i]);
        __EXTENSION_UNIT_MATCH__(driver, ac_interface, id, i,
            *unit_type=EXTENSION_UNIT;return ac_interface->extension_unit[i]);
        __PROCESSING_UNIT_MATCH__(driver, ac_interface, id, i,
            *unit_type=PROCESSING_UNIT;return ac_interface->processing_unit[i]);
    }
    *unit_type=AC_DESCRIPTOR_UNDEFINED;
    return (void*)0;
}

static inline uint8_t __get_unit_id(AudioDriver* driver, void* unit){
    __TYPE_CAST__(uint8_t*, raw, unit);
    return *(raw+3);
}

static inline uint8_t __get_unit_sub_type(AudioDriver* driver, void* unit){
    __TYPE_CAST__(uint8_t*, raw, unit);
    return *(raw+2);
}

static inline uint16_t __get_terminal_type(AudioDriver* driver, void* terminal){
    __TYPE_CAST__(uint8_t*, raw, terminal);
    return *(raw+4);
}

static inline uint8_t* __get_feature_bmControl_of_channel(AudioDriver* driver, 
    uint8_t channel_number, struct FeatureUnit* feature_unit){ 
    uint8_t available_controls = get_available_controls(driver, feature_unit);
    __IF_RET_NULL__(channel_number > available_controls);
    return *(feature_unit->feature_unit_desc.bmaControls + channel_number);
}

static inline int8_t __bmControl_supported(AudioDriver* driver, uint8_t* bmControl,
    enum FeatureUnitbmaControls control_bit){
    return __IF_EXT__((__IS_ZERO__((*bmControl & control_bit))), -1 , 1);
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

static inline int8_t __support_enable_processing_control(AudioDriver* driver, struct ProcessingUnit* processing_unit){
    __IF_RET_NEG__(__IS_ZERO__(processing_unit->processing_unit_desc.bControlSize));
    return __IF_EXT__(__IS_ZERO__((*(processing_unit->processing_unit_desc.bmControls) & ENABLE_PROCESSING)),
        -1, 1);
}

static inline int8_t __common_control(AudioDriver* driver, uint8_t bControlSize, uint8_t* bmControl, uint16_t control){
    __IF_RET_NEG__(__IS_ZERO__(bControlSize));
    return __IF_EXT__(__IS_ZERO__((*(bmControl) & control)), -1, 1);
}

static inline int8_t __support_mode_select_control(AudioDriver* driver, struct ProcessingUnit* processing_unit){
    __IF_RET_NEG__(processing_unit->processing_unit_desc.wProcessType != UP_DOWNMIX_PROCESS
        && processing_unit->processing_unit_desc.wProcessType != DOLBY_PROLOGIC_PROCESS);
    uint8_t  bControlSize;
    uint8_t* bmControl;
    if(processing_unit->processing_unit_desc.wProcessType == UP_DOWNMIX_PROCESS){
        __TYPE_CAST__(UP_DOWN_MixProcessingUnitDescriptor*, up_down, 
            processing_unit->processing_unit_desc.process_specific_descriptor);
        bControlSize = up_down->bControlSize; bmControl = up_down->bmControls;
    }
    else{
        __TYPE_CAST__(DolbyPrologicProcessingUnitDescriptor*, dolby, 
            processing_unit->processing_unit_desc.process_specific_descriptor);
        bControlSize = dolby->bControlSize; bmControl = dolby->bmControls;
    }
    return __common_control(driver, bControlSize, bmControl, MODE_SELECT);
}

static inline int8_t __support_spaciousness_control(AudioDriver* driver, struct ProcessingUnit* processing_unit){
    __IF_RET_NEG__(processing_unit->processing_unit_desc.wProcessType != STEREO_EXTENDER_PROCESS);
    return __common_control(driver, processing_unit->processing_unit_desc.bControlSize,
        processing_unit->processing_unit_desc.bmControls, SPACIOUSNESS);
}

static inline int8_t __reverb_control(AudioDriver* driver, enum ProcessingUnitProcessTypes type, 
    uint8_t bControlSize, uint8_t* bmControl, enum ReverbProcessingUnitControl control){
    __IF_RET_NEG__(type != REVERBERATION_PROCESS);
    return __common_control(driver, bControlSize, bmControl, control);
}

static inline int8_t __support_reverb_type_control(AudioDriver* driver, struct ProcessingUnit* processing_unit){
    return __reverb_control(driver, processing_unit->processing_unit_desc.wProcessType,
        processing_unit->processing_unit_desc.bControlSize, processing_unit->processing_unit_desc.bmControls,
        REVERB_TYPE); 
}

static inline int8_t __support_reverb_level_control(AudioDriver* driver, struct ProcessingUnit* processing_unit){
    return __reverb_control(driver, processing_unit->processing_unit_desc.wProcessType,
        processing_unit->processing_unit_desc.bControlSize, processing_unit->processing_unit_desc.bmControls,
        REVERB_LEVEL); 
}

static inline int8_t __support_reverb_time_control(AudioDriver* driver, struct ProcessingUnit* processing_unit){
    return __reverb_control(driver, processing_unit->processing_unit_desc.wProcessType,
        processing_unit->processing_unit_desc.bControlSize, processing_unit->processing_unit_desc.bmControls,
        REVERB_TIME); 
}

static inline int8_t __support_reverb_delay_feedback_control(AudioDriver* driver, struct ProcessingUnit* processing_unit){
    return __reverb_control(driver, processing_unit->processing_unit_desc.wProcessType,
        processing_unit->processing_unit_desc.bControlSize, processing_unit->processing_unit_desc.bmControls,
        REVERB_DELAY_FEEDBACK); 
}

static inline int8_t __chorus_control(AudioDriver* driver, enum ProcessingUnitProcessTypes type, uint8_t bControlSize,
    uint8_t* bmControl, enum ChorusProcessingUnitControl control){
    __IF_RET_NEG__(type != CHORUS_PROCESS);
    return __common_control(driver, bControlSize, bmControl, control);
}

static inline int8_t __support_chorus_level_control(AudioDriver* driver, struct ProcessingUnit* processing_unit){
    return __chorus_control(driver, processing_unit->processing_unit_desc.wProcessType,
        processing_unit->processing_unit_desc.bControlSize, processing_unit->processing_unit_desc.bmControls,
        CHORUS_LEVEL); 
}

static inline int8_t __support_chorus_modulation_rate_control(AudioDriver* driver, struct ProcessingUnit* processing_unit){
    return __chorus_control(driver, processing_unit->processing_unit_desc.wProcessType,
        processing_unit->processing_unit_desc.bControlSize, processing_unit->processing_unit_desc.bmControls,
        CHORUS_MODULATION_RATE); 
}

static inline int8_t __support_chorus_modulation_depth_control(AudioDriver* driver, struct ProcessingUnit* processing_unit){
    return __chorus_control(driver, processing_unit->processing_unit_desc.wProcessType,
        processing_unit->processing_unit_desc.bControlSize, processing_unit->processing_unit_desc.bmControls,
        CHORUS_MODULATION_DEPTH); 
}

static inline int8_t __dynamic_control(AudioDriver* driver, enum ProcessingUnitProcessTypes type, uint8_t bControlSize,
    uint8_t* bmControl, enum DynamicRangeUnitControl control){
    __IF_RET_NEG__(type != DYN_RANGE_COMP_PROCESS);
    return __common_control(driver, bControlSize, bmControl, control);
}

static inline int8_t __support_compression_ratio_control(AudioDriver* driver, struct ProcessingUnit* processing_unit){
    return __dynamic_control(driver, processing_unit->processing_unit_desc.wProcessType,
        processing_unit->processing_unit_desc.bControlSize, processing_unit->processing_unit_desc.bmControls,
        COMPRESSION_RATIO); 
}

static inline int8_t __support_max_amplitude_control(AudioDriver* driver, struct ProcessingUnit* processing_unit){
    return __dynamic_control(driver, processing_unit->processing_unit_desc.wProcessType,
        processing_unit->processing_unit_desc.bControlSize, processing_unit->processing_unit_desc.bmControls,
        MAX_AMPLITUDE); 
}

static inline int8_t __support_threshold_control(AudioDriver* driver, struct ProcessingUnit* processing_unit){
    return __dynamic_control(driver, processing_unit->processing_unit_desc.wProcessType,
        processing_unit->processing_unit_desc.bControlSize, processing_unit->processing_unit_desc.bmControls,
        THRESHOLD); 
}

static inline int8_t __support_attack_time_control(AudioDriver* driver, struct ProcessingUnit* processing_unit){
    return __dynamic_control(driver, processing_unit->processing_unit_desc.wProcessType,
        processing_unit->processing_unit_desc.bControlSize, processing_unit->processing_unit_desc.bmControls,
        ATTACK_TIME); 
}

static inline int8_t __support_release_time_control(AudioDriver* driver, struct ProcessingUnit* processing_unit){
    return __dynamic_control(driver, processing_unit->processing_unit_desc.wProcessType,
        processing_unit->processing_unit_desc.bControlSize, processing_unit->processing_unit_desc.bmControls,
        RELEASE_TIME); 
}

static inline void __parse_waModes(AudioDriver* driver, uint8_t* pos,
    uint8_t* number_modes, uint16_t* waModes){
    __FOR_RANGE__(i, int, 0, *number_modes){
        *(waModes + i) = *(((uint16_t*)pos) + i);
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
    selector_control_block->bSelector = selector_value;
}

static inline void __build_mixer_control_first_form(AudioDriver* driver, 
    MixerControlParameterBlockFirstForm* mixer_control_block, int16_t mixer_value){
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
    copy_protect_block->bCopyProtect = copy_protect;
}

static inline void __build_enable_processing_control_block(AudioDriver* driver,
    struct EnableProcessingControlParameterBlock* enable_processing_block, 
    uint8_t bEnable){
    enable_processing_block->bEnable = bEnable;
}

static inline void __build_mode_select_control_block(AudioDriver* driver,
    struct ModeSelectControlParameterBlock* mode_select_block, uint8_t bMode){
    mode_select_block->bMode = bMode;
}

static inline void __build_spaciousness_control_block(AudioDriver* driver,
    struct SpaciousnessControlParameterBlock* control_block, uint8_t value){
    control_block->bSpaciousness = value;
}

static inline void __build_reverb_type_control_block(AudioDriver* driver,
    struct ReverbTypeControlParameterBlock* reverb_control_block,
    uint8_t bReverbType){
    
}

static inline void __build_reverb_level_control_block(AudioDriver* driver,
    struct ReverbLevelControlParameterBlock* reverb_control_block,
    uint8_t bReverbLevel){
    reverb_control_block->bReverbLevel = bReverbLevel;
}

static inline void __build_chorus_level_control_block(AudioDriver* driver,
    struct ChorusLevelControlParameterBlock* chorus_control_block,
    uint8_t bChorusLevel){
    chorus_control_block->bChorusLevel = bChorusLevel;
}

static inline void __build_enable_extension_processing_control_block(AudioDriver* driver,
    struct EnableProcessingControlParameterBlock* control_block, uint8_t bOn){
    control_block->bEnable = bOn;
}

static inline void __build_volume_control_block(AudioDriver* driver, 
    VolumeControlParameterBlockFirstForm* volume_control_block, int16_t value){
    volume_control_block->wVolume = value;
}

static inline void __build_mute_control_block(AudioDriver* driver,
    MuteControlParameterBlockFirstForm* mute_control_block, uint8_t value){
    mute_control_block->bMute = value;
}

static inline void __build_pitch_control_block(AudioDriver* driver,
    struct PitchControlParameterBlock* pitch_control_block, uint8_t value){
    pitch_control_block->bPitchEnable = value;
}

void new_audio_driver(AudioDriver* driver, char* name, UsbDevice_ID* entry){
    __FOR_RANGE__(i, int, 0, MAX_DEVICES_PER_USB_DRIVER){
        driver->audio_map[i] = 0;
        driver->dev[i].usb_dev = 0;
        driver->dev[i].buffer_first = 0;
        driver->dev[i].buffer_second = 0;
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
    cluster->wChannelConfig = *((uint16_t*)(start+1));
    cluster->iChannelNames = (*start+3);
}

static void mute_control_callback(UsbDev *dev, Interface* interface, uint32_t status, void *data){
    __IF_RET__(status & E_TRANSFER);
    __TYPE_CAST__(struct MuteControlParameterBlockFirstForm*, mute_block, data);
    __TYPE_CAST__(AudioDev*, audio_dev, interface->data);
    audio_dev->muted = mute_block->bMute;
}

static void volume_change_callback(UsbDev* dev, Interface* interface, uint32_t status, void* data){
    __IF_RET__(status & E_TRANSFER);
    __TYPE_CAST__(AudioDev*, audio_dev, interface->data);
    __TYPE_CAST__(struct VolumeControlParameterBlockFirstForm*, volume_block, data);
    audio_dev->curr_volume = volume_block->wVolume;
}

static void volume_max_callback(UsbDev* dev, Interface* interface, uint32_t status, void* data){
    __IF_RET__(status & E_TRANSFER);
    __TYPE_CAST__(AudioDev*, audio_dev, interface->data);
    __TYPE_CAST__(struct VolumeControlParameterBlockFirstForm*, volume_block, data);
    audio_dev->max_volume = volume_block->wVolume;
}

static void volume_min_callback(UsbDev* dev, Interface* interface, uint32_t status, void* data){
    __IF_RET__(status & E_TRANSFER);
    __TYPE_CAST__(AudioDev*, audio_dev, interface->data);
    __TYPE_CAST__(struct VolumeControlParameterBlockFirstForm*, volume_block, data);
    audio_dev->min_volume = volume_block->wVolume;
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

static void* back_propagate_output_processing(AudioDriver* driver, ACInterface* ac_interface,
    struct OutputTerminal* output_terminal, enum ProcessingUnitProcessTypes search){
    __BACK_PROPAGATE_OUT_PROCESSING__(driver, output_terminal, ac_interface, search);
    return (void*)0;
}

static void* back_propagate_output(AudioDriver* driver, ACInterface* ac_interface,
    struct OutputTerminal* output_terminal, enum ACInterfaceDescriptorSubtypes search){
    if(search == OUTPUT_TERMINAL) return output_terminal;
    __BACK_PROPAGATE_OUT__(driver, output_terminal, ac_interface, search);
    return (void*)0;
}

static void* back_propagate_input(AudioDriver* driver, ACInterface* ac_interface,
    struct InputTerminal* input_terminal, enum ACInterfaceDescriptorSubtypes start,
    enum ProcessingUnitProcessTypes subtype){
    void** ac_units = (void**)0;
    uint8_t unit_count = 0;
    if(start == INPUT_TERMINAL) return input_terminal;
    uint8_t search_id = input_terminal->in_terminal_desc.bTerminalID;
    switch(start){
        case MIXER_UNIT : ac_units = (void**)ac_interface->mixer_unit; 
            unit_count = ac_interface->unit_count_map[MIXER_UNIT]; break;
        case SELECTOR_UNIT : ac_units = (void**)ac_interface->selector_unit;
            unit_count = ac_interface->unit_count_map[SELECTOR_UNIT]; break;
        case FEATURE_UNIT : ac_units = (void**)ac_interface->feature_unit;
            unit_count = ac_interface->unit_count_map[FEATURE_UNIT]; break;
        case PROCESSING_UNIT : ac_units = (void**)ac_interface->processing_unit;
            unit_count = ac_interface->unit_count_map[PROCESSING_UNIT]; break;
        case EXTENSION_UNIT : ac_units = (void**)ac_interface->extension_unit;
            unit_count = ac_interface->unit_count_map[EXTENSION_UNIT]; break;
        case OUTPUT_TERMINAL : ac_units = (void**)ac_interface->out_terminal;
            unit_count = ac_interface->unit_count_map[OUTPUT_TERMINAL]; break;
        default : return (void*)0;
    }
    // now back prop
    __FOR_RANGE__(i, int, 0, unit_count){
        __IF_CONTINUE__((start == PROCESSING_UNIT) &&  (__STRUCT_CALL__(driver, 
            retrieve_process_type, (uint8_t*)(ac_units[i])) != subtype));
        __BACK_PROPAGATE_IN__(driver, start, ac_units[i], ac_interface, search_id);
    }
    return (void*)0;
}

static uint8_t get_available_controls(AudioDriver* driver,
    struct FeatureUnit* feature_unit){
    uint8_t size = feature_unit->feature_unit_desc.bLength;
    uint8_t ctl_size = feature_unit->feature_unit_desc.bControlSize;
    return (size - 7) / ctl_size;
}

static uint8_t get_upstream_channels_by_source_id_depth(AudioDriver* driver,
    uint8_t bSourceID, ACInterface* ac_interface, uint8_t depth){
    __IF_RET_ZERO__(depth == 0);
    __TRAVERSE_UNIT__{
        __OUTPUT_TERMINAL_MATCH__(driver, ac_interface, bSourceID, i, 
            return get_upstream_channels_by_source_id_depth(driver, 
            ac_interface->out_terminal[i]->out_terminal_desc.bSourceID, ac_interface, depth-1));
        __SELECTOR_UNIT_MATCH__(driver, ac_interface, bSourceID, i, 
            return get_upstream_channels_by_source_id_depth(driver,
                *(ac_interface->selector_unit[i]->selector_unit_desc.baSourceID), ac_interface, depth-1));
        __FEATURE_UNIT_MATCH__(driver, ac_interface, bSourceID, i, 
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

// find_structure can also be used directly, giving the id to search for
static void* find_structure_by_id(AudioDriver* driver, ACInterface* ac_interface,
    uint8_t bSourceID, enum ACInterfaceDescriptorSubtypes* unit_type){
    __TRAVERSE_UNIT__ {
        __INPUT_TERMINAL_MATCH__(driver, ac_interface, bSourceID, i, 
            *unit_type=INPUT_TERMINAL;return ac_interface->in_terminal[i]);
        __OUTPUT_TERMINAL_MATCH__(driver, ac_interface, bSourceID, i,
            *unit_type=OUTPUT_TERMINAL;return ac_interface->out_terminal[i]);
        __FEATURE_UNIT_MATCH__(driver, ac_interface, bSourceID, i,
            *unit_type=FEATURE_UNIT;return ac_interface->feature_unit[i]);
        __MIXER_UNIT_MATCH__(driver, ac_interface, bSourceID, i,
            *unit_type=MIXER_UNIT;return ac_interface->mixer_unit[i]);
        __SELECTOR_UNIT_MATCH__(driver, ac_interface, bSourceID, i,
            *unit_type=SELECTOR_UNIT;return ac_interface->selector_unit[i]);
        __EXTENSION_UNIT_MATCH__(driver, ac_interface, bSourceID, i,
            *unit_type=EXTENSION_UNIT;return ac_interface->extension_unit[i]);
        __PROCESSING_UNIT_MATCH__(driver, ac_interface, bSourceID, i,
            *unit_type=PROCESSING_UNIT;return ac_interface->processing_unit[i]);
    }
    *unit_type=AC_DESCRIPTOR_UNDEFINED;
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
    uint8_t output_channel){
    uint8_t output_channels = __STRUCT_CALL__(driver, 
        __get_total_channels_in_cluster, mixer_unit->mixer_unit_desc.cluster);
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
    __IF_ELSE__(((input_channels * output_channels) % 8) != 0, 
        N = ((input_channels * output_channels) / 8) + 1,
        N = ((input_channels* output_channels) / 8));
    return N;
}

static int selector_build_routine(AudioDriver* driver, uint8_t* start,
  ACInterface* ac_interface, MemoryService_C* m){
  __IF_RET_NEG__(__UNIT_COUNT__(ac_interface, SELECTOR_UNIT));
  __ALLOC_KERNEL_MEM_S__(m, struct SelectorUnit, selector_unit);
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
  selector_unit->selector_unit_desc = selector_unit_desc;
  __ADD_UNIT__(ac_interface, selector_unit, SELECTOR_UNIT, selector_unit);

  return __RET_S__;
}

static int feature_build_routine(AudioDriver* driver, uint8_t* start,
  ACInterface* ac_interface, MemoryService_C* m){
  __IF_RET_NEG__(__UNIT_COUNT__(ac_interface, FEATURE_UNIT));
  __ALLOC_KERNEL_MEM_S__(m, struct FeatureUnit, feature_unit);
  uint8_t control_size = *(start+5);
  __parse_common_unit_descriptor_part(driver, start, &feature_unit->feature_unit_desc.bLength,
    &feature_unit->feature_unit_desc.bDescriptorType, &feature_unit->feature_unit_desc.bDescriptorSubtype,
    &feature_unit->feature_unit_desc.bUnitID);
  feature_unit->feature_unit_desc.bSourceID = *(start+4);
  feature_unit->feature_unit_desc.bControlSize = control_size;
  uint8_t available_controls = get_available_controls(driver, feature_unit); 
  __parse_bmaControls(driver, &feature_unit->feature_unit_desc.bmaControls, &control_size,
    m, available_controls, start);
  feature_unit->feature_unit_desc.iFeature = *(start + 6 + 
    (available_controls * control_size));
  __ADD_UNIT__(ac_interface, feature_unit, FEATURE_UNIT, feature_unit);
  
  return __RET_S__;
}

static void* up_down_mix_processing_unit_build_routine(AudioDriver* driver,
    uint8_t* start, MemoryService_C* m){
    __EXT_SPECIFIC_PROCESSING_UNIT_ROUTINE__(driver, start,
      UP_DOWN_MixProcessingUnitDescriptor, m);
    return processing_unit_desc;
}

static void* dolby_prologic_processing_unit_build_routine(AudioDriver* driver,
    MemoryService_C* m, uint8_t* start){
    __EXT_SPECIFIC_PROCESSING_UNIT_ROUTINE__(driver, start,
      DolbyPrologicProcessingUnitDescriptor, m);
    return processing_unit_desc;
}

static void* stereo_extender_processing_unit_build_routine(AudioDriver* driver,
    MemoryService_C* m, uint8_t* start){
    __SIMPLE_SPECIFIC_PROCESSING_UNIT_ROUTINE__(driver, start,
      THREE_D_StereoExtenderProcessingUnitDescriptor, m);
    return processing_unit_desc;
}

static void* reverberation_processing_unit_build_routine(AudioDriver* driver,
    MemoryService_C* m, uint8_t* start){
    __SIMPLE_SPECIFIC_PROCESSING_UNIT_ROUTINE__(driver, start,
      ReverberationProcessingUnitDescriptor, m);
    return processing_unit_desc;
}

static void* chorus_processing_unit_build_routine(AudioDriver* driver, 
    MemoryService_C* m, uint8_t* start){
    __SIMPLE_SPECIFIC_PROCESSING_UNIT_ROUTINE__(driver, start,
      ChorusProcessingUnitDescriptor, m);
    return processing_unit_desc;
}

static void* dynamic_range_compressor_processing_unit_build_routine(AudioDriver* driver, 
    MemoryService_C* m, uint8_t* start){
    __SIMPLE_SPECIFIC_PROCESSING_UNIT_ROUTINE__(driver, start,
      DynamicRangeCompressorProcessnigUnitDescriptor, m);
    return processing_unit_desc;
}

static void parse_process_specific_descriptor(AudioDriver* driver, 
    struct ProcessingUnit* process_unit, enum ProcessingUnitProcessTypes wProcessType,
    uint8_t* pos, MemoryService_C* m){
    void* specific = (void*)0;    
    switch(wProcessType){
        case UP_DOWNMIX_PROCESS : 
            specific = up_down_mix_processing_unit_build_routine(driver, pos, m); 
            process_unit->ext = 1; break;
        case DOLBY_PROLOGIC_PROCESS : 
            specific = dolby_prologic_processing_unit_build_routine(driver, m, pos); 
            process_unit->ext = 1; break;
        case STEREO_EXTENDER_PROCESS : 
            specific = stereo_extender_processing_unit_build_routine(driver, m, pos); 
            process_unit->ext = 0; break;
        case REVERBERATION_PROCESS : 
            specific = reverberation_processing_unit_build_routine(driver, m, pos); 
            process_unit->ext = 0; break;
        case CHORUS_PROCESS : 
            specific = chorus_processing_unit_build_routine(driver, m, pos); 
            process_unit->ext = 0; break;
        case DYN_RANGE_COMP_PROCESS : 
            specific = dynamic_range_compressor_processing_unit_build_routine(driver, m, pos);
            process_unit->ext = 0;
        default : break;
    };
    __assign_process(driver, specific, &process_unit->processing_unit_desc);
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
    processing_unit->processing_unit_desc = processing_unit_desc;    
    parse_process_specific_descriptor(driver, processing_unit, 
        processing_unit_desc.wProcessType, start+13+processing_unit_desc.bNrInPins+
        processing_unit_desc.bControlSize, m);
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
    return 0;
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

static uint8_t __get_terminal(AudioDriver* driver, AudioDev* audio_dev, 
    ASInterface* as_interface){
    uint8_t terminal_id = as_interface->as_desc.bTerminalLink;
    enum ACInterfaceDescriptorSubtypes ac_subtype;
    ACInterface* ac_interface = (ACInterface*)audio_dev->audio_control_interface->active_interface->class_specific;
    find_structure_by_id(driver, ac_interface, terminal_id, &ac_subtype);

    return (uint8_t)ac_subtype;
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

static ASInterface* as_routine(AudioDriver* driver, AudioDev* audio_dev, uint8_t* start,
    MemoryService_C* m, Alternate_Interface* alt_itf){
  
  __ALLOC_KERNEL_MEM_S__(m, ASInterface, as_interface);
  as_interface->as_desc = *((struct AudioStreamingDescriptor*)start);
  as_interface->bPitchEnable = 0;
  as_interface->current_freq = (SampleFrequency){0, 0};
  as_interface->format_type = 0;
  __set_class_specific_interface(driver, alt_itf, as_interface);
  uint8_t terminal_type = __get_terminal(driver, audio_dev, as_interface);
  as_interface->terminal_type = terminal_type;

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
    if((*((*start) + 1) == INTERFACE) && (*((*start) + 6) == AUDIO_STREAMING)){
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
        as_interface = as_routine(driver, &audio_dev, *start, m, alt_itf);
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

static enum FormatTypeCodes get_format_type1(void* type_descriptor){
    __TYPE_CAST__(struct Type1_FormatTypeDescriptor*, type1, type_descriptor);
    return type1->bFormatType;
}

static uint8_t get_total_channels_type_1(void* type_descriptor){
    __TYPE_CAST__(struct Type1_FormatTypeDescriptor*, type1, type_descriptor);
    return type1->bNrChannels;
}

static uint8_t get_subframe_size_type_1(void* type_descriptor){
    __TYPE_CAST__(struct Type1_FormatTypeDescriptor*, type1, type_descriptor);
    return type1->bSubframeSize;
}

static uint8_t get_bith_depth_type_1(void* type_descriptor){
    __TYPE_CAST__(struct Type1_FormatTypeDescriptor*, type1, type_descriptor);
    return type1->bBitResolution;
}

static uint8_t get_sam_freq_type_1(void* type_descriptor){
    __TYPE_CAST__(struct Type1_FormatTypeDescriptor*, type1, type_descriptor);
    return type1->bSamFreqType;
}

// num : from 1 to x
static SampleFrequency get_sam_freq_discrete_type_1(void* type_descriptor, uint8_t num){
    __TYPE_CAST__(struct Type1_FormatTypeDescriptor*, type1, type_descriptor);
    __TYPE_CAST__(struct DiscreteNumberSamplingFrequencies*, sam_freq, type1->samFreq);
    if(num > get_sam_freq_type_1(type_descriptor)) 
        return __DEFAULT_STRUCT__(SampleFrequency, 0, 0);
    return *(sam_freq->tSam + num-1);    
}

// 0, 1 : 0 for lower bound, 1 for upper bound
static SampleFrequency get_sam_freq_cont_type_1(void* type_descriptor, uint8_t num){
    __TYPE_CAST__(struct Type1_FormatTypeDescriptor*, type1, type_descriptor);
    __TYPE_CAST__(struct ContinuousSamplingFrequency*, sam_freq, type1->samFreq);
    if(__IS_ZERO__(num)) return sam_freq->tLower;
    else if(num == 1) return sam_freq->tUpper;

    return __DEFAULT_STRUCT__(SampleFrequency, 0, 0);
}

static int handle_format_type_1(AudioDriver* driver, uint8_t* start,
    MemoryService_C* m, ASInterface* as_interface){
    uint8_t* pos = start + 8;
    void* samFreq = 0;
    SampleFrequency (*get_sam_frequency)(void* t_d, uint8_t num);
    __TYPE_CAST__(struct Type1_FormatTypeDescriptor*, type1, start);
    if(type1->bSamFreqType == 0){
        samFreq = __parse_contig_sample_freq(driver, pos, m);
        get_sam_frequency = &get_sam_freq_cont_type_1;
    }
    else if((type1->bSamFreqType) >= 1 && (type1->bSamFreqType <= 255)){
        samFreq = __parse_discrete_sample_freq(driver, pos, type1->bSamFreqType, m);
        get_sam_frequency = &get_sam_freq_discrete_type_1;
    }
    if(__IS_NULL__(samFreq)) return -1;
    __ALLOC_KERNEL_MEM_S__(m, struct Type1_FormatTypeDescriptor, type1_descriptor);
    *type1_descriptor = *type1;
    type1_descriptor->samFreq = samFreq;
    __ALLOC_KERNEL_MEM_S__(m, struct FormatType, format_type);
    format_type->type_descriptor = type1_descriptor;
    format_type->get_format_type = &get_format_type1;
    format_type->get_total_channels = &get_total_channels_type_1;
    format_type->get_subframe_size = &get_subframe_size_type_1;
    format_type->get_bit_depth = &get_bith_depth_type_1;
    format_type->get_sam_freq_type = &get_sam_freq_type_1;
    format_type->get_sam_frequency = get_sam_frequency;
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

static int8_t is_valid_frequency(AudioDriver* driver, Interface* itf,
    AudioDev* audio_dev, struct SampleFrequency frequency){
    ASInterface* as_interface = __STRUCT_CALL__(driver, 
        __get_class_specific_as_interface, audio_dev->usb_dev, itf);
    struct FormatType* format_type = as_interface->format_type;
    if(format_type->get_format_type(format_type->type_descriptor) == FORMAT_TYPE_1){
        struct Type1_FormatTypeDescriptor* type1 = (struct Type1_FormatTypeDescriptor*)as_interface->format_type->type_descriptor;
        struct DiscreteNumberSamplingFrequencies* discrete = (struct DiscreteNumberSamplingFrequencies*)type1->samFreq;
        SampleFrequency* sample_freq = discrete->tSam;
        __FOR_RANGE__(t, int, 0, type1->bSamFreqType){
            if(__STRUCT_CALL__(driver, __match_frequency, sample_freq+t, &frequency)){
                return __RET_S__;
            }
        }
    }
    return __RET_E__;
}

static int8_t sync_streaming_interface(AudioDriver* driver, UsbDev* dev,
    Interface* interface){
    Endpoint* endpoint = __STRUCT_CALL__(dev, 
        __get_first_endpoint, interface->active_interface);
    ASInterface* as_interface = __STRUCT_CALL__(driver, 
        __convert_to_class_specific_as_interface, interface);
    __IF_RET_NEG__(__IS_NULL__(endpoint));
    uint16_t delay = __STRUCT_CALL__(driver, __get_sync_delay, endpoint);
    LockDelayUnit delay_unit = __STRUCT_CALL__(driver, __get_sync_unit, endpoint);
    uint16_t time_in_ms;
    if(delay_unit == LOCK_UNIT_UNDEFINED) return __RET_E__;
    else if(delay_unit == LOCK_PCM_SAMPLES){
        uint16_t samples_per_1ms = __STRUCT_CALL__(driver, 
            __get_1ms_size, as_interface);
        time_in_ms = delay / samples_per_1ms;
        time_in_ms += (delay % samples_per_1ms == 0) ? 0 : 1;
    }
    else if(delay_unit == LOCK_UNIT_MS){
        time_in_ms = delay;
    }
    mdelay(time_in_ms);

    return __RET_S__;
}

static void request_device_attributes(AudioDriver* driver, AudioDev* audio_dev,
    Interface* itf){
    __STRUCT_CALL__(driver, get_sampling_frequency, audio_dev, itf);
    __STRUCT_CALL__(driver, get_min_volume, audio_dev, itf);
    __STRUCT_CALL__(driver, get_mute_attribute, audio_dev, itf);
    __STRUCT_CALL__(driver, get_max_volume, audio_dev, itf);
    __STRUCT_CALL__(driver, get_current_volume, audio_dev, itf);
}

static void configure_interfaces(AudioDriver* driver, AudioDev* audio_dev){
    Interface** streaming_interfaces = audio_dev->audio_streaming_interfaces;
    __FOR_RANGE__(i, int, 0, audio_dev->audio_streaming_interfaces_num){
        /*__STRUCT_CALL__(driver, switch_if_zero_bandwidth, audio_dev, 
            streaming_interfaces[i]); */
        mdelay(50);
        __STRUCT_CALL__(driver, request_device_attributes, audio_dev,
            streaming_interfaces[i]);
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
        __STRUCT_CALL__(driver, audio_device_routine, start, end, mem_service, i);
        //__STRUCT_CALL__(driver, configure_interfaces, driver->dev + i);
        __STRUCT_CALL__(driver, switch_if_zero_bandwidth, driver->dev + i);
        for(int j = 0; j < driver->dev[i].audio_streaming_interfaces_num; j++){
            Interface* as_interface = driver->dev[i].audio_streaming_interfaces[j];
            request_device_attributes(driver, driver->dev + i, as_interface);
        }
    }
    return __RET_S__;
}

static int8_t feature_control(AudioDriver* driver, uint8_t control, 
    uint8_t request_form, AudioDev* audio_dev, Interface* interface, 
    uint8_t bRequest, uint8_t channel_number, void* data, 
    uint8_t len, callback_function callback, uint8_t flags){
    __FEATURE_REQUEST_PROTO__();
    UsbDev* dev = audio_dev->usb_dev;
    UsbDeviceRequest* device_req = dev->get_free_device_request(dev);
    __IF_RET_NEG__(__IS_NULL__(device_req));
    struct FeatureUnit* feature_unit = __RECOVER_FEATURE_UNIT__(driver, 
        audio_dev, interface);
    __IF_RET_NEG__(__IS_NULL__(feature_unit));
    uint8_t audio_interface = dev->__interface_number(dev, 
        audio_dev->audio_control_interface->active_interface);
    __IF_ELSE__(request_form == __SET_REQUEST__,
        __request_feature_ = &__request_feature_set,
        __request_feature_ = &__request_feature_get);
    __request_feature_(driver, dev, interface, device_req, bRequest, control,
        channel_number, feature_unit->feature_unit_desc.bUnitID, 
        audio_interface, data, len, callback, flags);

    return __RET_S__;
}

static SpatialLocations get_loc_by_channel_number(AudioDriver* driver, 
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
static int16_t get_channel_number_by_loc(AudioDriver* driver, ClusterDescriptor cluster,
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

/* @deprecated static uint8_t get_total_controls(AudioDriver* driver,
    struct FeatureUnit* feature_unit, 
    int8_t (*control_supported)(AudioDriver* driver, uint8_t* bmControl)){
    uint8_t upstream_ch = get_available_controls(driver, 
        feature_unit);
    uint8_t tot_size = 0;
    __FOR_RANGE__(i, int, 0, upstream_ch){
        uint8_t* bmControl = __get_feature_bmControl_of_channel(driver, 
            i, feature_unit);
        if(control_supported(driver, bmControl) == 1)
            tot_size++;   
    }

    return tot_size;
} */
 
static void* recover_unit_from_as_interface(AudioDriver* driver,
    AudioDev* audio_dev, Interface* interface, enum ACInterfaceDescriptorSubtypes search,
    enum ProcessingUnitProcessTypes process_sub_type){
    enum ACInterfaceDescriptorSubtypes subtype;
    void* terminal = (void*)0;
    void* unit     = (void*)0;
    __IF_RET_NULL__(__IS_NULL__(audio_dev->audio_control_interface));
    ASInterface* as_interface = driver->__convert_to_class_specific_as_interface(
        driver, interface);
    __IF_RET_NULL__(__IS_NULL__(as_interface));
    uint8_t terminal_type = as_interface->terminal_type;
    ACInterface* ac_interface = driver->__convert_to_class_specific_ac_interface(driver, 
        audio_dev->audio_control_interface);
    terminal = find_structure_by_id(driver, ac_interface,
            as_interface->as_desc.bTerminalLink, &subtype);
    if(terminal_type == INPUT_TERMINAL){
        unit = back_propagate_input(driver, ac_interface, terminal, search, 
            process_sub_type);
    }
    else if(terminal_type == OUTPUT_TERMINAL){
        if(search == PROCESSING_UNIT)
            unit = back_propagate_output_processing(driver, ac_interface,
                terminal, process_sub_type);
        else 
            unit = back_propagate_output(driver, ac_interface, 
                terminal, search);
    }
    return unit;
}

/* @deprecated static uint8_t* build_mute_control_block(AudioDriver* driver,
    MemoryService_C* m, struct FeatureUnit* feature_unit, uint8_t bMute,
    uint8_t* mute_block_size){
    uint8_t tot_size = get_total_controls(driver, feature_unit, 
        &__bmControl_mute_supported);
    __IF_RET_NULL__(__IS_ZERO__(tot_size));
    __MAP_IO_KERNEL_T__(m, uint8_t, mute_block, tot_size);
    __FOR_RANGE__(i, int, 0, tot_size){
        mute_block[i] = bMute;
    }
    *mute_block_size = tot_size;
    return mute_block;
} */

/* @deprecated static int16_t* build_volume_control_block(AudioDriver* driver, 
    MemoryService_C* m, struct FeatureUnit* feature_unit, int16_t wVolume,
    uint8_t* vol_block_size){
    uint8_t tot_size = get_total_controls(driver, feature_unit,
        &__bmControl_volume_supported);
    __IF_RET_NULL__(__IS_ZERO__(tot_size));
    __MAP_IO_KERNEL_T__(m, int16_t, volume_block, tot_size);
    __FOR_RANGE__(i, int, 0, tot_size){
        volume_block[i] = wVolume;
    }
    *vol_block_size = tot_size;
    return volume_block;
} */

static int8_t audio_mute_control(AudioDriver* driver, AudioDev* audio_dev,
    Interface* interface, uint8_t request_form, uint8_t bRequest, 
    uint8_t channel_number, callback_function callback, uint8_t flags, uint8_t bMute){
    MemoryService_C* mem_service = __DEV_MEMORY(audio_dev->usb_dev);
    
    __MAP_IO_KERNEL_S__(mem_service, struct MuteControlParameterBlockFirstForm, mute_block);
    __build_mute_control_block(driver, mute_block, bMute);
    
    feature_control(driver, MUTE_CONTROL, request_form, audio_dev, interface, bRequest,
        channel_number, mute_block,
        sizeof(struct MuteControlParameterBlockFirstForm), callback, 0);

    return __RET_S__;
}

// + 127.9961 dB (0x7FFF) : -127.9961 dB (0x8001) , 1/256 dB (0x0001)step
// CUR : 0x8000 represents silence
// RES : 1/256 dB (0x0001) : +127.9961 dB (0x7FFF)
static int8_t audio_volume_control(AudioDriver* driver, AudioDev* audio_dev,
    Interface* interface, uint8_t request_form, uint8_t bRequest, 
    uint8_t channel_number, callback_function callback, uint8_t flags, 
    int16_t wVolume){
    MemoryService_C* mem_service = __DEV_MEMORY(audio_dev->usb_dev);
    
    //int16_t* volume_block = build_volume_control_block(driver, 
        //mem_service, feature_unit, wVolume, &len);
    __MAP_IO_KERNEL_S__(mem_service, struct VolumeControlParameterBlockFirstForm, volume_block);
    __build_volume_control_block(driver, volume_block, wVolume);
    
    feature_control(driver, VOLUME_CONTROL, request_form, audio_dev, interface, bRequest,
        channel_number, volume_block,
        sizeof(struct VolumeControlParameterBlockFirstForm), callback, flags);

    return __RET_S__;
}

static void mixer_callback(UsbDev* dev, Interface* interface, uint32_t status, void* data){
    // mixer transfer checks
}

// range from +127.9961 dB (0x7FFF) : -127.9961 dB (0x8001)
// 0x8000 for cur req : silence request
// res reqest range : 0x0001 - 0x7FFF
static int8_t audio_mixer_control(AudioDriver* driver, AudioDev* audio_dev,
    Interface* interface, uint8_t request_form, uint8_t bRequest, 
    uint8_t icn, uint8_t ocn, int16_t wMixer){
    // check if icn and ocn are valid channels in the audio device
    __MIXER_REQUEST_PROTO__(); 
    UsbDev* dev = audio_dev->usb_dev;
    struct MixerUnit* mixer_unit = __RECOVER_MIXER_UNIT__(driver, 
        audio_dev, interface);
    __IF_RET_NEG__(__IS_NULL__(mixer_unit));
    __IF_RET_NEG__(__STRUCT_CALL__(driver, 
        is_bm_control_programmable, mixer_unit, icn, ocn) <= 0); 
    UsbDeviceRequest* device_req = dev->get_free_device_request(dev);
    __IF_RET_NEG__(__IS_NULL__(device_req));
    MemoryService_C* mem_service = __DEV_MEMORY(audio_dev->usb_dev);
    __MAP_IO_KERNEL_S__(mem_service, MixerControlParameterBlockFirstForm, 
        mixer_control_block);
    __build_mixer_control_first_form(driver, mixer_control_block, wMixer);
    uint8_t audio_interface = dev->__interface_number(dev, 
        audio_dev->audio_control_interface->active_interface);

    __IF_ELSE__(request_form == __SET_REQUEST__,
        __request_mixer_ = &__request_mixer_set,
        __request_mixer_ = &__request_mixer_get);

    __request_mixer_(driver, dev, interface, device_req, bRequest, 
        icn, ocn, mixer_unit->mixer_unit_desc.bUnitID, audio_interface,
        mixer_control_block, sizeof(MixerControlParameterBlockFirstForm), 
        &mixer_callback, 0);
    
    return __RET_S__;
}


static int8_t set_mixer_unit_control(AudioDriver* driver, AudioDev* audio_dev,
    Interface* interface, uint8_t icn, uint8_t ocn, int16_t wMixer){
    
    return audio_mixer_control(driver, audio_dev, interface, __SET_REQUEST__,
        SET_CUR, icn, ocn, wMixer);
}

static int8_t get_mixer_unit_control(AudioDriver* driver, AudioDev* audio_dev,
    Interface* interface, uint8_t icn, uint8_t ocn){
    return audio_mixer_control(driver, audio_dev, interface, __GET_REQUEST__,
        GET_CUR, icn, ocn, 0);
}

static void selector_control_callback(UsbDev* dev, Interface* interface, uint32_t status, void* data){
    __IF_RET__(status & E_TRANSFER);
}

// the res request should not be supported 
static int8_t audio_selector_control(AudioDriver* driver, AudioDev* audio_dev,
    Interface* interface, uint8_t request_form, uint8_t bRequest, uint8_t bSelector){
    __COMMON_REQUEST_PROTO__(selector);
    UsbDev* dev = audio_dev->usb_dev;
    UsbDeviceRequest* device_req = dev->get_free_device_request(dev);
    __IF_RET_NEG__(__IS_NULL__(device_req));
    struct SelectorUnit* selector_unit = __RECOVER_SELECTOR_UNIT__(
        driver, audio_dev, interface);
    __IF_RET_NEG__(__IS_NULL__(selector_unit));
    MemoryService_C* mem_service = __DEV_MEMORY(audio_dev->usb_dev);
    __MAP_IO_KERNEL_S__(mem_service, SelectorControlParameterBlock, selector_block);
    __build_selector_control_block(driver, selector_block, bSelector);
    uint8_t audio_interface = dev->__interface_number(dev, 
        audio_dev->audio_control_interface->active_interface);
    __IF_ELSE__(request_form == __SET_REQUEST__,
        __request_common_selector = &__request_common_set,
        __request_common_selector = &__request_common_get);
    
    __request_common_selector(driver, dev, interface, device_req,
        bRequest, 0, audio_interface, selector_unit->selector_unit_desc.bUnitID,
        selector_block, sizeof(SelectorControlParameterBlock), 
        &selector_control_callback, 0);

    return __RET_S__;
}

static int8_t selector_set_cur(AudioDriver* driver, AudioDev* audio_dev, 
    Interface* interface, uint8_t bSelector){
    
    return audio_selector_control(driver, audio_dev, interface, 
        __SET_REQUEST__, SET_CUR, bSelector);
}

static void processing_callback(UsbDev* dev, Interface* interface, uint32_t status, void* data){
    __IF_RET__(status & E_TRANSFER);
}

static int8_t audio_processing_control(AudioDriver* driver, AudioDev* audio_dev,
    Interface* interface, uint8_t unit_id, uint8_t request_form, uint8_t bRequest, 
    uint8_t control, void* data, uint8_t len, callback_function callback,
    uint8_t flags){
    __COMMON_REQUEST_PROTO__(processing);
    UsbDev* dev = audio_dev->usb_dev;
    UsbDeviceRequest* device_req = dev->get_free_device_request(dev);
    __IF_RET_NEG__(__IS_NULL__(device_req));

    uint8_t audio_interface = dev->__interface_number(dev, 
        audio_dev->audio_control_interface->active_interface);
    __IF_ELSE__(request_form == __SET_REQUEST__,
        __request_common_processing = &__request_common_set,
        __request_common_processing = &__request_common_get);

    __request_common_processing(driver, dev, interface, device_req, 
        bRequest, control, audio_interface, unit_id, data, len, callback, 
        flags);

    return __RET_S__;
}

static void extension_callback(UsbDev* dev, Interface* interface, uint32_t status, void* data){

}

static int8_t audio_extension_control(AudioDriver* driver, AudioDev* audio_dev,
    Interface* interface, uint8_t request_form, uint8_t bRequest, uint8_t bOn){
    __COMMON_REQUEST_PROTO__(extension);
    UsbDev* dev = audio_dev->usb_dev;
    UsbDeviceRequest* device_req = dev->get_free_device_request(dev);
    __IF_RET_NEG__(__IS_NULL__(device_req));
    struct ExtensionUnit* ext_unit = __RECOVER_EXTENSION_UNIT__(driver, 
        audio_dev, interface);
    __IF_RET_NEG__(__IS_NULL__(ext_unit));
    MemoryService_C* mem_service = __DEV_MEMORY(audio_dev->usb_dev);
    struct EnableProcessingControlParameterBlock* enable_processing_block = 
    __MAP_IO_KERNEL__(mem_service, struct EnableProcessingControlParameterBlock, 
        sizeof(struct EnableProcessingControlParameterBlock));
    __build_enable_extension_processing_control_block(driver, 
        enable_processing_block, bOn);
    uint8_t audio_interface = dev->__interface_number(dev, 
        audio_dev->audio_control_interface->active_interface);
    __IF_ELSE__(request_form == __SET_REQUEST__,
        __request_common_extension = &__request_common_set,
        __request_common_extension = &__request_common_get);
    
    __request_common_extension(driver, dev, interface, device_req,
        bRequest, XU_ENABLE_CONTROL, audio_interface, 
        ext_unit->extension_unit_desc.bUnitID, enable_processing_block,
        sizeof(struct EnableProcessingControlParameterBlock), &extension_callback, 0);

    return __RET_S__;
}

// set cur only
// control selector high byte & 0 low byte
// processing_unit high byte & interface number low byte
static int8_t audio_enable_processing_control(AudioDriver* driver, 
    AudioDev* audio_dev, Interface* interface, uint8_t request_form, 
    uint8_t bRequest, uint8_t bEnable){
    MemoryService_C* mem_service = __DEV_MEMORY(audio_dev->usb_dev);

    // pass subtype when using this request
    struct ProcessingUnit* processing_unit = __RECOVER_PROCESSING_UNIT__(driver, 
        audio_dev, interface, 0);
    __IF_RET_NEG__(__IS_NULL__(processing_unit));
    __IF_RET_NEG__(__IS_NEG_ONE__(__support_enable_processing_control(
        driver, processing_unit)));

    struct EnableProcessingControlParameterBlock* enable_processing_control_block = 
        __MAP_IO_KERNEL__(mem_service, struct EnableProcessingControlParameterBlock,
        sizeof(struct EnableProcessingControlParameterBlock));
    __build_enable_processing_control_block(driver, enable_processing_control_block,
        bEnable);

    return audio_processing_control(driver, audio_dev, interface, 
        processing_unit->processing_unit_desc.bUnitID, request_form, 
        bRequest, UD_ENABLE_CONTROL, enable_processing_control_block, 
        sizeof(struct EnableProcessingControlParameterBlock), &processing_callback, 0);
}

static Endpoint* get_iso_endpoint(AudioDriver* driver, AudioDev* audio_dev,
    Interface* as_interface){
    Endpoint* endpoint;
    if(__STRUCT_CALL__(driver, __is_zero_bandwidth_active, audio_dev->usb_dev, as_interface) == 1){
        Alternate_Interface* alt_itf = __STRUCT_CALL__(audio_dev->usb_dev, 
            __get_alternate_interface_by_setting, as_interface, 1);
        endpoint = audio_dev->usb_dev->__get_first_endpoint(audio_dev->usb_dev,
            alt_itf); 
    }
    else 
        endpoint = audio_dev->usb_dev->__get_first_endpoint(audio_dev->usb_dev,
            as_interface->active_interface);

    return endpoint;
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
    
    Endpoint* endpoint = __STRUCT_CALL__(driver, get_iso_endpoint, audio_dev, as_interface);

    ASEndpoint* as_endpoint = __STRUCT_CALL__(driver, 
        __convert_to_class_specific_as_endpoint, endpoint);
    return __IF_EXT__((__IS_ZERO__((as_endpoint->as_iso_endpoint.bmAttributes & BIT_SAMPLING_FREQ))), -1, 1);
}

static int8_t support_pitch_control(AudioDriver* driver, AudioDev* audio_dev, 
    Interface* as_interface){
    __IF_RET_NEG__(__IS_NEG_ONE__(contains_audio_streaming_interface(driver,
        audio_dev, as_interface)));

    Endpoint* endpoint = __STRUCT_CALL__(driver, get_iso_endpoint, audio_dev, as_interface);
 
    ASEndpoint* as_endpoint = __STRUCT_CALL__(driver, 
        __convert_to_class_specific_as_endpoint, endpoint);
    return __IF_EXT__((__IS_ZERO__((as_endpoint->as_iso_endpoint.bmAttributes & BIT_PITCH))), -1, 1);
}

static int8_t audio_endpoint_controls(AudioDriver* driver, AudioDev* audio_dev,
    uint8_t request_form, uint8_t bRequest, uint8_t control_selector, 
    Interface* as_interface, void* data, uint8_t len, 
    callback_function callback, uint8_t flags){
    UsbDev* dev = audio_dev->usb_dev;
    Endpoint* e;
    uint8_t endpoint_number;
    __ENDPOINT_REQUEST_PROTO__();
    e = __STRUCT_CALL__(driver, get_iso_endpoint, audio_dev, as_interface);
    UsbDeviceRequest* device_req = dev->get_free_device_request(dev);
    __IF_RET_NEG__(__IS_NULL__(device_req));
    endpoint_number = dev->__endpoint_number(dev, e);
    __IF_ELSE__(request_form == __SET_REQUEST__,
        __request_endpoint_ = __request_endpoint_set,
        __request_endpoint_ = __request_endpoint_get);
    __request_endpoint_(driver, dev, as_interface, device_req, bRequest,
        control_selector, endpoint_number, data, len, callback, flags);
    return __RET_S__;
}

// callback mechanism has to be changed into (UsbDev* dev, Interface* itf, uint32_t status, void* data)
// for requests that have to change interface values this is pretty important
static void frequency_control_callback(UsbDev* dev, Interface* interface, uint32_t status, void* data){
    __IF_RET__(status & E_TRANSFER);
    __TYPE_CAST__(struct SamplingFrequencyControlParameterBlock*, 
        samp_control_block, data);
    ASInterface* as_interface = __STRUCT_CALL__(internal_audio_driver, 
        __get_class_specific_as_interface,
        dev, interface);
    as_interface->current_freq = samp_control_block->tSampleFreq;
}

static void pitch_control_callback(UsbDev* dev, Interface* interface, uint32_t status, void* data){
    __IF_RET__(status & E_TRANSFER);
}

static int8_t sampling_frequency_control(AudioDriver* driver, 
    AudioDev* audio_dev, uint8_t request_form, uint8_t bRequest, 
    Interface* as_interface, struct SampleFrequency sample_freq,
    uint8_t flags){
    MemoryService_C* m = __DEV_MEMORY(audio_dev->usb_dev);
    __IF_RET_NEG__(__IS_NEG_ONE__(support_sampling_freq_control(driver, audio_dev, 
        as_interface)));
    __MAP_IO_KERNEL_S__(m, struct SamplingFrequencyControlParameterBlock,
        sam_freq_block);
    sam_freq_block->tSampleFreq = sample_freq;
    audio_endpoint_controls(driver, audio_dev, request_form,
        bRequest, SAMPLING_FREQ_CONTROL, as_interface, sam_freq_block,
        sizeof(struct SamplingFrequencyControlParameterBlock),
        &frequency_control_callback, CONTROL_INITIAL_STATE);
    
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
    __build_pitch_control_block(driver, pitch_control_block, bPitchEnable);
    audio_endpoint_controls(driver, audio_dev, request_form, bRequest,
        PITCH_CONTROL, as_interface, pitch_control_block, 
        sizeof(struct PitchControlParameterBlock), &pitch_control_callback, 0);

    return __RET_S__;
}

static int8_t set_sampling_frequency(AudioDriver* driver, AudioDev* audio_dev,
    Interface* as_interface, struct SampleFrequency sample_freq){
    __IF_RET_NEG__(__IS_NEG_ONE__(is_valid_frequency(driver, as_interface, audio_dev,
        sample_freq)));
    return sampling_frequency_control(driver, audio_dev, __SET_REQUEST__,
        SET_CUR, as_interface, sample_freq, 0);
}

static int8_t get_sampling_frequency(AudioDriver* driver,
    AudioDev* audio_dev, Interface* as_interface){
    return sampling_frequency_control(driver, audio_dev, __GET_REQUEST__,
        GET_CUR, as_interface, (struct SampleFrequency){}, 
        CONTROL_INITIAL_STATE);
}

static int8_t enable_processing_control(AudioDriver* driver, AudioDev* audio_dev,
    Interface* interface){
    
    return audio_enable_processing_control(driver, audio_dev, interface, __SET_REQUEST__,
        SET_CUR, 1);
}

static int8_t disable_processing_control(AudioDriver* driver, AudioDev* audio_dev,
    Interface* interface){
    
    return audio_enable_processing_control(driver, audio_dev, interface, __SET_REQUEST__,
        SET_CUR, 0);
}

static int8_t audio_mode_select_control(AudioDriver* driver, AudioDev* audio_dev,
    Interface* interface, uint8_t request_form, uint8_t bRequest, uint8_t bMode){
    MemoryService_C* mem_service = __DEV_MEMORY(audio_dev->usb_dev);
    // dolby as standard but can also be the other other process type
    struct ProcessingUnit* processing_unit = __RECOVER_PROCESSING_UNIT__(driver, audio_dev, 
        interface, DOLBY_PROLOGIC_PROCESS);
    __IF_RET_NEG__(__IS_NULL__(processing_unit));
    __IF_RET_NEG__(__IS_NEG_ONE__(__support_mode_select_control(
        driver, processing_unit)));

    __TYPE_CAST__(DolbyPrologicProcessingUnitDescriptor*, dolby_unit, 
        processing_unit->processing_unit_desc.process_specific_descriptor);
    __IF_RET_NEG__(dolby_unit->bNrModes < bMode);

    struct ModeSelectControlParameterBlock* mode_select_block = 
    __MAP_IO_KERNEL__(mem_service, struct ModeSelectControlParameterBlock, 
        sizeof(struct ModeSelectControlParameterBlock));
    __build_mode_select_control_block(driver, mode_select_block, bMode);

    return audio_processing_control(driver, audio_dev, interface, 
        processing_unit->processing_unit_desc.bUnitID, request_form,
        bRequest, UD_MODE_SELECT_CONTROL, mode_select_block, 
        sizeof(struct ModeSelectControlParameterBlock), &processing_callback, 0);
}

static int8_t audio_spaciousness_control(AudioDriver* driver, AudioDev* audio_dev,
    Interface* interface, uint8_t request_form, uint8_t bRequest, uint8_t bSpaciousness){
    MemoryService_C* mem_service = __DEV_MEMORY(audio_dev->usb_dev);
    struct ProcessingUnit* processing_unit = __RECOVER_PROCESSING_UNIT__(driver, 
        audio_dev, interface, STEREO_EXTENDER_PROCESS);
    __IF_RET_NEG__(__IS_NULL__(processing_unit));
    __IF_RET_NEG__(__IS_NEG_ONE__(__support_spaciousness_control(driver, 
        processing_unit)));
    __MAP_IO_KERNEL_S__(mem_service, struct SpaciousnessControlParameterBlock,
        sp_control_block);
    __build_spaciousness_control_block(driver, sp_control_block, bSpaciousness);

    return audio_processing_control(driver, audio_dev, interface, 
        processing_unit->processing_unit_desc.bUnitID, request_form, bRequest,
        STEREO_SPACIOUSNESS_CONTROL, sp_control_block, 
        sizeof(struct SpaciousnessControlParameterBlock), &processing_callback, 0);
}

static int8_t audio_reverb_type_control(AudioDriver* driver, AudioDev* audio_dev,
    Interface* interface, uint8_t request_form, uint8_t bRequest, enum ReverbTypes type){
    MemoryService_C* mem_service = __DEV_MEMORY(audio_dev->usb_dev);
    struct ProcessingUnit* processing_unit = __RECOVER_PROCESSING_UNIT__(driver, 
        audio_dev, interface, REVERBERATION_PROCESS);
    __IF_RET_NEG__(processing_unit);
    __IF_RET_NEG__(__IS_NEG_ONE__(__support_reverb_type_control(driver, 
        processing_unit)));

    __MAP_IO_KERNEL_S__(mem_service, struct ReverbTypeControlParameterBlock,
        rv_control_block);
    __build_reverb_type_control_block(driver, rv_control_block, type);

    // assuming 1 for the control selector -> not sure about it (docs don't specify.)
    return audio_processing_control(driver, audio_dev, interface, 
        processing_unit->processing_unit_desc.bUnitID, request_form, bRequest,
        0, rv_control_block, 
        sizeof(struct ReverbTypeControlParameterBlock), &processing_callback, 0);
}

static int8_t audio_reverb_level_control(AudioDriver* driver, AudioDev* audio_dev,
    Interface* interface, uint8_t request_form, uint8_t bRequest, uint8_t bReverbLevel){
    MemoryService_C* mem_service = __DEV_MEMORY(audio_dev->usb_dev);
    struct ProcessingUnit* processing_unit = __RECOVER_PROCESSING_UNIT__(driver, 
        audio_dev, interface, REVERBERATION_PROCESS);
    __IF_RET_NEG__(__IS_NEG_ONE__(__support_reverb_level_control(driver, 
        processing_unit)));

    __MAP_IO_KERNEL_S__(mem_service, struct ReverbLevelControlParameterBlock,
        rv_control_block);
    __build_reverb_level_control_block(driver, rv_control_block, bReverbLevel);

    return audio_processing_control(driver, audio_dev, interface, 
        processing_unit->processing_unit_desc.bUnitID, request_form, bRequest,
        REVERB_LEVEL_CONTROL, rv_control_block, 
        sizeof(struct ReverbTypeControlParameterBlock), &processing_callback, 0);
}

static int8_t audio_chorus_level_control(AudioDriver* driver, AudioDev* audio_dev,
    Interface* interface, uint8_t request_form, uint8_t bRequest, uint8_t bChorusLevel){
    MemoryService_C* mem_service = __DEV_MEMORY(audio_dev->usb_dev);
    struct ProcessingUnit* processing_unit = __RECOVER_PROCESSING_UNIT__(driver, 
        audio_dev, interface, CHORUS_PROCESS);
    __IF_RET_NEG__(__IS_NEG_ONE__(__support_chorus_level_control(driver, 
        processing_unit)));
    __MAP_IO_KERNEL_S__(mem_service, struct ChorusLevelControlParameterBlock,
        ch_control_block);
    __build_chorus_level_control_block(driver, ch_control_block, bChorusLevel);

    return audio_processing_control(driver, audio_dev, interface, 
        processing_unit->processing_unit_desc.bUnitID, request_form, bRequest,
        CHORUS_LEVEL_CONTROL, ch_control_block, 
        sizeof(struct ReverbTypeControlParameterBlock), &processing_callback, 0);
}

// valid range from 0 to 255 % 
static int8_t audio_set_cur_spaciousness_control(AudioDriver* driver, AudioDev* audio_dev,
    Interface* interface, uint8_t bSpaciousness){
    
    return audio_spaciousness_control(driver, audio_dev, interface, __SET_REQUEST__,
        SET_CUR, bSpaciousness);
}

static enum ProcessingUnitProcessTypes retrieve_process_type(AudioDriver* driver,
    uint8_t* pos){
    uint8_t process_type_offset = 4, descriptor_type = 1, descriptor_subtype = 2;
    __IF_RET_SELF__(*(pos + descriptor_type) != CS_INTERFACE && 
        *(pos + descriptor_subtype) != PROCESSING_UNIT, PROCESS_UNDEFINED);
    return *((uint16_t*)(pos + process_type_offset));
}

static int8_t set_audio_mode_attribute(AudioDriver* driver, AudioDev* audio_dev,
    Interface* interface, uint8_t bMode){
    return audio_mode_select_control(driver, audio_dev, interface, __SET_REQUEST__,
        SET_CUR, bMode);
}

static int8_t enable_extension_unit(AudioDriver* driver, AudioDev* audio_dev,
    Interface* interface){
    return audio_extension_control(driver, audio_dev, interface, __SET_REQUEST__,
        SET_CUR, 1);
}

static int8_t disable_extension_unit(AudioDriver* driver, AudioDev* audio_dev,
    Interface* interface){
    return audio_extension_control(driver, audio_dev, interface, __SET_REQUEST__,
        SET_CUR, 0);
}

static void copy_protect_control_callback(UsbDev* dev, Interface* interface, uint32_t status, void* data){
    __IF_RET__(status & E_TRANSFER);
    /*__TYPE_CAST__(CopyProtectControlParameterBlock*, copy_protect_block, data);    
    MemoryService_C* mem_service = __DEV_MEMORY(dev); */
}

// input terminal should only support get terminal copy protect control req
// output terminal should only support set terminal copy protect control req
static int8_t audio_copy_protect_control(AudioDriver* driver, AudioDev* audio_dev,
    Interface* interface, uint8_t request_form, uint8_t bRequest, CPL cpl){
    enum ACInterfaceDescriptorSubtypes subtype;
    ASInterface* as_interface = __STRUCT_CALL__(driver, 
        __get_class_specific_as_interface, audio_dev->usb_dev, interface);
    ACInterface* ac_interface = __STRUCT_CALL__(driver, __convert_to_class_specific_ac_interface, 
        audio_dev->audio_control_interface);
    uint8_t terminal_type = as_interface->terminal_type;
    UsbDev* dev = audio_dev->usb_dev;
    __COMMON_REQUEST_PROTO__(terminal);
    UsbDeviceRequest* device_req = dev->get_free_device_request(dev);
    __IF_RET_NEG__(__IS_NULL__(device_req));
    __IF_RET_NEG__((terminal_type == INPUT_TERMINAL) && request_form == __GET_REQUEST__);
    __IF_RET_NEG__((terminal_type == OUTPUT_TERMINAL) && request_form == __SET_REQUEST__);
    MemoryService_C* mem_service = __DEV_MEMORY(audio_dev->usb_dev);
    CopyProtectControlParameterBlock* copy_protect_block = 
        __MAP_IO_KERNEL__(mem_service, CopyProtectControlParameterBlock, 
        sizeof(CopyProtectControlParameterBlock));
    __build_copy_protect_control(driver, copy_protect_block, cpl);
    uint8_t terminal_id;
    void* terminal = __STRUCT_CALL__(driver, find_structure_by_id, 
        ac_interface, as_interface->as_desc.bTerminalLink, &subtype);
    __IF_RET_NEG__((subtype != OUTPUT_TERMINAL) && (subtype != INPUT_TERMINAL));
    if(subtype == OUTPUT_TERMINAL) terminal_id = ((struct OutputTerminal*)terminal)->out_terminal_desc.bTerminalID;
    else terminal_id = ((struct InputTerminal*)terminal)->in_terminal_desc.bTerminalID;
    uint8_t audio_interface = dev->__interface_number(dev, 
        audio_dev->audio_control_interface->active_interface);
    
    __IF_ELSE__(request_form == __SET_REQUEST__,
        __request_common_terminal = &__request_common_set,
        __request_common_terminal = &__request_common_get);

    __request_common_terminal(driver, dev, interface, device_req, bRequest, COPY_PROTECT_CONTROL,
        audio_interface, terminal_id, copy_protect_block, sizeof(CopyProtectControlParameterBlock),
        &copy_protect_control_callback, 0);

    return __RET_S__;
}

static int8_t set_copy_protect(AudioDriver* driver, AudioDev* audio_dev,
    Interface* interface, CPL cpl){
    
    return audio_copy_protect_control(driver, audio_dev, interface, __SET_REQUEST__,
        SET_CUR, cpl);
}

static int8_t get_copy_protect(AudioDriver* driver, AudioDev* audio_dev,
    Interface* interface, CPL cpl){
    
    return audio_copy_protect_control(driver, audio_dev, interface, __GET_REQUEST__,
        GET_CUR, cpl);
}

static int8_t set_sound_value(AudioDriver* driver, int16_t value, AudioDev* audio_dev,
    Interface* as_interface){
    __IF_RET_NEG__(value > audio_dev->max_volume || value < audio_dev->min_volume);
    as_interface->data = audio_dev;
    return audio_volume_control(driver, audio_dev, as_interface, __SET_REQUEST__,
        SET_CUR, 0, &volume_change_callback, 0, value);
}

static int8_t set_max_sound_value(AudioDriver* driver, int16_t value, 
    AudioDev* audio_dev, Interface* as_interface){
    __IF_RET_NEG__(value < audio_dev->min_volume);
    as_interface->data = audio_dev;
    return audio_volume_control(driver, audio_dev, as_interface, __SET_REQUEST__,
        SET_MAX, 0, &volume_max_callback, 0, value);
}

static int8_t set_min_sound_value(AudioDriver* driver, int16_t value,
    AudioDev* audio_dev, Interface* as_interface){
    __IF_RET_NEG__(value > audio_dev->max_volume);
    as_interface->data = audio_dev;
    return audio_volume_control(driver, audio_dev, as_interface, __SET_REQUEST__,
        SET_MIN, 0, &volume_min_callback, 0, value);
}

static int8_t get_max_volume(AudioDriver* driver, AudioDev* audio_dev, 
    Interface* as_interface){
    return audio_volume_control(driver, audio_dev, as_interface, __GET_REQUEST__,
        GET_MAX, 0, &volume_max_callback, CONTROL_INITIAL_STATE, 0);
}

static int8_t get_min_volume(AudioDriver* driver,
    AudioDev* audio_dev, Interface* as_interface){
    as_interface->data = audio_dev;
    return audio_volume_control(driver, audio_dev, as_interface, __GET_REQUEST__,
        GET_MIN, 0, &volume_min_callback, CONTROL_INITIAL_STATE, 0);
}

static int8_t get_current_volume(AudioDriver* driver,
    AudioDev* audio_dev, Interface* as_interface){
    as_interface->data = audio_dev;
    return audio_volume_control(driver, audio_dev, as_interface, __GET_REQUEST__,
        GET_CUR, 0, &volume_change_callback, CONTROL_INITIAL_STATE, 0);
}

static int8_t set_volume_resolution(AudioDriver* driver, AudioDev* audio_dev,
    int16_t resolution_value, Interface* as_interface){
    __IF_RET_NEG__(resolution_value <= 0);
    as_interface->data = audio_dev;
    return audio_volume_control(driver, audio_dev, as_interface, __SET_REQUEST__,
        SET_RES, 0xFF, 0, resolution_value, 0);
} 

static int8_t mute(AudioDriver* driver, AudioDev* audio_dev, Interface* interface){
    interface->data = audio_dev;
    return audio_mute_control(driver, audio_dev, interface, __SET_REQUEST__,
        SET_CUR, 0, &mute_control_callback, 0, 1);
}

static int8_t unmute(AudioDriver* driver, AudioDev* audio_dev, Interface* interface){
    interface->data = audio_dev;
    return audio_mute_control(driver, audio_dev, interface, __SET_REQUEST__, 
        SET_CUR, 0, &mute_control_callback, 0, 0);
}

static int8_t get_mute_attribute(AudioDriver* driver, AudioDev* audio_dev, Interface* interface){
    interface->data = audio_dev;
    return audio_mute_control(driver, audio_dev, interface, __GET_REQUEST__,
        GET_CUR, 0, &mute_control_callback, CONTROL_INITIAL_STATE, 0);
}