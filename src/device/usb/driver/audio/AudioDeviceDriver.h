#ifndef AUDIO_DEVICE_DRIVER_INCLUDE
#define AUDIO_DEVICE_DRIVER_INCLUDE

#include "../UsbDriver.h"
#include "../../interfaces/MapInterface.h"
#include "../../interfaces/MutexInterface.h"

#define AUDIO_CONFIGURATION_BUFFER 2048

#define __INIT_AUDIO_DRIVER__(name, driver_name, entry) \
  __ENTRY__(name, get_free_audio_dev) = &get_free_audio_dev; \
  __ENTRY__(name, match_audio_dev) = &match_audio_dev; \
  __ENTRY__(name, free_audio_dev) = &free_audio_dev; \
  __ENTRY__(name, configure_audio_device) = &configure_audio_device; \
  __ENTRY__(name, sync_streaming_interface) = &sync_streaming_interface; \
  __ENTRY__(name, audio_device_routine) = &audio_device_routine; \
  __ENTRY__(name, switch_if_zero_bandwidth) = &switch_if_zero_bandwidth; \
  __ENTRY__(name, set_default_frequency) = &set_default_frequency; \
  __ENTRY__(name, __get_terminal) = &__get_terminal; \
  __ENTRY__(name, __get_sample_frequency) = &__get_sample_frequency; \
  __ENTRY__(name, __get_sub_frame_size) = &__get_sub_frame_size; \
  __ENTRY__(name, __get_total_supported_frequencies) = &__get_total_supported_frequencies; \
  __ENTRY__(name, __get_1ms_size) = &__get_1ms_size; \
  __ENTRY__(name, __get_bit_depth) = &__get_bit_depth; \
  __ENTRY__(name, __get_frame_size) = &__get_frame_size; \
  __ENTRY__(name, __is_freq_set) = &__is_freq_set; \
  __ENTRY__(name, __convert_to_class_specific_as_interface) = &__convert_to_class_specific_as_interface; \
  __ENTRY__(name, __convert_to_class_specific_ac_interface) = &__convert_to_class_specific_ac_interface; \
  __ENTRY__(name, __get_audio_dev) = &__get_audio_dev; \
  __ENTRY__(name, __match_terminal_type) = &__match_terminal_type; \
  __ENTRY__(name, __is_as_output_terminal) = &__is_as_output_terminal; \
  __ENTRY__(name, __is_as_input_terminal) = &__is_as_input_terminal; \
  __ENTRY__(name, __get_as_interface_by_terminal) = &__get_as_interface_by_terminal; \
  __ENTRY__(name, __get_sync_delay) = &__get_sync_delay; \
  __ENTRY__(name, __get_sync_unit)  = &__get_sync_unit; \
  __ENTRY__(name, __convert_to_class_specific_as_endpoint) = &__convert_to_class_specific_as_endpoint; \
  \
  __SUPER__(name, probe) = &probe_audio; \
  __SUPER__(name, disconnect) = &disconnect_audio; \
  __SUPER__(name, new_usb_driver) = &new_usb_driver; \
  \
  __CALL_SUPER__(name->super, new_usb_driver, driver_name, entry)

#define __SET_REQUEST__ 0x01
#define __GET_REQUEST__ 0x02

#define __UPPER_VOL_BOUNDARY__ 0x7FFF
#define __LOWER_VOL_BOUNDARY__ 0x8001
#define __SILENCE_VOL__ 0x8000

#define __CHANNEL_OR_INC__(driver, cluster, loc, spatial_loc, channel_number) \
  __IF_RET_SELF__(loc == SPATIAL_LOC_L, channel_number); \
  __IF_CUSTOM__(__is_channel_in_cluster(driver, cluster, SPATIAL_LOC_L), \
        channel_number++)

#define __LOCATION_OR_INC__(driver, cluster, loc , channel_number, channel_count) \
  __IF_CUSTOM__(__is_channel_in_cluster(driver ,cluster, loc), \
    __IF_ELSE__((channel_number == channel_count), return loc, channel_count++));

#define __PARSE_BREAK_ROUTINE__(function_routine) \
  function_routine; break;

#define __SIMPLE_SPECIFIC_PROCESSING_UNIT_ROUTINE__(driver, start, processing_unit_descriptor, type, mem_service) \
  __ALLOC_KERNEL_MEM_S__(m, type, processing_unit_desc); \
  processing_unit_desc->bLength = *start; \
  processing_unit_desc->bDescriptorType = *(start+1); \
  processing_unit_desc->bDescriptorSubtype = *(start+2); \
  processing_unit_desc->bUnitID = *(start+3); \
  processing_unit_desc->wProcessType = *(start+4); \
  processing_unit_desc->bNrInPins = *(start+6); \
  processing_unit_desc->bSourceID = *(start+7); \
  __parse_cluster(driver, &processing_unit_desc->cluster, start, 8); \
  processing_unit_desc->bControlSize = *(start+12); \
  __parse_bmControls(driver, start+13, 0, \
        &processing_unit_desc->bControlSize, mem_service); \
   processing_unit_desc->iProcessing = \
    *(start+13+processing_unit_desc->bControlSize); \
  __assign_process(driver, processing_unit_desc, processing_unit_descriptor);

