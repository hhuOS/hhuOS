#ifndef AUDIO_DEVICE_DRIVER_INCLUDE
#define AUDIO_DEVICE_DRIVER_INCLUDE

#include "../../interfaces/MapInterface.h"
#include "../../interfaces/MutexInterface.h"
#include "../UsbDriver.h"

#define AUDIO_CONFIGURATION_BUFFER 2048

#define __INIT_AUDIO_DRIVER__(name, driver_name, entry)                        \
  __ENTRY__(name, get_free_audio_dev) = &get_free_audio_dev;                   \
  __ENTRY__(name, match_audio_dev) = &match_audio_dev;                         \
  __ENTRY__(name, free_audio_dev) = &free_audio_dev;                           \
  __ENTRY__(name, configure_audio_device) = &configure_audio_device;           \
  __ENTRY__(name, sync_streaming_interface) = &sync_streaming_interface;       \
  __ENTRY__(name, audio_device_routine) = &audio_device_routine;               \
  __ENTRY__(name, set_sound_value) = &set_sound_value;                         \
  __ENTRY__(name, set_max_sound_value) = &set_max_sound_value;                 \
  __ENTRY__(name, set_min_sound_value) = &set_min_sound_value;                 \
  __ENTRY__(name, get_max_volume) = &get_max_volume;                           \
  __ENTRY__(name, get_min_volume) = &get_min_volume;                           \
  __ENTRY__(name, get_current_volume) = &get_current_volume;                   \
  __ENTRY__(name, mute) = &mute;                                               \
  __ENTRY__(name, unmute) = &unmute;                                           \
  __ENTRY__(name, get_mute_attribute) = &get_mute_attribute;                   \
  __ENTRY__(name, set_sampling_frequency) = &set_sampling_frequency;           \
  __ENTRY__(name, request_device_attributes) = &request_device_attributes;     \
  __ENTRY__(name, switch_if_zero_bandwidth) = &switch_if_zero_bandwidth;       \
  __ENTRY__(name, configure_interfaces) = &configure_interfaces;               \
  __ENTRY__(name, get_top_layer_buffer) = &get_top_layer_buffer;               \
  __ENTRY__(name, get_iso_endpoint) = &get_iso_endpoint;                       \
  __ENTRY__(name, get_sampling_frequency) = &get_sampling_frequency;           \
  __ENTRY__(name, get_upstream_channels_by_source_id) = &get_upstream_channels_by_source_id; \
  __ENTRY__(name, get_upstream_channels_by_source_id_depth) = &get_upstream_channels_by_source_id_depth; \
  __ENTRY__(name, find_structure_by_id) = &find_structure_by_id;               \
  __ENTRY__(name, get_upstream_structure) = &get_upstream_structure;           \
  __ENTRY__(name, mixer_build_routine) = &mixer_build_routine;                 \
  __ENTRY__(name, is_bm_control_programmable) = &is_bm_control_programmable;   \
  __ENTRY__(name, mixer_bmControlLength) = &mixer_bmControlLength;             \
  __ENTRY__(name, selector_build_routine) = &selector_build_routine;           \
  __ENTRY__(name, feature_build_routine) = &feature_build_routine;             \
  __ENTRY__(name, processing_unit_build_routine) = &processing_unit_build_routine; \
  __ENTRY__(name, extension_unit_build_routine) = &extension_unit_build_routine; \
  __ENTRY__(name, associated_interface_build_routine) = &associated_interface_build_routine; \
  __ENTRY__(name, handle_ac_routine) = &handle_ac_routine;                     \
  __ENTRY__(name, handle_as_routine) = &handle_as_routine;                     \
  __ENTRY__(name, handle_as_endpoint_routine) = &handle_as_endpoint_routine;   \
  __ENTRY__(name, audio_device_routine) = &audio_device_routine;               \
  __ENTRY__(name, get_loc_by_channel_number) = &get_loc_by_channel_number;     \
  __ENTRY__(name, get_channel_number_by_loc) = &get_channel_number_by_loc;     \
  __ENTRY__(name, retrieve_process_type) = &retrieve_process_type;             \
  __ENTRY__(name, audio_mixer_control) = &audio_mixer_control;                 \
  __ENTRY__(name, audio_volume_control) = &audio_volume_control;               \
  __ENTRY__(name, audio_mute_control) = &audio_mute_control;                   \
  __ENTRY__(name, recover_unit_from_as_interface) = &recover_unit_from_as_interface; \
  __ENTRY__(name, feature_control) = &feature_control;                         \
  __ENTRY__(name, set_mixer_unit_control) = &set_mixer_unit_control;           \
  __ENTRY__(name, get_mixer_unit_control) = &get_mixer_unit_control;           \
  __ENTRY__(name, audio_selector_control) = &audio_selector_control;           \
  __ENTRY__(name, selector_set_cur) = &selector_set_cur;                       \
  __ENTRY__(name, audio_processing_control) = &audio_processing_control;       \
  __ENTRY__(name, audio_extension_control) = &audio_extension_control;         \
  __ENTRY__(name, audio_enable_processing_control) = &audio_enable_processing_control; \
  __ENTRY__(name, support_pitch_control) = &support_pitch_control;             \
  __ENTRY__(name, audio_endpoint_controls) = &audio_endpoint_controls;         \
  __ENTRY__(name, sampling_frequency_control) = &sampling_frequency_control;   \
  __ENTRY__(name, sampling_pitch_control) = &sampling_pitch_control;           \
  __ENTRY__(name, enable_processing_control) = &enable_processing_control;     \
  __ENTRY__(name, disable_processing_control) = &disable_processing_control;   \
  __ENTRY__(name, audio_mode_select_control) = &audio_mode_select_control;     \
  __ENTRY__(name, audio_spaciousness_control) = &audio_spaciousness_control;   \
  __ENTRY__(name, audio_reverb_type_control) = &audio_reverb_type_control;     \
  __ENTRY__(name, audio_reverb_level_control) = &audio_reverb_level_control;   \
  __ENTRY__(name, audio_chorus_level_control) = &audio_chorus_level_control;   \
  __ENTRY__(name, audio_set_cur_spaciousness_control) = &audio_set_cur_spaciousness_control; \
  __ENTRY__(name, set_audio_mode_attribute) = &set_audio_mode_attribute;       \
  __ENTRY__(name, enable_extension_unit) = &enable_extension_unit;             \
  __ENTRY__(name, disable_extension_unit) = &disable_extension_unit;           \
  __ENTRY__(name, audio_copy_protect_control) = &audio_copy_protect_control;   \
  __ENTRY__(name, set_copy_protect) = &set_copy_protect;                       \
  __ENTRY__(name, get_copy_protect) = &get_copy_protect;                       \
  __ENTRY__(name, set_volume_resolution) = &set_volume_resolution;             \
  __ENTRY__(name, back_propagate_input) = &back_propagate_input;               \
  __ENTRY__(name, back_propagate_output) = &back_propagate_output;             \
  __ENTRY__(name, back_propagate_output_processing) = &back_propagate_output_processing; \
  __ENTRY__(name, __get_terminal) = &__get_terminal;                           \
  __ENTRY__(name, __get_sample_frequency) = &__get_sample_frequency;           \
  __ENTRY__(name, __get_sub_frame_size) = &__get_sub_frame_size;               \
  __ENTRY__(name, __get_total_supported_frequencies) =                         \
      &__get_total_supported_frequencies;                                      \
  __ENTRY__(name, __get_1ms_size) = &__get_1ms_size;                           \
  __ENTRY__(name, __get_bit_depth) = &__get_bit_depth;                         \
  __ENTRY__(name, __get_frame_size) = &__get_frame_size;                       \
  __ENTRY__(name, __get_num_channels) = &__get_num_channels;                   \
  __ENTRY__(name, __is_freq_set) = &__is_freq_set;                             \
  __ENTRY__(name, __convert_to_class_specific_as_interface) =                  \
      &__convert_to_class_specific_as_interface;                               \
  __ENTRY__(name, __convert_to_class_specific_ac_interface) =                  \
      &__convert_to_class_specific_ac_interface;                               \
  __ENTRY__(name, __get_audio_dev) = &__get_audio_dev;                         \
  __ENTRY__(name, __match_terminal_type) = &__match_terminal_type;             \
  __ENTRY__(name, __is_as_output_terminal) = &__is_as_output_terminal;         \
  __ENTRY__(name, __is_as_input_terminal) = &__is_as_input_terminal;           \
  __ENTRY__(name, __get_as_interface_by_terminal) =                            \
      &__get_as_interface_by_terminal;                                         \
  __ENTRY__(name, __get_sync_delay) = &__get_sync_delay;                       \
  __ENTRY__(name, __get_sync_unit) = &__get_sync_unit;                         \
  __ENTRY__(name, __convert_to_class_specific_as_endpoint) =                   \
      &__convert_to_class_specific_as_endpoint;                                \
  __ENTRY__(name, __get_volume) = &__get_volume;                               \
  __ENTRY__(name, __get_mute_attribute) = &__get_mute_attribute;               \
  __ENTRY__(name, __get_max_volume) = &__get_max_volume;                       \
  __ENTRY__(name, __get_min_volume) = &__get_min_volume;                       \
  __ENTRY__(name, __has_zero_bandwidth_setting) = &__has_zero_bandwidth_setting; \
  __ENTRY__(name, __match_frequency) = &__match_frequency;                    \
  __ENTRY__(name, __clear_low_level_buffers) = &__clear_low_level_buffers; \
  __ENTRY__(name, __is_zero_bandwidth_active) = &__is_zero_bandwidth_active; \
  __ENTRY__(name, __get_class_specific_as_interface) = &__get_class_specific_as_interface; \
  __ENTRY__(name, __is_cs_interface) = &__is_cs_interface; \
  __ENTRY__(name, __is_as_endpoint) = &__is_as_endpoint; \
  __ENTRY__(name, __is_ac_interface) = &__is_ac_interface; \
  __ENTRY__(name, __is_as_interface) = &__is_as_interface; \
  __ENTRY__(name, __is_format_type) = &__is_format_type; \
  __ENTRY__(name, __is_input_terminal) = &__is_input_terminal; \
  __ENTRY__(name, __is_output_terminal) = &__is_output_terminal; \
  __ENTRY__(name, __is_mixer) = &__is_mixer; \
  __ENTRY__(name, __is_selector) = &__is_selector; \
  __ENTRY__(name, __is_feature) = &__is_feature; \
  __ENTRY__(name, __is_processing_unit) = &__is_processing_unit; \
  __ENTRY__(name, __is_extension_unit) = &__is_extension_unit; \
  __ENTRY__(name, __is_zero_frequency) = &__is_zero_frequency; \
  __ENTRY__(name, __is_channel_in_cluster) = &__is_channel_in_cluster; \
  __ENTRY__(name, __set_active_and_passive_buffers) = &__set_active_and_passive_buffers; \
  __ENTRY__(name, __set_default_audio_properties) = &__set_default_audio_properties; \
  __ENTRY__(name, __set_requests) = &__set_requests; \
  __ENTRY__(name, __get_requests) = &__get_requests; \
  __ENTRY__(name, __request_common_get) = &__request_common_get; \
  __ENTRY__(name, __request_common_set) = &__request_common_set; \
  __ENTRY__(name, __request_mixer_get) = &__request_mixer_get; \
  __ENTRY__(name, __request_mixer_set) = &__request_mixer_set; \
  __ENTRY__(name, __request_feature_get) = &__request_feature_get; \
  __ENTRY__(name, __request_feature_set) = &__request_feature_set; \
  __ENTRY__(name, __request_endpoint_get) = &__request_endpoint_get; \
  __ENTRY__(name, __request_endpoint_set) = &__request_endpoint_set; \
  __ENTRY__(name, __parse_bmControls) = &__parse_bmControls; \
  __ENTRY__(name, __parse_bmaControls) = &__parse_bmaControls; \
  __ENTRY__(name, __valid_request_form) = &__valid_request_form; \
  __ENTRY__(name, __get_total_channels_in_cluster) = &__get_total_channels_in_cluster; \
  __ENTRY__(name, __search_for_unit_by_id) = &__search_for_unit_by_id; \
  __ENTRY__(name, __get_unit_id) = &__get_unit_id; \
  __ENTRY__(name, __get_unit_sub_type) = &__get_unit_sub_type; \
  __ENTRY__(name, __get_terminal_type) = &__get_terminal_type; \
  __ENTRY__(name, __get_feature_bmControl_of_channel) = &__get_feature_bmControl_of_channel; \
  __ENTRY__(name, __bmControl_supported) = &__bmControl_supported; \
  __ENTRY__(name, __bmControl_mute_supported) = &__bmControl_mute_supported; \
  __ENTRY__(name, __bmControl_volume_supported) = &__bmControl_volume_supported; \
  __ENTRY__(name, __bmControl_bass_supported) = &__bmControl_bass_supported; \
  __ENTRY__(name, __bmControl_mid_supported) = &__bmControl_mid_supported; \
  __ENTRY__(name, __bmControl_graphic_equalizer_supported) = &__bmControl_graphic_equalizer_supported; \
  __ENTRY__(name, __bmControl_delay_supported) = &__bmControl_delay_supported; \
  __ENTRY__(name, __parse_waModes) = &__parse_waModes; \
  __ENTRY__(name, __assign_process) = &__assign_process; \
  __ENTRY__(name, __input_terminal_present) = &__input_terminal_present; \
  __ENTRY__(name, __output_terminal_present) = &__output_terminal_present; \
  __ENTRY__(name, __feature_unit_present) = &__feature_unit_present; \
  __ENTRY__(name, __processing_unit_present) = &__processing_unit_present; \
  __ENTRY__(name, __extension_unit_present) = &__extension_unit_present; \
  __ENTRY__(name, __selector_unit_present) = &__selector_unit_present; \
  __ENTRY__(name, __mixer_unit_present) = &__mixer_unit_present; \
  __ENTRY__(name, __audio_interface_present) = &__audio_interface_present; \
  __ENTRY__(name, __support_enable_processing_control) = &__support_enable_processing_control; \
  __ENTRY__(name, __common_control) = &__common_control; \
  __ENTRY__(name, __support_mode_select_control) = &__support_mode_select_control; \
  __ENTRY__(name, __support_spaciousness_control) = &__support_spaciousness_control; \
  __ENTRY__(name, __reverb_control) = &__reverb_control; \
  __ENTRY__(name, __support_reverb_type_control) = &__support_reverb_type_control; \
  __ENTRY__(name, __support_reverb_level_control) = &__support_reverb_level_control; \
  __ENTRY__(name, __support_reverb_time_control) = &__support_reverb_time_control; \
  __ENTRY__(name, __support_reverb_delay_feedback_control) = &__support_reverb_delay_feedback_control; \
  __ENTRY__(name, __chorus_control) = &__chorus_control; \
  __ENTRY__(name, __support_chorus_level_control) = &__support_chorus_level_control; \
  __ENTRY__(name, __support_chorus_modulation_rate_control) = &__support_chorus_modulation_rate_control; \
  __ENTRY__(name, __support_chorus_modulation_depth_control) = &__support_chorus_modulation_depth_control; \
  __ENTRY__(name, __dynamic_control) = &__dynamic_control; \
  __ENTRY__(name, __support_compression_ratio_control) = &__support_compression_ratio_control; \
  __ENTRY__(name, __support_max_amplitude_control) = &__support_max_amplitude_control; \
  __ENTRY__(name, __support_threshold_control) = &__support_threshold_control; \
  __ENTRY__(name, __support_attack_time_control) = &__support_attack_time_control; \
  __ENTRY__(name, __support_release_time_control) = &__support_release_time_control; \
  \
  __SUPER__(name, probe) = &probe_audio;                                       \
  __SUPER__(name, disconnect) = &disconnect_audio;                             \
  __SUPER__(name, new_usb_driver) = &new_usb_driver;                           \
                                                                               \
  __CALL_SUPER__(name->super, new_usb_driver, driver_name, entry)

