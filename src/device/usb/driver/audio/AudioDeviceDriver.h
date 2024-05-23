#ifndef AUDIO_DEVICE_DRIVER_INCLUDE
#define AUDIO_DEVICE_DRIVER_INCLUDE

#include "../UsbDriver.h"

#define AUDIO_CONFIGURATION_BUFFER 2048

#define __INIT_AUDIO_DRIVER__(name) \

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

struct ACInterface {
  struct AudioControlDescriptor ac_desc;
  struct InputTerminalDescriptor in_terminal_desc;
  struct OutputTerminalDescriptor out_terminal_desc;
  struct MixerUnitDescriptor mixer_unit_desc;
  struct SelectorUnitDescriptor selector_unit_desc;
  struct FeatureUnitDescriptor feature_unit_desc;
  struct ProcessingUnitDescriptor processing_unit_desc;
  struct ExtensionUnitDescriptor extension_unit_desc;
  struct AssociatedInterfaceDescriptor associated_interface_desc;
};

struct ASEndpoint {
  struct AudioStreamingIsoEndpointDescriptor as_iso_endpoint;
};

struct ASInterface {
  struct AudioStreamingDescriptor as_desc;
  struct ASEndpoint* as_endpoint;
};

struct AudioInterfaceCollection {
  struct ACInterface* ac_itf;
  struct ASInterface** as_itf;

  uint8_t num_streaming_interfaces;
};

typedef struct ACInterface ACInterface;
typedef struct ASInterface ASInterface;
typedef struct AudioInterfaceCollection AudioInterfaceCollection;
typedef struct ASEndpoint ASEndpoint;

struct AudioDev {
    UsbDev* usb_dev;
    unsigned int endpoint_addr;
    void* buffer;
    unsigned int buffer_size;
    uint8_t priority;
    Interface* interface;
    uint16_t interval;
    UsbDriver* usb_driver;
    AudioInterfaceCollection* audio_collection;
    int16_t curr_volume;
    int16_t max_volume;
    int16_t min_volume;

    void (*callback)(UsbDev* dev, uint32_t status, void* data);
};

struct AudioDriver {
    struct UsbDriver driver;
    struct AudioDev dev[MAX_DEVICES_PER_USB_DRIVER];
    uint8_t audio_map[MAX_DEVICES_PER_USB_DRIVER];

    void (*new_driver)(struct AudioDriver* driver);
};

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

struct AudioControlDescriptor {
  uint8_t bLength;
  uint8_t bDescriptorType;
  uint8_t bDescriptorSubtype;
  uint16_t bcdADC;
  uint16_t wTotalLength;
  uint8_t bInCollection;
  uint8_t baInterfaceNr[];
} __attribute__((packed));

struct AudioStreamingDescriptor {
  uint8_t bLength;
  uint8_t bDescriptorType;
  uint8_t bDescriptorSubtype;
  uint8_t bTerminalLink;
  uint8_t bDelay;
  uint16_t wFormatTag;
} __attribute__((packed));

struct AudioStreamingIsoEndpointDescriptor {
  uint8_t bLength;
  uint8_t bDescriptorType;
  uint8_t bDescriptorSubtype;
  uint8_t bmAttributes;
  uint8_t bLockDelayUnits;
  
} __attribute__((packed));

struct InputTerminalDescriptor{
  uint8_t bLength;
  uint8_t bDescriptorType;
  uint8_t bDescriptorSubtype;
  uint8_t bTerminalID;
  uint16_t wTerminalType;
  uint8_t bAssocTerminal;
  ClusterDescriptor cluster_descriptor;
  uint8_t iTerminal;
} __attribute__((packed));

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

struct MixerUnitDescriptor{
  uint8_t bLength;
  uint8_t bDescriptorType;
  uint8_t bDescriptorSubtype;
  uint8_t bUnitID;
  uint8_t bNrInPins;
  uint8_t* baSourceID;
  ClusterDescriptor cluster_desc;
  uint8_t* bmControls;
  uint8_t bmControlsLength;
  uint8_t iMixer;
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

struct ProcessingUnitDescriptor{
  uint8_t bLength;
  uint8_t bDescriptorType;
  uint8_t bDescriptorSubtype;
  uint8_t bUnitID;
  uint16_t wProcessType;
  uint8_t bNrInPins;
  uint8_t* baSource;
  ClusterDescriptor cluster;
  uint8_t bControlSize;
  uint8_t* bmControls;
  uint8_t iProcessing;
  void* process_specific_descriptor;
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
  uint8_t* baSource;
  ClusterDescriptor cluster;
  uint8_t bControlSize;
  uint8_t* bmControls;
  uint8_t iExtension;
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

enum CPL{
  CPL_0 = 0x00,
  CPL_1 = 0x01,
  CPL_2 = 0x02
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
typedef enum CPL CPL;
typedef enum SpatialLocations SpatialLocations;
typedef struct ProcessingUnitDescriptor ProcessingUnitDescriptor;
typedef struct UP_DOWN_MixProcessingUnitDescriptor UP_DOWN_MixProcessingUnitDescriptor;
typedef struct DolbyPrologicProcessingUnitDescriptor DolbyPrologicProcessingUnitDescriptor;
typedef struct THREE_D_StereoExtenderProcessingUnitDescriptor THREE_D_StereoExtenderProcessingUnitDescriptor;
typedef struct ReverberationProcessingUnitDescriptor ReverberationProcessingUnitDescriptor;
typedef struct ChorusProcessingUnitDescriptor ChorusProcessingUnitDescriptor;
typedef struct DynamicRangeCompressorProcessnigUnitDescriptor DynamicRangeCompressorProcessnigUnitDescriptor;
typedef struct ExtensionUnitDescriptor ExtensionUnitDescriptor;
typedef struct AssociatedInterfaceDescriptor AssociatedInterfaceDescriptor;

#endif