#define __EXT_SPECIFIC_PROCESSING_UNIT_ROUTINE__(driver, start, processing_unit_descriptor, \
  type, mem_service) \
  __SIMPLE_SPECIFIC_PROCESSING_UNIT_ROUTINE__(driver, start, processing_unit_descriptor, type, mem_service); \
  processing_unit_desc->bNrModes = *(start+14+processing_unit_desc->bControlSize); \
  __parse_waModes(driver, start+15+processing_unit_desc->bControlSize, \
        &processing_unit_desc->bNrModes, &processing_unit_desc); \
  
#define __SPACIOUSNESS_UPPER_BOUND__ 255
#define __SPACIOUSNESS_LOWER_BOUND__ 0

#define __MATCH_ID__(driver, unit_descriptor, id, target_bSourceID, cmd) \
  __IF_CUSTOM__(id == target_bSourceID, \
    cmd \
  )

#define __UNIT_MATCH_ID__(driver, unit_descriptor, target_bSourceID, cmd) \
  __MATCH_ID__(driver, unit_descriptor, unit_descriptor.bUnitID, target_bSourceID, cmd)

#define __TERMINAL_MATCH_ID__(driver, terminal_descriptor, target_bSourceID, cmd) \
  __MATCH_ID__(driver, terminal_descriptor, terminal_descriptor.bTerminalID, target_bSourceID, cmd)

#define __INPUT_TERMINAL_MATCH__(driver, ac_interface, input_terminal_id, index, cmd) \
  __TERMINAL_MATCH_ID__(driver, ac_interface->in_terminal[index]->in_terminal_desc, input_terminal_id, cmd)

#define __OUTPUT_TERMINAL_MATCH__(driver, ac_interface, output_terminal_id, index, cmd) \
  __TERMINAL_MATCH_ID__(driver, ac_interface->out_terminal[index]->out_terminal_desc, output_terminal_id, cmd)

#define __FEATURE_UNIT_MATCH__(driver, ac_interface, feature_unit_id, index, cmd) \
  __UNIT_MATCH_ID__(driver, ac_interface->feature_unit[index]->feature_unit_desc, feature_unit_id, cmd)

#define __MIXER_UNIT_MATCH__(driver, ac_interface, mixer_unit_id, index, cmd) \
  __UNIT_MATCH_ID__(driver, ac_interface->mixer_unit[index]->mixer_unit_desc, mixer_unit_id, cmd)

#define __SELECTOR_UNIT_MATCH__(driver, ac_interface, selector_unit_id, index, cmd) \
  __UNIT_MATCH_ID__(driver, ac_interface->selector_unit[index]->selector_unit_desc, selector_unit_id, cmd)

#define __EXTENSION_UNIT_MATCH__(driver, ac_interface, extension_unit_id, index, cmd) \
  __UNIT_MATCH_ID__(driver, ac_interface->extension_unit[index]->extension_unit_desc, extension_unit_id, cmd)

#define __PROCESSING_UNIT_MATCH__(driver, ac_interface, processing_unit_id, index, cmd) \
  __UNIT_MATCH_ID__(driver, ac_interface->processing_unit[index]->processing_unit_desc, processing_unit_id, cmd)

#define __TOTAL_CHANNEL_MATCH_TERMINAL__(driver, terminal_descriptor, target_bSourceID) \
  __TERMINAL_MATCH_ID__(driver, terminal_descriptor, target_bSourceID, \
    return __get_total_channels_in_cluster(driver, terminal_descriptor.cluster))

#define __TOTAL_CHANNEL_MATCH_UNIT__(driver, unit_descriptor, target_bSourceID) \
  __UNIT_MATCH_ID__(driver, unit_descriptor, target_bSourceID, \
    return __get_total_channels_in_cluster(driver, unit_descriptor.cluster))

#define __MULTI_UPSTREAM__(driver, ac_interface, unit, sub_type, bSource_off, type) \
  return find_structure_by_id(driver, ac_interface, ((type*)unit)->baSourceID + bSource_off, sub_type); break;

#define __SINGLE_UPSTREAM__(driver, ac_interface, unit, sub_type, type) \
  return find_structure_by_id(driver, ac_interface, ((type*)unit)->bSourceID, sub_type); break;
  
#define __UNIT_COUNT__(ac_interface, unit) \
  ac_interface->unit_count_map[unit] == __MAX_UNITS__

#define __GET_UNIT_COUNT__(ac_interface, unit) \
  ac_interface->unit_count_map[unit]

#define __UNIT_INC__(ac_interface, unit) \
  ac_interface->unit_count_map[unit] = __GET_UNIT_COUNT__(ac_interface, unit) + 1

#define __ADD_UNIT__(ac_interface, unit, unit_type, unit_v) \
  ac_interface->unit[__GET_UNIT_COUNT__(ac_interface, unit_type)] = unit_v; \
  __UNIT_INC__(ac_interface, unit_type)

#define __MAX_UPSTREAM_DEPTH__ 3
#define __MAX_UNITS__ 10
#define __MAX_STREAMING_INTERFACES__ 10

#define __TRAVERSE_UNIT__ \
  __FOR_RANGE__(i, int, 0, __MAX_UNITS__)