#define __SET_REQUEST__ 0x01
#define __GET_REQUEST__ 0x02

#define __UPPER_VOL_BOUNDARY__ 0x7FFF
#define __LOWER_VOL_BOUNDARY__ 0x8001
#define __SILENCE_VOL__ 0x8000

#define __CHANNEL_OR_INC__(driver, cluster, loc, spatial_loc, channel_number)  \
  __IF_RET_SELF__(loc == SPATIAL_LOC_L, channel_number);                       \
  __IF_CUSTOM__(__is_channel_in_cluster(driver, cluster, SPATIAL_LOC_L),       \
                channel_number++)

#define __LOCATION_OR_INC__(driver, cluster, loc, channel_number,              \
                            channel_count)                                     \
  __IF_CUSTOM__(__is_channel_in_cluster(driver, cluster, loc),                 \
                __IF_ELSE__((channel_number == channel_count), return loc,     \
                            channel_count++));

#define __PARSE_BREAK_ROUTINE__(function_routine)                              \
  function_routine;                                                            \
  break;

#define __SIMPLE_SPECIFIC_PROCESSING_UNIT_ROUTINE__(                           \
    driver, start, type, mem_service)              \
  __ALLOC_KERNEL_MEM_S__(m, type, processing_unit_desc);                       \
  processing_unit_desc->bLength = *start;                                      \
  processing_unit_desc->bDescriptorType = *(start + 1);                        \
  processing_unit_desc->bDescriptorSubtype = *(start + 2);                     \
  processing_unit_desc->bUnitID = *(start + 3);                                \
  processing_unit_desc->wProcessType = *(start + 4);                           \
  processing_unit_desc->bNrInPins = *(start + 6);                              \
  processing_unit_desc->bSourceID = *(start + 7);                              \
  __parse_cluster(driver, &processing_unit_desc->cluster, start, 8);           \
  processing_unit_desc->bControlSize = *(start + 12);                          \
  __parse_bmControls(driver, start + 13, 0,                                    \
                     &processing_unit_desc->bControlSize, mem_service);        \
  processing_unit_desc->iProcessing =                                          \
      *(start + 13 + processing_unit_desc->bControlSize);

#define __EXT_SPECIFIC_PROCESSING_UNIT_ROUTINE__(                              \
    driver, start, type, mem_service)                                          \
  __SIMPLE_SPECIFIC_PROCESSING_UNIT_ROUTINE__(                                 \
      driver, start, type, mem_service);                                       \
  processing_unit_desc->bNrModes =                                             \
      *(start + 14 + processing_unit_desc->bControlSize);                      \
  __ALLOC_KERNEL_MEM_T__(mem_service, uint16_t, waModes,                       \
    processing_unit_desc->bNrModes);                                           \
  __parse_waModes(driver, start + 15 + processing_unit_desc->bControlSize,     \
                  &processing_unit_desc->bNrModes, waModes);                   \
  processing_unit_desc->waModes = waModes 

#define __SPACIOUSNESS_UPPER_BOUND__ 255
#define __SPACIOUSNESS_LOWER_BOUND__ 0

#define __MATCH_ID__(driver, unit_descriptor, id, target_bSourceID, cmd)       \
  __IF_CUSTOM__(id == target_bSourceID, cmd)

#define __UNIT_MATCH_ID__(driver, unit_descriptor, target_bSourceID, cmd)      \
  __MATCH_ID__(driver, unit_descriptor, unit_descriptor.bUnitID,               \
               target_bSourceID, cmd)

#define __TERMINAL_MATCH_ID__(driver, terminal_descriptor, target_bSourceID,   \
                              cmd)                                             \
  __MATCH_ID__(driver, terminal_descriptor, terminal_descriptor.bTerminalID,   \
               target_bSourceID, cmd)

#define __INPUT_TERMINAL_MATCH__(driver, ac_interface, input_terminal_id,      \
                                 index, cmd)                                   \
  __IF_COND__(__NOT_NULL__(ac_interface->in_terminal[index])){                 \
    __TERMINAL_MATCH_ID__(driver,                                              \
                        ac_interface->in_terminal[index]->in_terminal_desc,    \
                        input_terminal_id, cmd);                               \
  }