struct AudioStreamingDescriptor {
  uint8_t bLength;
  uint8_t bDescriptorType;
  uint8_t bDescriptorSubtype;
  uint8_t bTerminalLink;
  uint8_t bDelay;
  uint16_t wFormatTag;
} __attribute__((packed));

struct AudioControlDescriptor {
  uint8_t bLength;
  uint8_t bDescriptorType;
  uint8_t bDescriptorSubtype;
  uint16_t bcdADC;
  uint16_t wTotalLength;
  uint8_t bInCollection;
  uint8_t* baInterfaceNr;
} __attribute__((packed));

struct AudioStreamingIsoEndpointDescriptor {
  uint8_t bLength;
  uint8_t bDescriptorType;
  uint8_t bDescriptorSubtype;
  uint8_t bmAttributes;
  uint8_t bLockDelayUnits;
  uint16_t wLockDelay;
} __attribute__((packed));

struct SampleFrequency{
  uint16_t sampleFreq_lo;
  uint8_t  sampleFreq_hi;
} __attribute__((packed));

typedef struct SampleFrequency SampleFrequency;

#define __8000_HZ__  (SampleFrequency){.sampleFreq_lo = 8000,  .sampleFreq_hi = 0}
#define __16000_HZ__ (SampleFrequency){.sampleFreq_lo = 16000, .sampleFreq_hi = 0}
#define __24000_HZ__ (SampleFrequency){.sampleFreq_lo = 24000, .sampleFreq_hi = 0}
#define __32000_HZ__ (SampleFrequency){.sampleFreq_lo = 32000, .sampleFreq_hi = 0}
#define __44100_HZ__ (SampleFrequency){.sampleFreq_lo = 44100, .sampleFreq_hi = 0} 
#define __48000_HZ__ (SampleFrequency){.sampleFreq_lo = 48000, .sampleFreq_hi = 0}

struct ACInterface {
  struct AudioControlDescriptor ac_desc;
  struct InputTerminal* in_terminal[__MAX_UNITS__];
  struct OutputTerminal* out_terminal[__MAX_UNITS__];
  struct MixerUnit* mixer_unit[__MAX_UNITS__];
  struct SelectorUnit* selector_unit[__MAX_UNITS__];
  struct FeatureUnit* feature_unit[__MAX_UNITS__];
  struct ProcessingUnit* processing_unit[__MAX_UNITS__];
  struct ExtensionUnit* extension_unit[__MAX_UNITS__];
  struct AssociatedInterface* associated_interface[__MAX_UNITS__];
  uint8_t unit_count_map[__MAX_UNITS__];
};

struct ASEndpoint {
  struct AudioStreamingIsoEndpointDescriptor as_iso_endpoint;
};

struct ASInterface {
  struct AudioStreamingDescriptor as_desc;
  struct FormatType* format_type;
  struct SampleFrequency current_freq;
  uint8_t bPitchEnable;
  uint8_t terminal_type;

  uint8_t* buffer_first;
  uint8_t* buffer_second;
  uint8_t active_buffer;
  unsigned int buffer_size;
  uint32_t qh_id;
};

/*struct AudioInterfaceCollection {
  struct ACInterface* ac_itf;
  struct ASInterface** as_itf;
}; */

enum LockDelayUnit{
  LOCK_UNIT_UNDEFINED = 0x00,
  LOCK_UNIT_MS        = 0x01,
  LOCK_PCM_SAMPLES    = 0x02
};

typedef struct ACInterface ACInterface;
typedef struct ASInterface ASInterface;
typedef struct AudioInterfaceCollection AudioInterfaceCollection;
typedef struct ASEndpoint ASEndpoint;

struct AudioDev {
    UsbDev* usb_dev;
    unsigned int endpoint_addr;
    void* buffer_first;
    void* buffer_second;
    unsigned int buffer_size;
    uint8_t priority;
    Interface* audio_control_interface;
    Interface* audio_streaming_interfaces[__MAX_STREAMING_INTERFACES__];
    uint8_t audio_streaming_interfaces_num;
    uint16_t interval;
    UsbDriver* usb_driver;
    int16_t curr_volume;
    int16_t max_volume;
    int16_t min_volume;
    //Mutex_C* mutex;

    void (*callback)(UsbDev* dev, Interface* interface, uint32_t status, void* data);
};

struct AudioDriver {
  struct UsbDriver super;
  struct AudioDev dev[MAX_DEVICES_PER_USB_DRIVER];
  uint8_t audio_map[MAX_DEVICES_PER_USB_DRIVER];

  void (*new_audio_driver)(struct AudioDriver* driver, char* name, UsbDevice_ID* entry);
  struct AudioDev* (*get_free_audio_dev)(struct AudioDriver* driver);
  void (*free_audio_dev)(struct AudioDriver *driver, struct AudioDev *audio_dev);
  struct AudioDev* (*match_audio_dev)(struct AudioDriver *driver, UsbDev *dev);
  int (*configure_audio_device)(struct AudioDriver* driver);
  int8_t (*sync_streaming_interface)(struct AudioDriver* driver, 
    UsbDev* dev, Interface* interface);
  void (*audio_device_routine)(struct AudioDriver* driver, uint8_t* start,
    uint8_t* end, MemoryService_C* mem_service, uint8_t minor);
  void (*switch_if_zero_bandwidth)(struct AudioDriver* driver, struct AudioDev* audio_dev);
  void (*set_default_frequency)(struct AudioDriver* driver, struct AudioDev* audio_dev);
  uint8_t (*__get_terminal)(struct AudioDriver* driver, struct AudioDev* audio_dev, ASInterface* as_interface);
  uint32_t (*__get_sample_frequency)(struct AudioDriver* driver, ASInterface* as_interface);
  uint32_t (*__get_sub_frame_size)(struct AudioDriver* driver, ASInterface* as_interface);
  uint32_t (*__get_total_supported_frequencies)(struct AudioDriver* driver, ASInterface* as_interface);
  uint16_t (*__get_1ms_size)(struct AudioDriver* driver, ASInterface* as_interface);
  uint32_t (*__get_bit_depth)(struct AudioDriver* driver, ASInterface* as_interface);
  uint32_t (*__get_frame_size)(struct AudioDriver* driver, ASInterface* as_interface);
  int8_t (*__is_freq_set)(struct AudioDriver* driver, ASInterface* as_interface);
  ASInterface* (*__convert_to_class_specific_as_interface)(struct AudioDriver* driver,
    Interface* interface);
  ACInterface* (*__convert_to_class_specific_ac_interface)(struct AudioDriver* driver,
    Interface* interface);
  ASEndpoint* (*__convert_to_class_specific_as_endpoint)(struct AudioDriver* driver,
    Endpoint* endpoint);
  struct AudioDev* (*__get_audio_dev)(struct AudioDriver* driver, uint8_t minor);
  int8_t (*__match_terminal_type)(struct AudioDriver* driver, 
    ASInterface* as_interface, uint8_t terminal_type);
  int8_t (*__is_as_output_terminal)(struct AudioDriver* driver,
    ASInterface* as_interface);
  int8_t (*__is_as_input_terminal)(struct AudioDriver* driver,
    ASInterface* as_interface);
  ASInterface* (*__get_as_interface_by_terminal)(struct AudioDriver* driver, 
    struct AudioDev* audio_dev, uint8_t terminal_type);
  uint16_t (*__get_sync_delay)(struct AudioDriver* driver, Endpoint* endpoint);
  enum LockDelayUnit (*__get_sync_unit)(struct AudioDriver* driver, Endpoint* endpoint);

};

void new_audio_driver(struct AudioDriver* driver, char* name, UsbDevice_ID* entry);

struct ASAudioEndpoint{ // since the default endpoint is an extended enpoint this structure is required and has to be stored in the corresponding audio component 
  Endpoint endpoint;
  uint8_t bInterval;
  uint8_t bRefresh;
  uint8_t bSynchAddress;
} __attribute__((packed));



enum AudioDescriptorTypes {
  CS_DEVICE = 0x21,
  CS_CONFIGURATION = 0x22,
  CS_STRING = 0x23,
  CS_INTERFACE = 0x24,
  CS_ENDPOINT = 0x25
};

enum ACInterfaceDescriptorSubtypes {
  AC_DESCRIPTOR_UNDEFINED = 0x00,
  HEADER = 0x01,
  INPUT_TERMINAL = 0x02,
  OUTPUT_TERMINAL = 0x03,
  MIXER_UNIT = 0x04,
  SELECTOR_UNIT = 0x05,
  FEATURE_UNIT = 0x06,
  PROCESSING_UNIT = 0x07,
  EXTENSION_UNIT = 0x08
};

enum ASInterfaceDescriptorSubtypes {
  AS_DESCRIPTOR_UNDEFINED = 0x00,
  AS_GENERAL = 0x01,
  FORMAT_TYPE = 0x02,
  FORMAT_SPECIFIC = 0x03
};

enum AudioEndpointDescriptorSubtypes {
  DESCRIPTOR_UNDEFINED = 0x00,
  EP_GENERAL = 0x01
};

enum TerminalTypes{
  USB_UNDEFINED = 0x0100,
  USB_STREAMING = 0x0101,
  USB_VENDOR_SPECIFIC = 0x01FF
};

enum InputTerminalTypes{
  INPUT_UNDEFINED = 0x0200,
  MICROPHONE = 0x0201,
  DESKTOP_MICROPHONE = 0x0202,
  PERSONAL_MICROPHONE = 0x0203,
  OMNI_DIRECTIONAL_MICROPHONE = 0x0204
};

enum OutputTerminalTypes{
  OUTPUT_UNDEFINED = 0x0300,
  SPEAKER = 0x0301,
  HEADPHONES = 0x0302,
  HEAD_MOUNTED_DISPLAY_AUDIO = 0x0303,
  DESKTOP_SPEAKER = 0x0304,
  ROOM_SPEAKER = 0x0305,
  COMMUNICATION_SPEAKER = 0x0306,
  LOW_FREQ_EFFECTS_SPEAKER = 0x0307
};