#define __OUTPUT_TERMINAL_MATCH__(driver, ac_interface, output_terminal_id,    \
                                  index, cmd)                                  \
  __IF_COND__(__NOT_NULL__(ac_interface->out_terminal[index])){                \
    __TERMINAL_MATCH_ID__(driver,                                              \
                        ac_interface->out_terminal[index]->out_terminal_desc,  \
                        output_terminal_id, cmd);                              \
  }

#define __FEATURE_UNIT_MATCH__(driver, ac_interface, feature_unit_id, index,   \
                               cmd)                                            \
  __IF_COND__(__NOT_NULL__(ac_interface->feature_unit[index])){                \
    __UNIT_MATCH_ID__(driver,                                                  \
                    ac_interface->feature_unit[index]->feature_unit_desc,      \
                    feature_unit_id, cmd);                                     \
  }

#define __MIXER_UNIT_MATCH__(driver, ac_interface, mixer_unit_id, index, cmd)  \
  __IF_COND__(__NOT_NULL__(ac_interface->mixer_unit[index])){                  \
    __UNIT_MATCH_ID__(driver, ac_interface->mixer_unit[index]->mixer_unit_desc,\
                    mixer_unit_id, cmd);                                       \
  }

#define __SELECTOR_UNIT_MATCH__(driver, ac_interface, selector_unit_id, index, \
                                cmd)                                           \
  __IF_COND__(__NOT_NULL__(ac_interface->selector_unit[index])){               \
    __UNIT_MATCH_ID__(driver,                                                  \
                    ac_interface->selector_unit[index]->selector_unit_desc,    \
                    selector_unit_id, cmd);                                    \
  }

#define __EXTENSION_UNIT_MATCH__(driver, ac_interface, extension_unit_id,      \
                                 index, cmd)                                   \
  __IF_COND__(__NOT_NULL__(ac_interface->extension_unit[index])){              \
    __UNIT_MATCH_ID__(driver,                                                  \
                    ac_interface->extension_unit[index]->extension_unit_desc,  \
                    extension_unit_id, cmd);                                   \
  }

#define __PROCESSING_UNIT_MATCH__(driver, ac_interface, processing_unit_id,    \
                                  index, cmd)                                  \
  __IF_COND__(__NOT_NULL__(ac_interface->processing_unit[index])){             \
    __UNIT_MATCH_ID__(                                                         \
      driver, ac_interface->processing_unit[index]->processing_unit_desc,      \
      processing_unit_id, cmd);                                                \
  }

#define __TOTAL_CHANNEL_MATCH_TERMINAL__(driver, terminal_descriptor,          \
                                         target_bSourceID)                     \
  __TERMINAL_MATCH_ID__(driver, terminal_descriptor, target_bSourceID,         \
                        return __get_total_channels_in_cluster(                \
                            driver, terminal_descriptor.cluster))

#define __TOTAL_CHANNEL_MATCH_UNIT__(driver, unit_descriptor,                  \
                                     target_bSourceID)                         \
  __UNIT_MATCH_ID__(                                                           \
      driver, unit_descriptor, target_bSourceID,                               \
      return __get_total_channels_in_cluster(driver, unit_descriptor.cluster))

#define __MULTI_UPSTREAM__(driver, ac_interface, unit, sub_type, bSource_off,  \
                           type)                                               \
  return find_structure_by_id(driver, ac_interface,                            \
                              *((((type *)unit)->baSourceID) + bSource_off),   \
                              sub_type);                                       \
  break;

#define __SINGLE_UPSTREAM__(driver, ac_interface, unit, sub_type, type)        \
  return find_structure_by_id(driver, ac_interface, ((type *)unit)->bSourceID, \
                              sub_type);                                       \
  break;

#define __UNIT_COUNT__(ac_interface, unit)                                     \
  ac_interface->unit_count_map[unit] == __MAX_UNITS__

#define __GET_UNIT_COUNT__(ac_interface, unit)                                 \
  ac_interface->unit_count_map[unit]

#define __UNIT_INC__(ac_interface, unit)                                       \
  ac_interface->unit_count_map[unit] =                                         \
      __GET_UNIT_COUNT__(ac_interface, unit) + 1

#define __ADD_UNIT__(ac_interface, unit, unit_type, unit_v)                    \
  ac_interface->unit[__GET_UNIT_COUNT__(ac_interface, unit_type)] = unit_v;    \
  __UNIT_INC__(ac_interface, unit_type)

#define __MAX_UPSTREAM_DEPTH__ 3
#define __MAX_UNITS__ 10
#define __MAX_STREAMING_INTERFACES__ 10

#define __TRAVERSE_UNIT__ __FOR_RANGE__(i, int, 0, __MAX_UNITS__)

#define __BACK_PROPAGATE__(driver, start_sub_type, unit_name, ac_interface, \
  cmd) \
  void* upstream_structure;   \
  void* units[100]; \
  enum ACInterfaceDescriptorSubtypes subtypes[100]; \
  units[0] = (void*)unit_name;  \
  subtypes[0] = start_sub_type; \
  uint8_t unit_count = 1;  \
  while(__NOT_ZERO__(unit_count)){ \
      void* unit = units[unit_count-1]; \
      enum ACInterfaceDescriptorSubtypes subtype = subtypes[unit_count-- -1]; \
      uint8_t baSource = 1; \
      if(subtype == MIXER_UNIT){ \
          __TYPE_CAST__(struct MixerUnit*, mixer_unit, unit); \
          baSource = mixer_unit->mixer_unit_desc.bNrInPins; \
      } \
      else if(subtype == SELECTOR_UNIT){ \
          __TYPE_CAST__(struct SelectorUnit*, sel_unit, unit); \
          baSource = sel_unit->selector_unit_desc.bNrInPins; \
      } \
      else if(subtype == PROCESSING_UNIT){ \
          __TYPE_CAST__(struct ProcessingUnit*, proc_unit, unit); \
          baSource = proc_unit->processing_unit_desc.bNrInPins; \
      } \
      else if(subtype == EXTENSION_UNIT){ \
          __TYPE_CAST__(struct ExtensionUnit*, ext_unit, unit); \
          baSource = ext_unit->extension_unit_desc.bNrInPins; \
      } \
      while(__NOT_ZERO__(baSource)){ \
          upstream_structure = get_upstream_structure(driver, ac_interface, \
              &subtype, unit, baSource); \
          cmd; \
          if(__NOT_NULL__(upstream_structure)) units[unit_count++] = upstream_structure; \
          baSource--; \
      } \
  }

#define __BACK_PROPAGATE_OUT_PROCESSING__(driver, unit_name, ac_interface, \
  sub_target_search) \
  __BACK_PROPAGATE__(driver, OUTPUT_TERMINAL, unit_name, ac_interface, \
    if((subtype == PROCESSING_UNIT) && (__STRUCT_CALL__(driver, retrieve_process_type, \
      (uint8_t*)upstream_structure) == sub_target_search)) return upstream_structure)

#define __BACK_PROPAGATE_IN__(driver, start_sub_type, unit_name, ac_interface, \
  target_id) \
  __BACK_PROPAGATE__(driver, start_sub_type, unit_name, ac_interface, \
    if(__get_unit_id(driver, upstream_structure) == target_id) return unit_name)

#define __BACK_PROPAGATE_OUT__(driver, unit_name, ac_interface, \
  target_search) \
  __BACK_PROPAGATE__(driver, OUTPUT_TERMINAL, unit_name, ac_interface, \
    if(subtype == target_search) return upstream_structure)

#define __RECOVER_FEATURE_UNIT__(driver, audio_dev, interface) \
  recover_unit_from_as_interface(driver, audio_dev, interface, FEATURE_UNIT, 0)

#define __RECOVER_MIXER_UNIT__(driver, audio_dev, interface) \
  recover_unit_from_as_interface(driver, audio_dev, interface, MIXER_UNIT, 0)

#define __RECOVER_SELECTOR_UNIT__(driver, audio_dev, interface) \
  recover_unit_from_as_interface(driver, audio_dev, interface, SELECTOR_UNIT, 0)

#define __RECOVER_PROCESSING_UNIT__(driver, audio_dev, interface, subtype) \
  recover_unit_from_as_interface(driver, audio_dev, interface, PROCESSING_UNIT, subtype)

#define __RECOVER_EXTENSION_UNIT__(driver, audio_dev, interface) \
  recover_unit_from_as_interface(driver, audio_dev, interface, EXTENSION_UNIT, 0)

#define __RECOVER_MIXER_UNIT__(driver, audio_dev, interface) \
  recover_unit_from_as_interface(driver, audio_dev, interface, MIXER_UNIT, 0)

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
  uint8_t *baInterfaceNr;
} __attribute__((packed));

struct AudioStreamingIsoEndpointDescriptor {
  uint8_t bLength;
  uint8_t bDescriptorType;
  uint8_t bDescriptorSubtype;
  uint8_t bmAttributes;
  uint8_t bLockDelayUnits;
  uint16_t wLockDelay;
} __attribute__((packed));

struct SampleFrequency {
  uint16_t sampleFreq_lo;
  uint8_t sampleFreq_hi;
} __attribute__((packed));

typedef struct SampleFrequency SampleFrequency;

#define __SAMPLE_FREQUENCY_OF__(value) \
  (SampleFrequency) {.sampleFreq_lo = (uint16_t)(value & 0xFFFF), \
    .sampleFreq_hi = (uint8_t)(__16_BIT_L_SHIFT__((value & 0xFF0000)))}

#define __8000_HZ__                                                            \
  (SampleFrequency) { .sampleFreq_lo = 8000, .sampleFreq_hi = 0 }
#define __16000_HZ__                                                           \
  (SampleFrequency) { .sampleFreq_lo = 16000, .sampleFreq_hi = 0 }
#define __24000_HZ__                                                           \
  (SampleFrequency) { .sampleFreq_lo = 24000, .sampleFreq_hi = 0 }
#define __32000_HZ__                                                           \
  (SampleFrequency) { .sampleFreq_lo = 32000, .sampleFreq_hi = 0 }
#define __44100_HZ__                                                           \
  (SampleFrequency) { .sampleFreq_lo = 44100, .sampleFreq_hi = 0 }