enum BiDirectionalTerminalTypes{
  BI_DIRECTIONAL_UNDEFINED = 0x0400,
  HANDSET = 0x0401,
  HEADSET = 0x0402,
  SPEAKER_PHONE = 0x0403,
  ECHO_SUPRESSING_SPEAKER_PHONE = 0x0404,
  ECHO_CANCELING_SPEAKER_PHONE = 0x0405
};

enum ProcessingUnitProcessTypes {
    PROCESS_UNDEFINED = 0x00,
    UP_DOWNMIX_PROCESS = 0x01,
    DOLBY_PROLOGIC_PROCESS = 0x02,
    STEREO_EXTENDER_PROCESS = 0x03,
    REVERBERATION_PROCESS = 0x04,
    CHORUS_PROCESS = 0x05,
    DYN_RANGE_COMP_PROCESS = 0x06
};

enum TerminalControlSelectors{
    TE_CONTROL_UNDEFINED = 0x00,
    COPY_PROTECT_CONTROL = 0x01
};

enum FeatureUnitbmaControls{
  BIT_MUTE = 0x0001,
  BIT_VOL  = 0x0002,
  BIT_BASS = 0x0004,
  BIT_MID  = 0x0008,
  BIT_TREBLE = 0x0010,
  BIT_GRAPHIC_EQ = 0x0020,
  BIT_AUTO_GAIN = 0x0040,
  BIT_DELAY = 0x0080,
  BIT_BASS_BOOST = 0x0100,
  BIT_LOUDNESS = 0x0200
};

enum IsoEndpointControls {
  BIT_SAMPLING_FREQ = 0x01,
  BIT_PITCH = 0x02,
  BIT_MAX_PACKETS_ONLY = 0x80
};

enum FeatureUnitControlSelectors{
    FU_CONTROL_UNDEFINED = 0x00,
    MUTE_CONTROL = 0x01,
    VOLUME_CONTROL = 0x02,
    BASS_CONTROL = 0x03,
    MID_CONTROL = 0x04,
    TREBLE_CONTROL = 0x05,
    GRAPHIC_EQUALIZER_CONTROL = 0x06,
    AUTOMATIC_GAIN_CONTROL = 0x07,
    DELAY_CONTROL = 0x08,
    BASS_BOOST_CONTROL = 0x09,
    LOUDNESS_CONTROL = 0x0A
};

enum UP_DOWN_MixProcessingUnitControlSelectors{
    UD_CONTROL_UNDEFINED = 0x00,
    UD_ENABLE_CONTROL = 0x01,
    UD_MODE_SELECT_CONTROL = 0x02
};

enum DolbyPrologicProcessingUnitControlSelectors{
    DP_CONTROL_UNDEFINED = 0x00,
    DP_ENABLE_CONTROL = 0x01,
    DP_MODE_SELECT_CONTROL = 0x02
};

enum StereoExtenderProcessingUnitControlSelectors{
    STEREO_CONTROL_UNDEFINED = 0x00,
    STEREO_ENABLE_CONTROL = 0x01,
    STEREO_SPACIOUSNESS_CONTROL = 0x03
};

enum ReverberationProcessingUnitControlSelectors{
    RV_CONTROL_UNDEFINED = 0x00,
    RV_ENABLE_CONTROL = 0x01,
    REVERB_LEVEL_CONTROL = 0x02,
    REVERB_TIME_CONTROL = 0x03,
    REVERB_FEEDBACK_CONTROL = 0x04
};

enum ChorusProcessingUnitControlSelectors{
    CH_CONTROL_UNDEFINED = 0x00,
    CH_ENABLE_CONTROL = 0x01,
    CHORUS_LEVEL_CONTROL = 0x02,
    CHORUS_RATE_CONTROL = 0x03,
    CHORUS_DEPTH_CONTROL = 0x04
};

enum DynamicRangeCompressorProcessingUnitControlSelectors{
    DR_CONTROL_UNDEFINED = 0x00,
    DR_ENABLE_CONTROL = 0x01,
    COMPRESSION_RATE_CONTROL = 0x02,
    MAXAMPL_CONTROL = 0x03,
    THRESHOLD_CONTROL = 0x04,
    ATTACK_TIME = 0x05,
    RELEASE_TIME = 0x06
};

enum ExtensionUnitControlSelectors{
    XU_CONTROL_UNDEFINED = 0x00,
    XU_ENABLE_CONTROL = 0x01
};

enum EndpoinControlSelectors{
    EP_CONTROL_UNDEFINED = 0x00,
    SAMPLING_FREQ_CONTROL = 0x01,
    PITCH_CONTROL = 0x02
};

enum ReverbTypes{
  ROOM_ONE = 0x00,
  ROOM_TWO = 0x01,
  ROOM_THREE = 0x02,
  HALL_ONE = 0x03,
  HALL_TWO = 0x04,
  PLATE = 0x05,
  DELAY = 0x06,
  PANNING_DELAY = 0x07
};

enum AudioRequestCodes{
    REQUEST_CODE_UNDEFINED = 0x00,
    SET_CUR = 0x01,
    GET_CUR = 0x81,
    SET_MIN = 0x02,
    GET_MIN = 0x82,
    SET_MAX = 0x03,
    GET_MAX = 0x83,
    SET_RES = 0x04,
    GET_RES = 0x84,
    SET_MEM = 0x05,
    GET_MEM = 0x85,
    GET_STAT = 0xFF
};

enum SpatialLocations{
  SPATIAL_LOC_L = 0x01,
  SPATIAL_LOC_R = 0x02,
  SPATIAL_LOC_C = 0x04,
  SPATIAL_LOC_LFE = 0x08,
  SPATIAL_LOC_LS  = 0x10,
  SPATIAL_LOC_RS  = 0x20,
  SPATIAL_LOC_LC  = 0x40,
  SPATIAL_LOC_RC  = 0x80,
  SPATIAL_LOC_S = 0x100,
  SPATIAL_LOC_SL = 0x200,
  SPATIAL_LOC_SR = 0x400,
  SPATIAL_LOC_T  = 0x800
};

struct ClusterDescriptor{
  uint8_t bNrChannels;
  uint16_t wChannelConfig;
  uint8_t iChannelNames;
} __attribute__((packed));

typedef struct ClusterDescriptor ClusterDescriptor;

struct InputTerminalDescriptor{
  uint8_t bLength;
  uint8_t bDescriptorType;
  uint8_t bDescriptorSubtype;
  uint8_t bTerminalID;
  uint16_t wTerminalType;
  uint8_t bAssocTerminal;
  ClusterDescriptor cluster;
  uint8_t iTerminal;
} __attribute__((packed));

struct InputTerminal{
  struct InputTerminalDescriptor in_terminal_desc;
  char* in_terminal_description;
};

struct OutputTerminalDescriptor{
  uint8_t bLength;
  uint8_t bDescriptorType;
  uint8_t bDescriptorSubtype;
  uint8_t bTerminalID;
  uint16_t wTerminalType;
  uint8_t bAssocTerminal;
  uint8_t bSourceID;
  uint8_t iTerminal;
} __attribute__((packed));

struct OutputTerminal{
  struct OutputTerminalDescriptor out_terminal_desc;
  char* out_terminal_description;
};

struct MixerUnitDescriptor{
  uint8_t bLength;
  uint8_t bDescriptorType;
  uint8_t bDescriptorSubtype;
  uint8_t bUnitID;
  uint8_t bNrInPins;
  uint8_t* baSourceID;
  ClusterDescriptor cluster;
  uint8_t* bmControls;
  uint8_t iMixer;
};

struct MixerUnit{
  struct MixerUnitDescriptor mixer_unit_desc;
  char* mixer_description;
  uint8_t bmControlsLength;
};

struct SelectorUnitDescriptor{
  uint8_t bLength;
  uint8_t bDescriptorType;
  uint8_t bDescriptorSubtype;
  uint8_t bUnitID;
  uint8_t bNrInPins;
  uint8_t* baSourceID;
  uint8_t iSelector;
};

struct SelectorUnit{
  struct SelectorUnitDescriptor selector_unit_desc;
  char* selector_description;
};

struct FeatureUnitDescriptor{
  uint8_t bLength;
  uint8_t bDescriptorType;
  uint8_t bDescriptorSubtype;
  uint8_t bUnitID;
  uint8_t bSourceID;
  uint8_t bControlSize;
  uint8_t** bmaControls; // get channels from upstream unit 
  uint8_t iFeature;
};

struct FeatureUnit{
  struct FeatureUnitDescriptor feature_unit_desc;
  char* feature_description;
};

struct ProcessingUnitDescriptor{
  uint8_t bLength;
  uint8_t bDescriptorType;
  uint8_t bDescriptorSubtype;
  uint8_t bUnitID;
  uint16_t wProcessType;
  uint8_t bNrInPins;
  uint8_t* baSourceID;
  ClusterDescriptor cluster;
  uint8_t bControlSize;
  uint8_t* bmControls;
  uint8_t iProcessing;
  void* process_specific_descriptor;
};

struct ProcessingUnit{
   struct ProcessingUnitDescriptor processing_unit_desc;
   char* processing_description;
};

struct UP_DOWN_MixProcessingUnitDescriptor{
  uint8_t bLength;
  uint8_t bDescriptorType;
  uint8_t bDescriptorSubtype;
  uint8_t bUnitID;
  uint16_t wProcessType;
  uint8_t bNrInPins;
  uint8_t bSourceID;
  ClusterDescriptor cluster;
  uint8_t bControlSize;
  uint8_t* bmControls;
  uint8_t iProcessing;
  uint8_t bNrModes;
  uint16_t* waModes;
};

// waModes :
// left,right,center = 0x0007
// left,right,surround = 0x0103
// left,right,center,surround = 0x0107
struct DolbyPrologicProcessingUnitDescriptor{
  uint8_t bLength;
  uint8_t bDescriptorType;
  uint8_t bDescriptorSubtype;
  uint8_t bUnitID;
  uint16_t wProcessType;
  uint8_t bNrInPins;
  uint8_t bSourceID;
  ClusterDescriptor cluster;
  uint8_t bControlSize;
  uint8_t* bmControls;
  uint8_t iProcessing;
  uint8_t bNrModes;
  uint16_t* waModes;
};