#define __48000_HZ__                                                           \
  (SampleFrequency) { .sampleFreq_lo = 48000, .sampleFreq_hi = 0 }

struct ACInterface {
  struct AudioControlDescriptor ac_desc;
  struct InputTerminal *in_terminal[__MAX_UNITS__];
  struct OutputTerminal *out_terminal[__MAX_UNITS__];
  struct MixerUnit *mixer_unit[__MAX_UNITS__];
  struct SelectorUnit *selector_unit[__MAX_UNITS__];
  struct FeatureUnit *feature_unit[__MAX_UNITS__];
  struct ProcessingUnit *processing_unit[__MAX_UNITS__];
  struct ExtensionUnit *extension_unit[__MAX_UNITS__];
  struct AssociatedInterface *associated_interface[__MAX_UNITS__];
  uint8_t unit_count_map[__MAX_UNITS__];
};

struct ASEndpoint {
  struct AudioStreamingIsoEndpointDescriptor as_iso_endpoint;
};

struct ASInterface {
  struct AudioStreamingDescriptor as_desc;
  struct FormatType *format_type;
  struct SampleFrequency current_freq;
  uint8_t bPitchEnable;
  uint8_t terminal_type;

  uint8_t *buffer_first;
  uint8_t *buffer_second;
  uint8_t active_buffer;
  unsigned int buffer_size;
  uint32_t qh_id;
};

/*struct AudioInterfaceCollection {
  struct ACInterface* ac_itf;
  struct ASInterface** as_itf;
}; */

enum LockDelayUnit {
  LOCK_UNIT_UNDEFINED = 0x00,
  LOCK_UNIT_MS = 0x01,
  LOCK_PCM_SAMPLES = 0x02
};

typedef struct ACInterface ACInterface;
typedef struct ASInterface ASInterface;
typedef struct AudioInterfaceCollection AudioInterfaceCollection;
typedef struct ASEndpoint ASEndpoint;

struct AudioDev {
  UsbDev *usb_dev;
  void *buffer_first;
  void *buffer_second;
  unsigned int buffer_size;
  uint8_t priority;
  Interface *audio_control_interface;
  Interface *audio_streaming_interfaces[__MAX_STREAMING_INTERFACES__];
  uint8_t audio_streaming_interfaces_num;
  uint16_t interval;
  UsbDriver *usb_driver;
  int16_t curr_volume;
  int16_t max_volume;
  int16_t min_volume;
  uint8_t muted;

  void (*callback)(UsbDev *dev, Interface *interface, uint32_t status,
                   void *data);
};

enum SpatialLocations {
  SPATIAL_LOC_L = 0x01,
  SPATIAL_LOC_R = 0x02,
  SPATIAL_LOC_C = 0x04,
  SPATIAL_LOC_LFE = 0x08,
  SPATIAL_LOC_LS = 0x10,
  SPATIAL_LOC_RS = 0x20,
  SPATIAL_LOC_LC = 0x40,
  SPATIAL_LOC_RC = 0x80,
  SPATIAL_LOC_S = 0x100,
  SPATIAL_LOC_SL = 0x200,
  SPATIAL_LOC_SR = 0x400,
  SPATIAL_LOC_T = 0x800
};

struct ClusterDescriptor {
  uint8_t bNrChannels;
  uint16_t wChannelConfig;
  uint8_t iChannelNames;
} __attribute__((packed));

typedef struct ClusterDescriptor ClusterDescriptor;

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
// waModes :
// left,right,center = 0x0007
// left,right,surround = 0x0103
// left,right,center,surround = 0x0107

struct UP_DOWN_MixProcessingUnitDescriptor {
  uint8_t bLength;
  uint8_t bDescriptorType;
  uint8_t bDescriptorSubtype;
  uint8_t bUnitID;
  uint16_t wProcessType;
  uint8_t bNrInPins;
  uint8_t bSourceID;
  ClusterDescriptor cluster;
  uint8_t bControlSize;
  uint8_t *bmControls;
  uint8_t iProcessing;
  uint8_t bNrModes;
  uint16_t *waModes;
};

struct ProcessingUnitDescriptor {
  uint8_t bLength;
  uint8_t bDescriptorType;
  uint8_t bDescriptorSubtype;
  uint8_t bUnitID;
  uint16_t wProcessType;
  uint8_t bNrInPins;
  uint8_t *baSourceID;
  ClusterDescriptor cluster;
  uint8_t bControlSize;
  uint8_t *bmControls;
  uint8_t iProcessing;
  void *process_specific_descriptor;
};

enum FeatureUnitbmaControls {
  BIT_MUTE = 0x0001,
  BIT_VOL = 0x0002,
  BIT_BASS = 0x0004,
  BIT_MID = 0x0008,
  BIT_TREBLE = 0x0010,
  BIT_GRAPHIC_EQ = 0x0020,
  BIT_AUTO_GAIN = 0x0040,
  BIT_DELAY = 0x0080,
  BIT_BASS_BOOST = 0x0100,
  BIT_LOUDNESS = 0x0200
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

enum CPL { CPL_0 = 0x00, CPL_1 = 0x01, CPL_2 = 0x02 };

enum ReverbTypes {
  ROOM_ONE = 0x00,
  ROOM_TWO = 0x01,
  ROOM_THREE = 0x02,
  HALL_ONE = 0x03,
  HALL_TWO = 0x04,
  PLATE = 0x05,
  DELAY = 0x06,
  PANNING_DELAY = 0x07
};

enum ReverbProcessingUnitControl{
  REVERB_TYPE  = 0x02,
  REVERB_LEVEL = 0x04,
  REVERB_TIME  = 0x08,
  REVERB_DELAY_FEEDBACK = 0x10
};

enum ChorusProcessingUnitControl{
  CHORUS_LEVEL = 0x02,
  CHORUS_MODULATION_RATE = 0x04,
  CHORUS_MODULATION_DEPTH = 0x08
};

enum DynamicRangeUnitControl{
  COMPRESSION_RATIO = 0x02,
  MAX_AMPLITUDE     = 0x04,
  THRESHOLD         = 0x08,
  ATTACK_TIME       = 0x10,
  RELEASE_TIME      = 0x20
};

#define __BASIC_REQUEST_PROTO__(leading_name) \
  void (*__ ## leading_name ## _requests)(struct AudioDriver* driver, UsbDev* dev, \
    Interface* interface, UsbDeviceRequest* device_req, uint8_t recipient, \
    uint8_t bRequest, uint8_t wValueHigh, uint8_t wValueLow, uint8_t wIndexHigh, \
    uint8_t wIndexLow, void* data, uint8_t len, callback_function callback, \
    uint8_t flags)

#define __COMMON_REQUEST_PROTO__(proto_type) \
  void (*__request_common_ ## proto_type)(struct AudioDriver* driver, UsbDev* dev, \
    Interface* interface, UsbDeviceRequest* device_req, uint8_t bRequest, \
    uint8_t control_selector, uint8_t audio_interface, uint8_t terminal_id, \
    void* data, uint8_t len, callback_function callback, uint8_t flags)

#define __MIXER_REQUEST_PROTO__(proto_type) \
  void (*__request_mixer_ ## proto_type)(struct AudioDriver* driver, UsbDev* dev, \
    Interface* interface, UsbDeviceRequest* device_req, uint8_t bRequest, uint8_t icn, uint8_t ocn, \
    uint8_t mixer_unit_id, uint8_t audio_interface, void* data, uint8_t len, \
    callback_function callback, uint8_t flags)

#define __FEATURE_REQUEST_PROTO__(proto_type) \
  void (*__request_feature_ ## proto_type)(struct AudioDriver* driver, UsbDev* dev, \
    Interface* interface, UsbDeviceRequest* device_req, uint8_t bRequest, uint8_t control_selector, \
    uint8_t channel_number, uint8_t feature_unit_id, \
    uint8_t audio_interface, void* data, uint8_t len, \
    callback_function callback, uint8_t flags)

#define __ENDPOINT_REQUEST_PROTO__(proto_type) \
  void (*__request_endpoint_ ## proto_type)(struct AudioDriver* driver, UsbDev* dev, \
    Interface* interface, UsbDeviceRequest* device_req, uint8_t bRequest, uint8_t control_selector, \
    uint8_t endpoint, void* data, uint8_t len, callback_function callback, \
    uint8_t flags)

struct AudioDriver {
  struct UsbDriver super;
  struct AudioDev dev[MAX_DEVICES_PER_USB_DRIVER];
  uint8_t audio_map[MAX_DEVICES_PER_USB_DRIVER];