struct THREE_D_StereoExtenderProcessingUnitDescriptor{
  uint8_t bLength;
  uint8_t bDescriptorType;
  uint8_t bDescriptorSubtype;
  uint8_t bUnitID;
  uint16_t wProcessType;
  uint8_t bNrInPins;
  uint8_t bSourceID;
  ClusterDescriptor cluster;
  uint8_t bControlSize;
  uint8_t* bmControls;
  uint8_t iProcessing;
};
struct ReverberationProcessingUnitDescriptor{
  uint8_t bLength;
  uint8_t bDescriptorType;
  uint8_t bDescriptorSubtype;
  uint8_t bUnitID;
  uint16_t wProcessType;
  uint8_t bNrInPins;
  uint8_t bSourceID;
  ClusterDescriptor cluster;
  uint8_t bControlSize;
  uint8_t* bmControls;
  uint8_t iProcessing;
};

struct ChorusProcessingUnitDescriptor{
  uint8_t bLength;
  uint8_t bDescriptorType;
  uint8_t bDescriptorSubtype;
  uint8_t bUnitID;
  uint16_t wProcessType;
  uint8_t bNrInPins;
  uint8_t bSourceID;
  ClusterDescriptor cluster;
  uint8_t bControlSize;
  uint8_t* bmControls;
  uint8_t iProcessing;
};

struct DynamicRangeCompressorProcessnigUnitDescriptor{
  uint8_t bLength;
  uint8_t bDescriptorType;
  uint8_t bDescriptorSubtype;
  uint8_t bUnitID;
  uint16_t wProcessType;
  uint8_t bNrInPins;
  uint8_t bSourceID;
  ClusterDescriptor cluster;
  uint8_t bControlSize;
  uint8_t* bmControls;
  uint8_t iProcessing;
};

struct ExtensionUnitDescriptor{
  uint8_t bLength;
  uint8_t bDescriptorType;
  uint8_t bDescriptorSubtype;
  uint8_t bUnitID;
  uint16_t wExtensionCode;
  uint8_t bNrInPins;
  uint8_t* baSourceID;
  ClusterDescriptor cluster;
  uint8_t bControlSize;
  uint8_t* bmControls;
  uint8_t iExtension;
};

struct ExtensionUnit{
  struct ExtensionUnitDescriptor extension_unit_desc;
  char* extension_description;
};

struct AssociatedInterfaceDescriptor{
  uint8_t bLength;
  uint8_t bDescriptorType;
  uint8_t bDescriptorSubtype;
  uint8_t bInterfaceNr;
  void* association_specific; 
};

struct CopyProtectControlParameterBlock{
  uint8_t control_selector;
  uint8_t wLength;
  uint8_t bCopyProtect;
} __attribute__((packed));

// channel request
struct MuteControlParameterBlockFirstForm{
  uint8_t control_selector;
  uint8_t wLength;
  uint8_t bMute;
} __attribute__((packed));

// 0xFF channel request
struct MuteControlParameterBlockSecondForm{
  uint8_t control_selector;
  uint8_t wLength;
  uint8_t bMute[];
} __attribute__((packed));

struct VolumeControlParameterBlockFirstForm{
  uint8_t control_selector;
  uint16_t wLength;
  int16_t wVolume;
} __attribute__((packed));

struct VolumeControlParameterBlockSecondForm{
  uint8_t control_selector;
  uint8_t wLength; // * 2
  int8_t wVolume[];
} __attribute__((packed));

struct MixerControlParameterBlockFirstForm{
  uint16_t wLength;
  int16_t wMixer;
} __attribute__((packed));

struct MixerControlParameterBlockSecondForm{
  uint16_t wLength;
  int16_t wMixer[];
} __attribute__((packed));

struct SelectorControlParameterBlock{
  uint8_t wLength;
  uint8_t bSelector;
} __attribute__((packed));

// just implement the most common controls !

struct EnableProcessingControlParameterBlock{
  uint8_t control_selector;
  uint8_t wLength;
  uint8_t bEnable;
} __attribute__((packed));

struct ModeSelectControlParameterBlock{
  uint8_t control_selector;
  uint8_t wLength;
  uint8_t bMode;
} __attribute__((packed));

struct SpaciousnessControlParameterBlock{
  uint8_t control_selector;
  uint8_t wLength;
  uint8_t bSpaciousness;
} __attribute__((packed));

struct ReverbTypeControlParameterBlock{
  uint8_t control_selector;
  uint8_t wLength;
  uint8_t bReverbType;
} __attribute__((packed));

struct ReverbLevelControlParameterBlock{
  uint8_t control_selector;
  uint8_t wLength;
  uint8_t bReverbLevel;
} __attribute__((packed));

struct ReverbTimeControlParameterBlock{

};

struct ChorusLevelControlParameterBlock{
  uint8_t control_selector;
  uint8_t wLength;
  uint8_t bChorusLevel;
} __attribute__((packed));

struct ChorusModulationRateControlParamterBlock{

};

struct ChorusModulationDepthControlParameterBlock{

};

struct DynamicRangeCompressorRatioControlParameterBlock{

};

struct DynamicRangeCompressorMaxAmplControlParameterBlock{

};

struct SamplingFrequencyControlParameterBlock{
  uint8_t control_selector;
  uint16_t wLength;
  struct SampleFrequency tSampleFreq;
} __attribute__((packed));

struct PitchControlParameterBlock{
  uint8_t control_selector;
  uint16_t wLength;
  uint8_t bPitchEnable;
} __attribute__((packed));

enum CPL{
  CPL_0 = 0x00,
  CPL_1 = 0x01,
  CPL_2 = 0x02
};

enum AudioDataFormatType1_Codes{
  TYPE_1_UNDEFINED = 0x0000,
  PCM = 0x0001,
  PCM_8 = 0x0002,
  IEEE_FLOAT = 0x0003,
  ALAW = 0x0004,
  MULAW = 0x0005
};

enum AudioDataFormatType2_Codes{
  TYPE_2_UNDEFINED = 0x1000,
  MPEG = 0x1001,
  AC_3 = 0x1002
};

enum AudioDataFormatType3_Codes{
  TYPE_3_UNDEFINED = 0x2000,
  IEC1937_AC_3 = 0x2001,
  IEC1937_MPEG_1_LAYER1 = 0x2002,
  IEC1937_MPEG_1_LAYER2_OR_3 = 0x2003,
  IEC1937_MPEG_2_EXT = 0x2004,
  IEC1937_MPEG_2_LAYER1_LS = 0x2005,
  IEC1937_MPEG_2_LAYER2_3_LS = 0x2006
};

enum FormatTypeCodes{
  FORMAT_TYPE_UNDEFINED = 0x00,
  FORMAT_TYPE_1 = 0x01,
  FORMAT_TYPE_2 = 0x02,
  FORMAT_TYPE_3 = 0x03
};

struct FormatType{
  void* type_descriptor;
  enum FormatTypeCodes (*get_format_type)(void* type_descriptor);
  uint8_t (*get_total_channels)(void* type_descriptor);
  uint8_t (*get_subframe_size)(void* type_descriptor);
  uint8_t (*get_bit_depth)(void* type_descriptor);
  uint8_t (*get_sam_freq_type)(void* type_descriptor);
  SampleFrequency (*get_sam_frequency)(void* type_descriptor, uint8_t freq_num);
};

struct FormatSpecificType{
  
};

struct Type1_FormatTypeDescriptor{
  uint8_t bLength;
  uint8_t bDescriptorType;
  uint8_t bDescriptorSubtype;
  uint8_t bFormatType;
  uint8_t bNrChannels;
  uint8_t bSubframeSize;
  uint8_t bBitResolution;
  uint8_t bSamFreqType;
  void* samFreq;
} __attribute__((packed));

struct ContinuousSamplingFrequency{
  struct SampleFrequency tLower;
  struct SampleFrequency tUpper;
} __attribute__((packed));

struct DiscreteNumberSamplingFrequencies{
  struct SampleFrequency* tSam;
};

typedef struct AudioDev AudioDev;
typedef struct AudioDriver AudioDriver;
typedef struct AudioControlDescriptor AudioControlDescriptor;
typedef struct InputTerminalDescriptor InputTerminalDescriptor;
typedef struct OutputTerminalDescriptor OutputTerminalDescriptor;
typedef struct MixerUnitDescriptor MixerUnitDescriptor;
typedef struct SelectorUnitDescriptor SelectorUnitDescriptor;
typedef struct FeatureUnitDescriptor FeatureUnitDescriptor;
typedef struct MuteControlParameterBlockFirstForm MuteControlParameterBlockFirstForm;
typedef struct MuteControlParameterBlockSecondForm MuteControlParameterBlockSecondForm;
typedef struct VolumeControlParameterBlockFirstForm VolumeControlParameterBlockFirstForm;
typedef struct VolumeControlParameterBlockSecondForm VolumeControlParameterBlockSecondForm;
typedef struct CopyProtectControlParameterBlock CopyProtectControlParameterBlock;
typedef struct MixerControlParameterBlockFirstForm MixerControlParameterBlockFirstForm;
typedef struct MixerControlParameterBlockSecondForm MixerControlParameterBlockSecondForm;
typedef struct SelectorControlParameterBlock SelectorControlParameterBlock;
typedef enum   CPL CPL;
typedef enum   SpatialLocations SpatialLocations;
typedef struct ProcessingUnitDescriptor ProcessingUnitDescriptor;
typedef struct UP_DOWN_MixProcessingUnitDescriptor UP_DOWN_MixProcessingUnitDescriptor;
typedef struct DolbyPrologicProcessingUnitDescriptor DolbyPrologicProcessingUnitDescriptor;
typedef struct THREE_D_StereoExtenderProcessingUnitDescriptor THREE_D_StereoExtenderProcessingUnitDescriptor;
typedef struct ReverberationProcessingUnitDescriptor ReverberationProcessingUnitDescriptor;
typedef struct ChorusProcessingUnitDescriptor ChorusProcessingUnitDescriptor;
typedef struct DynamicRangeCompressorProcessnigUnitDescriptor DynamicRangeCompressorProcessnigUnitDescriptor;
typedef struct ExtensionUnitDescriptor ExtensionUnitDescriptor;
typedef struct AssociatedInterfaceDescriptor AssociatedInterfaceDescriptor;
typedef struct ASAudioEndpoint ASAudioEndpoint;
typedef enum   LockDelayUnit LockDelayUnit;

#endif