  void (*new_audio_driver)(struct AudioDriver *driver, char *name,
                           UsbDevice_ID *entry);
  struct AudioDev *(*get_free_audio_dev)(struct AudioDriver *driver);
  void (*free_audio_dev)(struct AudioDriver *driver,
                         struct AudioDev *audio_dev);
  struct AudioDev *(*match_audio_dev)(struct AudioDriver *driver, UsbDev *dev);
  int (*configure_audio_device)(struct AudioDriver *driver);
  int8_t (*sync_streaming_interface)(struct AudioDriver *driver, UsbDev *dev,
                                     Interface *interface);
  void (*audio_device_routine)(struct AudioDriver *driver, uint8_t *start,
                               uint8_t *end, MemoryService_C *mem_service,
                               uint8_t minor);
  int8_t (*set_sound_value)(struct AudioDriver* driver, int16_t value, struct AudioDev* audio_dev,
    Interface* as_interface);
  int8_t (*set_max_sound_value)(struct AudioDriver* driver, int16_t value, 
    struct AudioDev* audio_dev, Interface* as_interface);
  int8_t (*set_min_sound_value)(struct AudioDriver* driver, int16_t value,
    struct AudioDev* audio_dev, Interface* as_interface);
  int8_t (*get_max_volume)(struct AudioDriver* driver, struct AudioDev* audio_dev, 
    Interface* as_interface);
  int8_t (*get_min_volume)(struct AudioDriver* driver,
    struct AudioDev* audio_dev, Interface* as_interface);
  int8_t (*get_current_volume)(struct AudioDriver* driver,
    struct AudioDev* audio_dev, Interface* as_interface);
  int8_t (*mute)(struct AudioDriver* driver, struct AudioDev* audio_dev, Interface* interface);
  int8_t (*unmute)(struct AudioDriver* driver, struct AudioDev* audio_dev, Interface* interface);
  int8_t (*get_mute_attribute)(struct AudioDriver* driver, struct AudioDev* audio_dev, Interface* interface);
  int8_t (*set_sampling_frequency)(struct AudioDriver* driver, struct AudioDev* audio_dev,
    Interface* as_interface, struct SampleFrequency sample_freq);
  void (*request_device_attributes)(struct AudioDriver* driver, struct AudioDev* audio_dev,
    Interface* itf);
  void (*switch_if_zero_bandwidth)(struct AudioDriver* driver, struct AudioDev* audio_dev);//,
    //Interface* itf);
  void (*configure_interfaces)(struct AudioDriver* driver, struct AudioDev* audio_dev);
  void* (*get_top_layer_buffer)(struct AudioDriver* driver, Interface* interface);
  Endpoint* (*get_iso_endpoint)(struct AudioDriver* driver, struct AudioDev* audio_dev,
    Interface* as_interface);
  int8_t (*get_sampling_frequency)(struct AudioDriver* driver,
    struct AudioDev* audio_dev, Interface* as_interface);
  uint8_t (*get_upstream_channels_by_source_id)(struct AudioDriver* driver, 
    uint8_t bSourceID, ACInterface* ac_interface);
  uint8_t (*get_upstream_channels_by_source_id_depth)(struct AudioDriver* driver,
    uint8_t bSourceID, ACInterface* ac_interface, uint8_t depth);
  void* (*find_structure_by_id)(struct AudioDriver* driver, ACInterface* ac_interface,
    uint8_t bSourceID, enum ACInterfaceDescriptorSubtypes* subtype);
  void* (*get_upstream_structure)(struct AudioDriver* driver, ACInterface* ac_interface, 
    enum ACInterfaceDescriptorSubtypes* subtype, void* unit, uint8_t bSource_num);
  int (*mixer_build_routine)(struct AudioDriver* driver, uint8_t* start,
    MemoryService_C* m, ACInterface* ac_interface);
  int8_t (*is_bm_control_programmable)(struct AudioDriver* driver, 
    struct MixerUnit* mixer_unit, uint8_t input_channel, 
    uint8_t output_channel);
  uint8_t (*mixer_bmControlLength)(struct AudioDriver* driver,
    uint8_t input_channels, uint8_t output_channels);
  int (*selector_build_routine)(struct AudioDriver* driver, uint8_t* start,
    ACInterface* ac_interface, MemoryService_C* m);
  int (*feature_build_routine)(struct AudioDriver* driver, uint8_t* start,
    ACInterface* ac_interface, MemoryService_C* m);
  int (*processing_unit_build_routine)(struct AudioDriver* driver, uint8_t* start,
    ACInterface* ac_interface, MemoryService_C* m);
  int (*extension_unit_build_routine)(struct AudioDriver* driver, uint8_t* start,
    ACInterface* ac_interface, MemoryService_C* m);
  int (*associated_interface_build_routine)(struct AudioDriver* driver, uint8_t* start,
    ACInterface* ac_interface, MemoryService_C* m);
  int (*handle_ac_routine)(struct AudioDriver* driver, uint8_t** start, 
    MemoryService_C* m, uint8_t minor);
  int (*handle_as_routine)(struct AudioDriver* driver, uint8_t** start, 
    uint8_t* end, MemoryService_C* m, uint8_t minor);
  int (*handle_as_endpoint_routine)(struct AudioDriver* driver, MemoryService_C* m, uint8_t* start,
    Alternate_Interface* alt_interface, uint8_t endpoint_num);
  enum SpatialLocations (*get_loc_by_channel_number)(struct AudioDriver* driver, 
    struct ClusterDescriptor cluster, uint8_t channel_number);
  int16_t (*get_channel_number_by_loc)(struct AudioDriver* driver, struct ClusterDescriptor cluster,
    enum SpatialLocations loc);
  enum ProcessingUnitProcessTypes (*retrieve_process_type)(struct AudioDriver* driver,
    uint8_t* pos);
  int8_t (*audio_mixer_control)(struct AudioDriver* driver, struct AudioDev* audio_dev,
    Interface* interface, uint8_t request_form, uint8_t bRequest, 
    uint8_t icn, uint8_t ocn, int16_t wMixer);
  int8_t (*audio_volume_control)(struct AudioDriver* driver, struct AudioDev* audio_dev,
    Interface* interface, uint8_t request_form, uint8_t bRequest, 
    uint8_t channel_number, callback_function callback, uint8_t flags, int16_t wVolume);
  int8_t (*audio_mute_control)(struct AudioDriver* driver, struct AudioDev* audio_dev,
    Interface* interface, uint8_t request_form, uint8_t bRequest, 
    uint8_t channel_number, callback_function callback, uint8_t flags, uint8_t bMute);
  void* (*recover_unit_from_as_interface)(struct AudioDriver* driver,
    struct AudioDev* audio_dev, Interface* interface, enum ACInterfaceDescriptorSubtypes search,
    enum ProcessingUnitProcessTypes process_sub_type);
  int8_t (*feature_control)(struct AudioDriver* driver, uint8_t control, 
    uint8_t request_form, struct AudioDev* audio_dev, Interface* interface, 
    uint8_t bRequest, uint8_t channel_number, void* data, 
    uint8_t len, callback_function callback, uint8_t flags);
  int8_t (*set_mixer_unit_control)(struct AudioDriver* driver, struct AudioDev* audio_dev,
    Interface* interface, uint8_t icn, uint8_t ocn, int16_t wMixer);
  int8_t (*get_mixer_unit_control)(struct AudioDriver* driver, struct AudioDev* audio_dev,
    Interface* interface, uint8_t icn, uint8_t ocn);
  int8_t (*audio_selector_control)(struct AudioDriver* driver, struct AudioDev* audio_dev,
    Interface* interface, uint8_t request_form, uint8_t bRequest, uint8_t bSelector);
  int8_t (*selector_set_cur)(struct AudioDriver* driver, struct AudioDev* audio_dev, 
    Interface* interface, uint8_t bSelector);
  int8_t (*audio_processing_control)(struct AudioDriver* driver, struct AudioDev* audio_dev,
    Interface* interface, uint8_t unit_id, uint8_t request_form, uint8_t bRequest, 
    uint8_t control, void* data, uint8_t len, callback_function callback,
    uint8_t flags);
  int8_t (*audio_extension_control)(struct AudioDriver* driver, struct AudioDev* audio_dev,
    Interface* interface, uint8_t request_form, uint8_t bRequest, uint8_t bOn);
  int8_t (*audio_enable_processing_control)(struct AudioDriver* driver, 
    struct AudioDev* audio_dev, Interface* interface, uint8_t request_form, 
    uint8_t bRequest, uint8_t bEnable);
  int8_t (*support_pitch_control)(struct AudioDriver* driver, struct AudioDev* audio_dev, 
    Interface* as_interface);
  int8_t (*audio_endpoint_controls)(struct AudioDriver* driver, struct AudioDev* audio_dev,
    uint8_t request_form, uint8_t bRequest, uint8_t control_selector, 
    Interface* as_interface, void* data, uint8_t len, 
    callback_function callback, uint8_t flags);
  int8_t (*sampling_frequency_control)(struct AudioDriver* driver, 
    struct AudioDev* audio_dev, uint8_t request_form, uint8_t bRequest, 
    Interface* as_interface, struct SampleFrequency sample_freq,
    uint8_t flags);
  int8_t (*sampling_pitch_control)(struct AudioDriver* driver, struct AudioDev* audio_dev,
    uint8_t request_form, uint8_t bRequest, Interface* as_interface, 
    uint8_t bPitchEnable);
  int8_t (*enable_processing_control)(struct AudioDriver* driver, struct AudioDev* audio_dev,
    Interface* interface);
  int8_t (*disable_processing_control)(struct AudioDriver* driver, struct AudioDev* audio_dev,
    Interface* interface);
  int8_t (*audio_mode_select_control)(struct AudioDriver* driver, struct AudioDev* audio_dev,
    Interface* interface, uint8_t request_form, uint8_t bRequest, uint8_t bMode);
  int8_t (*audio_spaciousness_control)(struct AudioDriver* driver, struct AudioDev* audio_dev,
    Interface* interface, uint8_t request_form, uint8_t bRequest, uint8_t bSpaciousness);
  int8_t (*audio_reverb_type_control)(struct AudioDriver* driver, struct AudioDev* audio_dev,
    Interface* interface, uint8_t request_form, uint8_t bRequest, enum ReverbTypes type);
  int8_t (*audio_reverb_level_control)(struct AudioDriver* driver, struct AudioDev* audio_dev,
    Interface* interface, uint8_t request_form, uint8_t bRequest, uint8_t bReverbLevel);
  int8_t (*audio_chorus_level_control)(struct AudioDriver* driver, struct AudioDev* audio_dev,
    Interface* interface, uint8_t request_form, uint8_t bRequest, uint8_t bChorusLevel);
  int8_t (*audio_set_cur_spaciousness_control)(struct AudioDriver* driver, struct AudioDev* audio_dev,
    Interface* interface, uint8_t bSpaciousness);
  int8_t (*set_audio_mode_attribute)(struct AudioDriver* driver, struct AudioDev* audio_dev,
    Interface* interface, uint8_t bMode);
  int8_t (*enable_extension_unit)(struct AudioDriver* driver, struct AudioDev* audio_dev,
    Interface* interface);
  int8_t (*disable_extension_unit)(struct AudioDriver* driver, struct AudioDev* audio_dev,
    Interface* interface);
  int8_t (*audio_copy_protect_control)(struct AudioDriver* driver, struct AudioDev* audio_dev,
    Interface* interface, uint8_t request_form, uint8_t bRequest, enum CPL cpl);
  int8_t (*set_copy_protect)(struct AudioDriver* driver, struct AudioDev* audio_dev,
    Interface* interface, enum CPL cpl);
  int8_t (*get_copy_protect)(struct AudioDriver* driver, struct AudioDev* audio_dev,
    Interface* interface, enum CPL cpl);
  int8_t (*set_volume_resolution)(struct AudioDriver* driver, struct AudioDev* audio_dev,
    int16_t resolution_value, Interface* as_interface);
  void* (*back_propagate_input)(struct AudioDriver* driver, ACInterface* ac_interface,
    struct InputTerminal* input_terminal, enum ACInterfaceDescriptorSubtypes start,
    enum ProcessingUnitProcessTypes subtype);
  void* (*back_propagate_output)(struct AudioDriver* driver, ACInterface* ac_interface,
    struct OutputTerminal* output_terminal, enum ACInterfaceDescriptorSubtypes search);
  void* (*back_propagate_output_processing)(struct AudioDriver* driver, ACInterface* ac_interface,
    struct OutputTerminal* output_terminal, enum ProcessingUnitProcessTypes search);

  uint8_t (*__get_terminal)(struct AudioDriver *driver,
                            struct AudioDev *audio_dev,
                            ASInterface *as_interface);
  uint32_t (*__get_sample_frequency)(struct AudioDriver *driver,
                                     ASInterface *as_interface);
  uint32_t (*__get_sub_frame_size)(struct AudioDriver *driver,
                                   ASInterface *as_interface);
  uint32_t (*__get_total_supported_frequencies)(struct AudioDriver *driver,
                                                ASInterface *as_interface);
  uint16_t (*__get_1ms_size)(struct AudioDriver *driver,
                             ASInterface *as_interface);
  uint32_t (*__get_bit_depth)(struct AudioDriver *driver,
                              ASInterface *as_interface);
  uint32_t (*__get_frame_size)(struct AudioDriver *driver,
                               ASInterface *as_interface);
  uint32_t (*__get_num_channels)(struct AudioDriver *driver,
                                 ASInterface *as_interface);
  int8_t (*__is_freq_set)(struct AudioDriver *driver,
                          ASInterface *as_interface);
  ASInterface *(*__convert_to_class_specific_as_interface)(
      struct AudioDriver *driver, Interface *interface);
  ACInterface *(*__convert_to_class_specific_ac_interface)(
      struct AudioDriver *driver, Interface *interface);
  ASEndpoint *(*__convert_to_class_specific_as_endpoint)(
      struct AudioDriver *driver, Endpoint *endpoint);
  struct AudioDev *(*__get_audio_dev)(struct AudioDriver *driver,
                                      uint8_t minor);
  int8_t (*__match_terminal_type)(struct AudioDriver *driver,
                                  ASInterface *as_interface,
                                  uint8_t terminal_type);
  int8_t (*__is_as_output_terminal)(struct AudioDriver *driver,
                                    ASInterface *as_interface);
  int8_t (*__is_as_input_terminal)(struct AudioDriver *driver,
                                   ASInterface *as_interface);
  Interface *(*__get_as_interface_by_terminal)(struct AudioDriver *driver,
                                                 struct AudioDev *audio_dev,
                                                 uint8_t terminal_type);
  uint16_t (*__get_sync_delay)(struct AudioDriver *driver, Endpoint *endpoint);
  enum LockDelayUnit (*__get_sync_unit)(struct AudioDriver *driver,
                                        Endpoint *endpoint);
  uint32_t (*__get_volume)(struct AudioDriver* audio_driver, struct AudioDev* audio_dev);
  uint32_t (*__get_mute_attribute)(struct AudioDriver* audio_driver, struct AudioDev* audio_dev);
  uint32_t (*__get_max_volume)(struct AudioDriver* audio_driver, struct AudioDev* audio_dev);
  uint32_t (*__get_min_volume)(struct AudioDriver* audio_driver, struct AudioDev* audio_dev);
  int8_t (*__should_transfer_data)(struct AudioDriver* driver,
    ASInterface* as_interface);
  int8_t (*__has_zero_bandwidth_setting)(struct AudioDriver* driver,
    UsbDev* dev, Interface* as_interface);
  uint8_t (*__match_frequency)(struct AudioDriver* driver, SampleFrequency* as_freq,
    SampleFrequency* target);
  void (*__clear_low_level_buffers)(struct AudioDriver* driver, 
    struct AudioDev* audio_dev);
  int8_t (*__is_zero_bandwidth_active)(struct AudioDriver* driver,
    UsbDev* dev, Interface* as_interface);
  ASInterface* (*__get_class_specific_as_interface)(
    struct AudioDriver* driver, UsbDev* dev, Interface* interface);
  uint8_t (*__is_input_terminal)(struct AudioDriver *driver, uint8_t *start);
  uint8_t (*__is_output_terminal)(struct AudioDriver* driver, uint8_t* start);
  int8_t (*__is_zero_frequency)(struct AudioDriver *driver, SampleFrequency sample_freq);
  uint8_t (*__is_cs_interface)(struct AudioDriver *driver, uint8_t *start);
  uint8_t (*__is_as_endpoint)(struct AudioDriver *driver, uint8_t *start);
  uint8_t (*__is_ac_interface)(struct AudioDriver *driver, uint8_t *start);
  uint8_t (*__is_as_interface)(struct AudioDriver *driver, uint8_t *start);
  uint8_t (*__is_format_type)(struct AudioDriver *driver, uint8_t *start);
  uint8_t (*__is_mixer)(struct AudioDriver *driver, uint8_t *start);
  uint8_t (*__is_selector)(struct AudioDriver *driver, uint8_t *start);
  uint8_t (*__is_feature)(struct AudioDriver *driver, uint8_t *start);
  uint8_t (*__is_processing_unit)(struct AudioDriver *driver, uint8_t *start);
  uint8_t (*__is_extension_unit)(struct AudioDriver *driver, uint8_t *start);
  int8_t (*__is_channel_in_cluster)(struct AudioDriver *driver, struct ClusterDescriptor cluster, 
    enum SpatialLocations loc);
  void (*__set_active_and_passive_buffers)(struct AudioDriver *driver, ASInterface *as_interface, 
    uint8_t **active_buffer, uint8_t **passive_buffer, 
    uint8_t *as_interface_active_buffer, uint8_t *as_interface_passive_buffer);
  void (*__set_default_audio_properties)(struct AudioDriver *driver, struct AudioDev *audio_dev, UsbDev *dev);
  void (*__parse_common_unit_descriptor_part)(struct AudioDriver* driver, uint8_t* start,
    uint8_t* len_member, uint8_t* type_member, uint8_t* subtype_member,
    uint8_t* unit_id);
  __BASIC_REQUEST_PROTO__(set);
  __BASIC_REQUEST_PROTO__(get);
  __COMMON_REQUEST_PROTO__(set);
  __COMMON_REQUEST_PROTO__(get);
  __MIXER_REQUEST_PROTO__(set);
  __MIXER_REQUEST_PROTO__(get);
  __FEATURE_REQUEST_PROTO__(set);
  __FEATURE_REQUEST_PROTO__(get);
  __ENDPOINT_REQUEST_PROTO__(get);
  __ENDPOINT_REQUEST_PROTO__(set);
  uint8_t* (*__parse_bmControls)(struct AudioDriver* driver, uint8_t* pos,
    uint8_t bm_control_number, uint8_t* control_size, MemoryService_C* m);
  void (*__parse_bmaControls)(struct AudioDriver* driver, uint8_t*** member,
    uint8_t* control_size, MemoryService_C* m, uint8_t available_controls,
    uint8_t* start);
  int8_t (*__valid_request_form)(struct AudioDriver* driver, uint8_t request_form);
  uint8_t (*__get_total_channels_in_cluster)(struct AudioDriver* driver, 
    struct ClusterDescriptor cluster);
  void* (*__search_for_unit_by_id)(struct AudioDriver* driver, ACInterface* ac_interface, 
    uint8_t id, enum ACInterfaceDescriptorSubtypes* unit_type);
  uint8_t (*__get_unit_id)(struct AudioDriver* driver, void* unit);
  uint8_t (*__get_unit_sub_type)(struct AudioDriver* driver, void* unit);
  uint16_t (*__get_terminal_type)(struct AudioDriver* driver, void* terminal);
  uint8_t* (*__get_feature_bmControl_of_channel)(struct AudioDriver* driver, 
    uint8_t channel_number, struct FeatureUnit* feature_unit);
  int8_t (*__bmControl_supported)(struct AudioDriver* driver, uint8_t* bmControl,
    enum FeatureUnitbmaControls control_bit);
  int8_t (*__bmControl_mute_supported)(struct AudioDriver* driver, uint8_t* bmControl);
  int8_t (*__bmControl_volume_supported)(struct AudioDriver* driver, uint8_t* bmControl);
  int8_t (*__bmControl_bass_supported)(struct AudioDriver* driver, uint8_t* bmControl);
  int8_t (*__bmControl_mid_supported)(struct AudioDriver* driver, uint8_t* bmControl);
  int8_t (*__bmControl_graphic_equalizer_supported)(struct AudioDriver* driver, 
    uint8_t* bmControl);
  int8_t (*__bmControl_delay_supported)(struct AudioDriver* driver, uint8_t* bmControl);
  void (*__parse_waModes)(struct AudioDriver* driver, uint8_t* pos,
    uint8_t* number_modes, uint16_t* waModes);
  void (*__assign_process)(struct AudioDriver* driver, void* process,
    struct ProcessingUnitDescriptor* processing_unit_descriptor);
  int8_t (*__input_terminal_present)(struct AudioDriver* driver, 
    ACInterface* ac_interface, uint8_t input_terminal_id);
  int8_t (*__output_terminal_present)(struct AudioDriver* driver, ACInterface* ac_interface,
    uint8_t output_terminal_id);
  int8_t (*__feature_unit_present)(struct AudioDriver* driver, ACInterface* ac_interface, 
    uint8_t feature_unit_id);
  int8_t (*__processing_unit_present)(struct AudioDriver* driver, ACInterface* ac_interface,
    uint8_t processing_unit_id);
  int8_t (*__extension_unit_present)(struct AudioDriver* driver, ACInterface* ac_interface,
    uint8_t extension_unit_id);
  int8_t (*__selector_unit_present)(struct AudioDriver* driver, ACInterface* ac_interface,
    uint8_t selector_unit_id);
  int8_t (*__mixer_unit_present)(struct AudioDriver* driver, ACInterface* ac_interface,
    uint8_t mixer_unit_id);
  int8_t (*__audio_interface_present)(struct AudioDriver* driver, struct AudioDev* dev, 
    uint8_t audio_interface);
  int8_t (*__support_enable_processing_control)(struct AudioDriver* driver, struct ProcessingUnit* processing_unit);
  int8_t (*__common_control)(struct AudioDriver* driver, uint8_t bControlSize, uint8_t* bmControl, uint16_t control);
  int8_t (*__support_mode_select_control)(struct AudioDriver* driver, struct ProcessingUnit* processing_unit);
  int8_t (*__support_spaciousness_control)(struct AudioDriver* driver, struct ProcessingUnit* processing_unit);
  int8_t (*__reverb_control)(struct AudioDriver* driver, enum ProcessingUnitProcessTypes type, 
    uint8_t bControlSize, uint8_t* bmControl, enum ReverbProcessingUnitControl control);
  int8_t (*__support_reverb_type_control)(struct AudioDriver* driver, struct ProcessingUnit* processing_unit);
  int8_t (*__support_reverb_level_control)(struct AudioDriver* driver, struct ProcessingUnit* processing_unit);
  int8_t (*__support_reverb_time_control)(struct AudioDriver* driver, struct ProcessingUnit* processing_unit);
  int8_t (*__support_reverb_delay_feedback_control)(struct AudioDriver* driver, struct ProcessingUnit* processing_unit);
  int8_t (*__chorus_control)(struct AudioDriver* driver, enum ProcessingUnitProcessTypes type, uint8_t bControlSize,
    uint8_t* bmControl, enum ChorusProcessingUnitControl control);
  int8_t (*__support_chorus_level_control)(struct AudioDriver* driver, struct ProcessingUnit* processing_unit);
  int8_t (*__support_chorus_modulation_rate_control)(struct AudioDriver* driver, struct ProcessingUnit* processing_unit);
  int8_t (*__support_chorus_modulation_depth_control)(struct AudioDriver* driver, struct ProcessingUnit* processing_unit);
  int8_t (*__dynamic_control)(struct AudioDriver* driver, enum ProcessingUnitProcessTypes type, uint8_t bControlSize,
    uint8_t* bmControl, enum DynamicRangeUnitControl control);
  int8_t (*__support_compression_ratio_control)(struct AudioDriver* driver, struct ProcessingUnit* processing_unit);
  int8_t (*__support_max_amplitude_control)(struct AudioDriver* driver, struct ProcessingUnit* processing_unit);
  int8_t (*__support_threshold_control)(struct AudioDriver* driver, struct ProcessingUnit* processing_unit);
  int8_t (*__support_attack_time_control)(struct AudioDriver* driver, struct ProcessingUnit* processing_unit);
  int8_t (*__support_release_time_control)(struct AudioDriver* driver, struct ProcessingUnit* processing_unit);
};

void new_audio_driver(struct AudioDriver *driver, char *name,
                      UsbDevice_ID *entry);

struct ASAudioEndpoint { // since the default endpoint is an extended enpoint
                         // this structure is required and has to be stored in
                         // the corresponding audio component
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

enum TerminalTypes {
  USB_UNDEFINED = 0x0100,
  USB_STREAMING = 0x0101,
  USB_VENDOR_SPECIFIC = 0x01FF
};

enum InputTerminalTypes {
  INPUT_UNDEFINED = 0x0200,
  MICROPHONE = 0x0201,
  DESKTOP_MICROPHONE = 0x0202,
  PERSONAL_MICROPHONE = 0x0203,
  OMNI_DIRECTIONAL_MICROPHONE = 0x0204
};

enum OutputTerminalTypes {
  OUTPUT_UNDEFINED = 0x0300,
  SPEAKER = 0x0301,
  HEADPHONES = 0x0302,
  HEAD_MOUNTED_DISPLAY_AUDIO = 0x0303,
  DESKTOP_SPEAKER = 0x0304,
  ROOM_SPEAKER = 0x0305,
  COMMUNICATION_SPEAKER = 0x0306,
  LOW_FREQ_EFFECTS_SPEAKER = 0x0307
};

enum BiDirectionalTerminalTypes {
  BI_DIRECTIONAL_UNDEFINED = 0x0400,
  HANDSET = 0x0401,
  HEADSET = 0x0402,
  SPEAKER_PHONE = 0x0403,
  ECHO_SUPRESSING_SPEAKER_PHONE = 0x0404,
  ECHO_CANCELING_SPEAKER_PHONE = 0x0405
};

enum TerminalControlSelectors {
  TE_CONTROL_UNDEFINED = 0x00,
  COPY_PROTECT_CONTROL = 0x01
};

enum IsoEndpointControls {
  BIT_SAMPLING_FREQ = 0x01,
  BIT_PITCH = 0x02,
  BIT_MAX_PACKETS_ONLY = 0x80
};

enum FeatureUnitControlSelectors {
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

enum UP_DOWN_MixProcessingUnitControlSelectors {
  UD_CONTROL_UNDEFINED = 0x00,
  UD_ENABLE_CONTROL = 0x01,
  UD_MODE_SELECT_CONTROL = 0x02
};

enum DolbyPrologicProcessingUnitControlSelectors {
  DP_CONTROL_UNDEFINED = 0x00,
  DP_ENABLE_CONTROL = 0x01,
  DP_MODE_SELECT_CONTROL = 0x02
};

enum StereoExtenderProcessingUnitControlSelectors {
  STEREO_CONTROL_UNDEFINED = 0x00,
  STEREO_ENABLE_CONTROL = 0x01,
  STEREO_SPACIOUSNESS_CONTROL = 0x03
};

enum ReverberationProcessingUnitControlSelectors {
  RV_CONTROL_UNDEFINED = 0x00,
  RV_ENABLE_CONTROL = 0x01,
  REVERB_LEVEL_CONTROL = 0x02,
  REVERB_TIME_CONTROL = 0x03,
  REVERB_FEEDBACK_CONTROL = 0x04
};

enum ChorusProcessingUnitControlSelectors {
  CH_CONTROL_UNDEFINED = 0x00,
  CH_ENABLE_CONTROL = 0x01,
  CHORUS_LEVEL_CONTROL = 0x02,
  CHORUS_RATE_CONTROL = 0x03,
  CHORUS_DEPTH_CONTROL = 0x04
};

enum DynamicRangeCompressorProcessingUnitControlSelectors {
  DR_CONTROL_UNDEFINED = 0x00,
  DR_ENABLE_CONTROL = 0x01,
  COMPRESSION_RATE_CONTROL = 0x02,
  MAXAMPL_CONTROL = 0x03,
  THRESHOLD_CONTROL = 0x04,
  ATTACK_TIME_CONTROL = 0x05,
  RELEASE_TIME_CONTROL = 0x06
};

enum ExtensionUnitControlSelectors {
  XU_CONTROL_UNDEFINED = 0x00,
  XU_ENABLE_CONTROL = 0x01
};

enum EndpoinControlSelectors {
  EP_CONTROL_UNDEFINED = 0x00,
  SAMPLING_FREQ_CONTROL = 0x01,
  PITCH_CONTROL = 0x02
};

// bit level in bmControl inside specific units

enum ProcessingUnitControl{
  ENABLE_PROCESSING = 0x01
};

enum ExtProcessingUnitControl{
  MODE_SELECT = 0x02
};

enum StereoProcessingUnitControl{
  SPACIOUSNESS = 0x02
};

enum AudioRequestCodes {
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

struct InputTerminalDescriptor {
  uint8_t bLength;
  uint8_t bDescriptorType;
  uint8_t bDescriptorSubtype;
  uint8_t bTerminalID;
  uint16_t wTerminalType;
  uint8_t bAssocTerminal;
  ClusterDescriptor cluster;
  uint8_t iTerminal;
} __attribute__((packed));

struct InputTerminal {
  struct InputTerminalDescriptor in_terminal_desc;
  char *in_terminal_description;
};

struct OutputTerminalDescriptor {
  uint8_t bLength;
  uint8_t bDescriptorType;
  uint8_t bDescriptorSubtype;
  uint8_t bTerminalID;
  uint16_t wTerminalType;
  uint8_t bAssocTerminal;
  uint8_t bSourceID;
  uint8_t iTerminal;
} __attribute__((packed));

struct OutputTerminal {
  struct OutputTerminalDescriptor out_terminal_desc;
  char *out_terminal_description;
};

struct MixerUnitDescriptor {
  uint8_t bLength;
  uint8_t bDescriptorType;
  uint8_t bDescriptorSubtype;
  uint8_t bUnitID;
  uint8_t bNrInPins;
  uint8_t *baSourceID;
  ClusterDescriptor cluster;
  uint8_t *bmControls;
  uint8_t iMixer;
};

struct MixerUnit {
  struct MixerUnitDescriptor mixer_unit_desc;
  char *mixer_description;
  uint8_t bmControlsLength;
};

struct SelectorUnitDescriptor {
  uint8_t bLength;
  uint8_t bDescriptorType;
  uint8_t bDescriptorSubtype;
  uint8_t bUnitID;
  uint8_t bNrInPins;
  uint8_t *baSourceID;
  uint8_t iSelector;
};

struct SelectorUnit {
  struct SelectorUnitDescriptor selector_unit_desc;
  char *selector_description;
};

struct FeatureUnitDescriptor {
  uint8_t bLength;
  uint8_t bDescriptorType;
  uint8_t bDescriptorSubtype;
  uint8_t bUnitID;
  uint8_t bSourceID;
  uint8_t bControlSize;
  uint8_t **bmaControls; // get channels from upstream unit
  uint8_t iFeature;
};

struct FeatureUnit {
  struct FeatureUnitDescriptor feature_unit_desc;
  char *feature_description;
};

struct ProcessingUnit {
  struct ProcessingUnitDescriptor processing_unit_desc;
  char *processing_description;
  uint8_t ext;
};

struct DolbyPrologicProcessingUnitDescriptor {
  uint8_t bLength;
  uint8_t bDescriptorType;
  uint8_t bDescriptorSubtype;
  uint8_t bUnitID;
  uint16_t wProcessType;
  uint8_t bNrInPins;
  uint8_t bSourceID;
  ClusterDescriptor cluster;
  uint8_t bControlSize;
  uint8_t *bmControls;
  uint8_t iProcessing;
  uint8_t bNrModes;
  uint16_t *waModes;
};

struct THREE_D_StereoExtenderProcessingUnitDescriptor {
  uint8_t bLength;
  uint8_t bDescriptorType;
  uint8_t bDescriptorSubtype;
  uint8_t bUnitID;
  uint16_t wProcessType;
  uint8_t bNrInPins;
  uint8_t bSourceID;
  ClusterDescriptor cluster;
  uint8_t bControlSize;
  uint8_t *bmControls;
  uint8_t iProcessing;
};
struct ReverberationProcessingUnitDescriptor {
  uint8_t bLength;
  uint8_t bDescriptorType;
  uint8_t bDescriptorSubtype;
  uint8_t bUnitID;
  uint16_t wProcessType;
  uint8_t bNrInPins;
  uint8_t bSourceID;
  ClusterDescriptor cluster;
  uint8_t bControlSize;
  uint8_t *bmControls;
  uint8_t iProcessing;
};

struct ChorusProcessingUnitDescriptor {
  uint8_t bLength;
  uint8_t bDescriptorType;
  uint8_t bDescriptorSubtype;
  uint8_t bUnitID;
  uint16_t wProcessType;
  uint8_t bNrInPins;
  uint8_t bSourceID;
  ClusterDescriptor cluster;
  uint8_t bControlSize;
  uint8_t *bmControls;
  uint8_t iProcessing;
};

struct DynamicRangeCompressorProcessnigUnitDescriptor {
  uint8_t bLength;
  uint8_t bDescriptorType;
  uint8_t bDescriptorSubtype;
  uint8_t bUnitID;
  uint16_t wProcessType;
  uint8_t bNrInPins;
  uint8_t bSourceID;
  ClusterDescriptor cluster;
  uint8_t bControlSize;
  uint8_t *bmControls;
  uint8_t iProcessing;
};

struct ExtensionUnitDescriptor {
  uint8_t bLength;
  uint8_t bDescriptorType;
  uint8_t bDescriptorSubtype;
  uint8_t bUnitID;
  uint16_t wExtensionCode;
  uint8_t bNrInPins;
  uint8_t *baSourceID;
  ClusterDescriptor cluster;
  uint8_t bControlSize;
  uint8_t *bmControls;
  uint8_t iExtension;
};

struct ExtensionUnit {
  struct ExtensionUnitDescriptor extension_unit_desc;
  char *extension_description;
};

struct AssociatedInterfaceDescriptor {
  uint8_t bLength;
  uint8_t bDescriptorType;
  uint8_t bDescriptorSubtype;
  uint8_t bInterfaceNr;
  void *association_specific;
};

struct CopyProtectControlParameterBlock {
  uint8_t bCopyProtect;
} __attribute__((packed));

// channel request
struct MuteControlParameterBlockFirstForm {
  uint8_t bMute;
} __attribute__((packed));

// 0xFF channel request
struct MuteControlParameterBlockSecondForm {
  uint8_t bMute[1];
} __attribute__((packed));

struct VolumeControlParameterBlockFirstForm {
  int16_t wVolume;
} __attribute__((packed));

struct VolumeControlParameterBlockSecondForm {
  int16_t wVolume[1];
} __attribute__((packed));

struct MixerControlParameterBlockFirstForm {
  int16_t wMixer;
} __attribute__((packed));

struct MixerControlParameterBlockSecondForm {
  int16_t wMixer[1];
} __attribute__((packed));

struct SelectorControlParameterBlock {
  uint8_t bSelector;
} __attribute__((packed));

// just implement the most common controls !

struct EnableProcessingControlParameterBlock {
  uint8_t bEnable;
} __attribute__((packed));

struct ModeSelectControlParameterBlock {
  uint8_t bMode;
} __attribute__((packed));

struct SpaciousnessControlParameterBlock {
  uint8_t bSpaciousness;
} __attribute__((packed));

struct ReverbTypeControlParameterBlock {
  uint8_t bReverbType;
} __attribute__((packed));

struct ReverbLevelControlParameterBlock {
  uint8_t bReverbLevel;
} __attribute__((packed));

struct ReverbTimeControlParameterBlock {};

struct ChorusLevelControlParameterBlock {
  uint8_t bChorusLevel;
} __attribute__((packed));

struct ChorusModulationRateControlParamterBlock {};

struct ChorusModulationDepthControlParameterBlock {};

struct DynamicRangeCompressorRatioControlParameterBlock {};

struct DynamicRangeCompressorMaxAmplControlParameterBlock {};

struct SamplingFrequencyControlParameterBlock {
  struct SampleFrequency tSampleFreq;
} __attribute__((packed));

struct PitchControlParameterBlock {
  uint8_t bPitchEnable;
} __attribute__((packed));

enum AudioDataFormatType1_Codes {
  TYPE_1_UNDEFINED = 0x0000,
  PCM = 0x0001,
  PCM_8 = 0x0002,
  IEEE_FLOAT = 0x0003,
  ALAW = 0x0004,
  MULAW = 0x0005
};

enum AudioDataFormatType2_Codes {
  TYPE_2_UNDEFINED = 0x1000,
  MPEG = 0x1001,
  AC_3 = 0x1002
};

enum AudioDataFormatType3_Codes {
  TYPE_3_UNDEFINED = 0x2000,
  IEC1937_AC_3 = 0x2001,
  IEC1937_MPEG_1_LAYER1 = 0x2002,
  IEC1937_MPEG_1_LAYER2_OR_3 = 0x2003,
  IEC1937_MPEG_2_EXT = 0x2004,
  IEC1937_MPEG_2_LAYER1_LS = 0x2005,
  IEC1937_MPEG_2_LAYER2_3_LS = 0x2006
};

enum FormatTypeCodes {
  FORMAT_TYPE_UNDEFINED = 0x00,
  FORMAT_TYPE_1 = 0x01,
  FORMAT_TYPE_2 = 0x02,
  FORMAT_TYPE_3 = 0x03
};

struct FormatType {
  void *type_descriptor;
  enum FormatTypeCodes (*get_format_type)(void *type_descriptor);
  uint8_t (*get_total_channels)(void *type_descriptor);
  uint8_t (*get_subframe_size)(void *type_descriptor);
  uint8_t (*get_bit_depth)(void *type_descriptor);
  uint8_t (*get_sam_freq_type)(void *type_descriptor);
  SampleFrequency (*get_sam_frequency)(void *type_descriptor, uint8_t freq_num);
};

struct FormatSpecificType {};

struct Type1_FormatTypeDescriptor {
  uint8_t bLength;
  uint8_t bDescriptorType;
  uint8_t bDescriptorSubtype;
  uint8_t bFormatType;
  uint8_t bNrChannels;
  uint8_t bSubframeSize;
  uint8_t bBitResolution;
  uint8_t bSamFreqType;
  void *samFreq;
} __attribute__((packed));

struct ContinuousSamplingFrequency {
  struct SampleFrequency tLower;
  struct SampleFrequency tUpper;
} __attribute__((packed));

struct DiscreteNumberSamplingFrequencies {
  struct SampleFrequency *tSam;
};

typedef struct AudioDev AudioDev;
typedef struct AudioDriver AudioDriver;
typedef struct AudioControlDescriptor AudioControlDescriptor;
typedef struct InputTerminalDescriptor InputTerminalDescriptor;
typedef struct OutputTerminalDescriptor OutputTerminalDescriptor;
typedef struct MixerUnitDescriptor MixerUnitDescriptor;
typedef struct SelectorUnitDescriptor SelectorUnitDescriptor;
typedef struct FeatureUnitDescriptor FeatureUnitDescriptor;
typedef struct MuteControlParameterBlockFirstForm
    MuteControlParameterBlockFirstForm;
typedef struct MuteControlParameterBlockSecondForm
    MuteControlParameterBlockSecondForm;
typedef struct VolumeControlParameterBlockFirstForm
    VolumeControlParameterBlockFirstForm;
typedef struct VolumeControlParameterBlockSecondForm
    VolumeControlParameterBlockSecondForm;
typedef struct CopyProtectControlParameterBlock
    CopyProtectControlParameterBlock;
typedef struct MixerControlParameterBlockFirstForm
    MixerControlParameterBlockFirstForm;
typedef struct MixerControlParameterBlockSecondForm
    MixerControlParameterBlockSecondForm;
typedef struct SelectorControlParameterBlock SelectorControlParameterBlock;
typedef enum CPL CPL;
typedef enum SpatialLocations SpatialLocations;
typedef struct ProcessingUnitDescriptor ProcessingUnitDescriptor;
typedef struct UP_DOWN_MixProcessingUnitDescriptor
    UP_DOWN_MixProcessingUnitDescriptor;
typedef struct DolbyPrologicProcessingUnitDescriptor
    DolbyPrologicProcessingUnitDescriptor;
typedef struct THREE_D_StereoExtenderProcessingUnitDescriptor
    THREE_D_StereoExtenderProcessingUnitDescriptor;
typedef struct ReverberationProcessingUnitDescriptor
    ReverberationProcessingUnitDescriptor;
typedef struct ChorusProcessingUnitDescriptor ChorusProcessingUnitDescriptor;
typedef struct DynamicRangeCompressorProcessnigUnitDescriptor
    DynamicRangeCompressorProcessnigUnitDescriptor;
typedef struct ExtensionUnitDescriptor ExtensionUnitDescriptor;
typedef struct AssociatedInterfaceDescriptor AssociatedInterfaceDescriptor;
typedef struct ASAudioEndpoint ASAudioEndpoint;
typedef enum LockDelayUnit LockDelayUnit;

#endif