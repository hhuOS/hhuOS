#include "stdint.h"
#include "UsbDevice.h"
#include "../controller/UsbControllerFlags.h"
#include "../controller/UsbController.h"
#include "data/UsbDev_Data.h"
#include "../include/UsbErrors.h"
#include "requests/UsbDescriptors.h"
#include "requests/UsbRequests.h"
#include "../driver/UsbDriver.h"
#include "../driver/hid/KeyBoardDriver.h"
#include "../driver/hid/MouseDriver.h"
#include "../events/listeners/EventListener.h"
#include "../events/listeners/hid/KeyBoardListener.h"
#include "../events/listeners/hid/MouseListener.h"
#include "../interfaces/SystemInterface.h"
#include "../utility/Utils.h"
#include "../include/UsbGeneral.h"
#include "../include/UsbControllerInclude.h"

uint8_t address = 0x01;

const uint8_t DEFAULT_STATE = 0x01;
const uint8_t ADDRESS_STATE = 0x02;
const uint8_t CONFIGURED_STATE = 0x03;

void new_usb_device(struct UsbDev *dev, uint8_t speed, uint8_t port,
                    SystemService_C *m, void *controller) {

  MemoryService_C *mem_service =
      (MemoryService_C *)container_of(m, MemoryService_C, super);

  uint8_t *map_io_buffer =
      (uint8_t *)mem_service->mapIO(mem_service, PAGE_SIZE, 1);
  int map_io_offset = 0;

  DeviceDescriptor *device_descriptor =
      (DeviceDescriptor *)(map_io_buffer + map_io_offset);
  map_io_offset += sizeof(DeviceDescriptor);

  ConfigurationDescriptor *config_descriptor =
      (ConfigurationDescriptor *)(map_io_buffer + map_io_offset);
  map_io_offset += sizeof(ConfigurationDescriptor);

  uint8_t *config_buffer = (map_io_buffer + map_io_offset);
  map_io_offset += (sizeof(uint8_t) * CONFIG_BUFFER_SIZE);

  uint8_t *string_buffer = (map_io_buffer + map_io_offset);
  map_io_offset += (sizeof(uint8_t) * STRING_BUFFER_SIZE);

  dev->device_request_map_io =
      (uint8_t *)mem_service->mapIO(mem_service, PAGE_SIZE, 1);

  dev->device_mutex = (Mutex_C*)mem_service->allocateKernelMemory_c(mem_service, sizeof(Mutex_C), 0);
  dev->device_mutex->new_mutex = &new_mutex;
  dev->device_mutex->new_mutex(dev->device_mutex);

  mem_set(dev->device_request_map_io, PAGE_SIZE, 0);
  mem_set(dev->device_request_map_io_bitmap, PAGE_SIZE / sizeof(UsbDeviceRequest), 0);

  dev->speed = speed;
  dev->port = port;
  dev->address = 0;
  dev->max_packet_size = 8; // default

  dev->active_config = 0;
  dev->supported_configs = 0;

  dev->controller = controller;
  dev->state = DEFAULT_STATE;

  dev->device_logger = 0;
  dev->mutex = 0;
  dev->error_while_transfering = 0;
  dev->mem_service = m;

  dev->lang_ids = (uint16_t *)mem_service->allocateKernelMemory_c(
      mem_service, SUPPORTED_LANGID_SIZE + 1, 0);

  dev->lang_id = 0;

  dev->manufacturer = "";
  dev->product = "";
  dev->serial_number = "";

  dev->l_e.l_e = 0;
  dev->l_e_driver.l_e = 0;

  dev->init_device_functions = &init_device_functions;
  dev->init_device_functions(dev);
  dev->init_device_logger(dev);

  dev->process_device_descriptor(dev, device_descriptor, 8);

  dev->max_packet_size = device_descriptor->bMaxPacketSize0;

  ((UsbController *)dev->controller)->reset_port((UsbController*)dev->controller, port);

  dev->set_address(dev, address);

  dev->address = address;
  address++;

  dev->state = ADDRESS_STATE;
  
  if(dev->handle_lang(dev, string_buffer) == -1){
    dev->device_logger->error_c(dev->device_logger, "Aborting configuration of device due to error while handling the langs ...");
    return;
  }

  if(dev->handle_dev(dev, string_buffer, device_descriptor) == -1){
    dev->device_logger->error_c(dev->device_logger, "Aborting configuration of device due to error while handling device spefic requests ...");
    return;
  }

  if(dev->handle_configuration(dev, string_buffer, config_buffer, config_descriptor, device_descriptor->bNumConfigurations) == -1){
    dev->device_logger->error_c(dev->device_logger, "Aborting configuration of device due to error while handling internal configuration of the device ...");
    return;
  } 

  dev->set_configuration(dev,
                         dev->active_config->config_desc.bConfigurationValue);

  dev->state = CONFIGURED_STATE;

  ((UsbController*)dev->controller)->add_device((UsbController *)dev->controller, dev);

  mem_service->unmap(mem_service, (uint32_t)(uintptr_t)map_io_buffer);
}

int handle_interface(UsbDev* dev, Configuration* configuration, uint8_t* string_buffer, uint8_t* start, uint8_t* end,
                      uint8_t num_interfaces){
  MemoryService_C* mem_service = (MemoryService_C*)container_of(dev->mem_service, MemoryService_C, super);
  Interface **interfaces = (Interface **)mem_service->allocateKernelMemory_c(
      mem_service, sizeof(Interface *) * num_interfaces, 0);
  configuration->interfaces = interfaces;

  uint8_t s_len;
  char* ascii_string;
  unsigned int interface_num = 0;
  unsigned int endpoint_num = 0;
  unsigned int prev_interface_number = -1;
  Alternate_Interface *prev;
  while (start < end) {
      if (*(start + 1) == INTERFACE) {
        Alternate_Interface *alt_interface =
            (Alternate_Interface *)mem_service->allocateKernelMemory_c(
                mem_service, sizeof(Alternate_Interface), 0);
        alt_interface->next = 0;

        InterfaceDescriptor *interface_desc = (InterfaceDescriptor *)start;

        if (interface_desc->iInterface != 0) {
          if(dev->process_string_descriptor(dev, string_buffer,
              interface_desc->iInterface, dev->lang_id, "iInterface", 1) == -1) return -1;
          
          s_len = string_buffer[0];

          if(dev->process_string_descriptor(
              dev, string_buffer, interface_desc->iInterface, dev->lang_id,
              "iInterface", s_len) == -1) return -1;

          ascii_string = dev->build_string(dev, s_len, string_buffer);
          interfaces[interface_num - 1]->interface_description = ascii_string;
        }

        alt_interface->alternate_interface_desc = *interface_desc;

        if (interface_desc->bInterfaceNumber == prev_interface_number) {
          prev->next = alt_interface;
          prev = prev->next;
        } else {
          Interface *interface =
              (Interface *)mem_service->allocateKernelMemory_c(
                  mem_service, sizeof(Interface), 0);
          interface->active = 0;
          interface->driver = 0;
          interface->active_interface = alt_interface;
          interface->alternate_interfaces = alt_interface;
          interface->interface_description = "";
          prev = alt_interface;
          interfaces[interface_num++] = interface;
        }
        endpoint_num = 0;
        prev_interface_number = interface_desc->bInterfaceNumber;
        alt_interface->endpoints =
            (Endpoint **)mem_service->allocateKernelMemory_c(
                mem_service,
                sizeof(struct Endpoint *) * interface_desc->bNumEndpoints, 0);
      }

      else if (*(start + 1) == HID) {
        // ReportDescriptor* report_desc = (ReportDescriptor*)start;
        //  for now we will just ignore custom input report and will only look
        //  at the default input report meaning if class code of 0 we can't
        //  support it right now
      }

      else if (*(start + 1) == ENDPOINT) {
        EndpointDescriptor *endpoint_desc = (EndpointDescriptor *)start;
        Endpoint *endpoint = (Endpoint *)mem_service->allocateKernelMemory_c(
            mem_service, sizeof(Endpoint), 0);
        endpoint->endpoint_desc = *endpoint_desc;
        prev->endpoints[endpoint_num++] = endpoint;
      }
      start += *(start);
    }
    return 1;
}

int handle_configuration(UsbDev* dev, uint8_t* string_buffer, uint8_t* config_buffer, ConfigurationDescriptor* config_descriptor, uint8_t num_configurations){
  MemoryService_C* mem_service = (MemoryService_C*)container_of(dev->mem_service, MemoryService_C, super);
  uint16_t total_len;
  uint8_t desc_len;
  uint8_t *start;
  uint8_t *end;
  uint8_t s_len;
  char* ascii_string;
  uint8_t config_sel = 0, current_entry = 0;

  Configuration **configurations =
      (Configuration **)mem_service->allocateKernelMemory_c(
          mem_service, sizeof(Configuration *) * num_configurations, 0);
  dev->supported_configs = configurations;
  // default to config0, interface0
  for (int i = 0; i < num_configurations; i++) {
    // request first 4 bytes of each config desc (wTotalLength)
    if(dev->process_configuration_descriptor(dev, config_descriptor, i, 4) == -1)
      return -1;
    
    total_len = config_descriptor->wTotalLength;
    desc_len = config_descriptor->bLength;

    if (total_len > CONFIG_BUFFER_SIZE) {
      continue;
    }

    if(dev->process_whole_configuration(dev, config_buffer, i, total_len) == -1)
      return -1;

    Configuration *configuration =
        (Configuration *)mem_service->allocateKernelMemory_c(
            mem_service, sizeof(Configuration), 0);
    ConfigurationDescriptor *config_desc =
        (ConfigurationDescriptor *)config_buffer;
    configuration->config_description = "";    

    if (config_desc->iConfiguration != 0) {
      if(dev->process_string_descriptor(dev, string_buffer,
          config_desc->iConfiguration, dev->lang_id, "iConfiguration", 1) == -1)
          return -1;
  
      s_len = string_buffer[0];

      if(dev->process_string_descriptor(
          dev, string_buffer, config_desc->iConfiguration, dev->lang_id,
          "iConfiguration", s_len) == -1)
          return -1;

      ascii_string = dev->build_string(dev, s_len, string_buffer);
      configuration->config_description = ascii_string;
    }

    if (!config_sel) {
      dev->active_config = configuration;
      config_sel = 1;
    }

    start = config_buffer + desc_len;
    end = config_buffer + total_len;

    configuration->config_desc = *config_desc;
    
    dev->handle_interface(dev, configuration, string_buffer, start, end, config_desc->bNumInterfaces);
    
    configurations[current_entry++] = configuration;
  }

  return 1;
}

int handle_lang(UsbDev* dev, uint8_t* string_buffer){
  uint8_t s_len;

  if(dev->process_string_descriptor(dev, string_buffer, 0, 0, "langs", 1) == -1)
    return -1;

  s_len = string_buffer[0];

  if(dev->process_string_descriptor(dev, string_buffer, 0, 0, "langs", s_len) == -1)
    return -1;

  dev->process_lang_ids(dev, string_buffer, s_len);

  return 1;
}

int handle_dev(UsbDev* dev, uint8_t* string_buffer, DeviceDescriptor* device_descriptor){
  uint8_t s_len;
  char *ascii_string;

  if(dev->process_device_descriptor(dev, device_descriptor,
                                 sizeof(DeviceDescriptor)) == -1)
    return -1;

  dev->device_desc = *device_descriptor;

  if (device_descriptor->iManufacturer != 0) {

    if(dev->process_string_descriptor(
        dev, string_buffer, device_descriptor->iManufacturer, dev->lang_id,
        "iManufacturer", 1) == -1)
        return -1;

    s_len = string_buffer[0];

    if(dev->process_string_descriptor(
        dev, string_buffer, device_descriptor->iManufacturer, dev->lang_id,
        "iManufacturer", s_len) == -1)
        return -1;

    ascii_string = dev->build_string(dev, s_len, string_buffer);
    dev->manufacturer = ascii_string;
  }

  if (device_descriptor->iProduct != 0) {

    if(dev->process_string_descriptor(dev, string_buffer,
                                                  device_descriptor->iProduct,
                                                  dev->lang_id, "iProduct", 1) == -1)
      return -1;

    s_len = string_buffer[0];

    if(dev->process_string_descriptor(
        dev, string_buffer, device_descriptor->iProduct, dev->lang_id,
        "iProduct", s_len) == -1)
      return -1;

    ascii_string = dev->build_string(dev, s_len, string_buffer);
    dev->product = ascii_string;
  }

  if (device_descriptor->iSerialNumber != 0) {

    if(dev->process_string_descriptor(
        dev, string_buffer, device_descriptor->iSerialNumber, dev->lang_id,
        "iSerialNumber", 1) == -1)
        return -1;

    s_len = string_buffer[0];

    if(dev->process_string_descriptor(
        dev, string_buffer, device_descriptor->iSerialNumber, dev->lang_id,
        "iSerialNumber", s_len) == -1)
        return -1;

    ascii_string = dev->build_string(dev, s_len, string_buffer);
    dev->serial_number = ascii_string;
  }

  return 1;
}

void init_device_functions(UsbDev *dev) {
  // todo add all required functions in here
  dev->request_build = &request_build;
  dev->request = &request;
  dev->usb_dev_control = &usb_dev_control;
  dev->usb_dev_interrupt = &usb_dev_interrupt;
  dev->usb_dev_bulk = &usb_dev_bulk;
  dev->usb_dev_interface_lock = &usb_dev_interface_lock;
  dev->usb_dev_free_interface = &usb_dev_free_interface;
  dev->request_callback = &request_callback;
  dev->is_pipe_buildable = &is_pipe_buildable;
  dev->contain_interface = &contain_interface;
  dev->request_switch_configuration = &request_switch_configuration;
  dev->request_switch_alternate_setting = &request_switch_alternate_setting;
  dev->support_bulk = &support_bulk;
  dev->support_isochronous = &support_isochronous;
  dev->support_control = &support_control;
  dev->support_interrupt = &support_interrupt;
  dev->support_transfer_type = &support_transfer_type;
  dev->dump_device_desc = &dump_device_desc;
  dev->dump_configuration = &dump_configuration;
  dev->dump_interface = &dump_interface;
  dev->dump_endpoints = &dump_endpoints;
  dev->dump_endpoint = &dump_endpoint;
  dev->init_device_logger = &init_device_logger;
  dev->build_string = &build_string;
  dev->process_lang_ids = &process_lang_ids;
  dev->dump_device = &dump_device;
  dev->get_free_device_request = &get_free_device_request;
  dev->free_device_request = &free_device_request;
  dev->set_report = &set_report;
  dev->set_protocol = &set_protocol;
  dev->set_idle = &set_idle;
  dev->process_device_descriptor = &process_device_descriptor;
  dev->process_configuration_descriptor = &process_configuration_descriptor;
  dev->process_string_descriptor = &process_string_descriptor;
  dev->set_address = &set_address;
  dev->set_configuration = &set_configuration;
  dev->process_whole_configuration = &process_whole_configuration;
  dev->handle_interface = &handle_interface;
  dev->handle_configuration = &handle_configuration;
  dev->handle_lang = &handle_lang;
  dev->handle_dev = &handle_dev;
}



char *build_string(UsbDev *dev, int len, uint8_t *string_buffer) {
  MemoryService_C *mem_service =
      (MemoryService_C *)container_of(dev->mem_service, MemoryService_C, super);

  uint8_t *start_string_b = string_buffer + 2;
  uint8_t *end_string_b = string_buffer + len;

  char *ascii_string =
      (char *)mem_service->allocateKernelMemory_c(mem_service, len - 1, 0);
  int ascii_index = 0;

  while (start_string_b < end_string_b) {
    ascii_string[ascii_index++] = *((uint8_t *)((uint16_t *)start_string_b));
    start_string_b += 2;
  }
  ascii_string[ascii_index] = '\0';

  return ascii_string;
}

void process_lang_ids(UsbDev *dev, uint8_t *string_buffer, int s_len) {
  uint8_t *start_string_b = string_buffer + 2;
  uint8_t *end_string_b = string_buffer + s_len;

  uint16_t *lang_id;
  int lang_id_index = 0, lang_sel = 0;

  while (start_string_b < end_string_b) {
    lang_id = (uint16_t *)start_string_b;
    if (!lang_sel) {
      dev->lang_id = *lang_id;
      lang_sel = 1;
    }
    if (lang_id_index < SUPPORTED_LANGID_SIZE) {
      *(dev->lang_ids + lang_id_index) = *lang_id;
      *(dev->lang_ids + lang_id_index + 1) = 0;
      lang_id_index++;
    }
    start_string_b += 2;
  }
}

int set_address(UsbDev *dev, uint8_t address) {
  UsbDeviceRequest *device_req = dev->get_free_device_request(dev);

  if (device_req == (void *)0)
    return -1;

  dev->request_build(dev, device_req, HOST_TO_DEVICE, SET_ADDRESS, 0, address,
                     0, 0, 0);
  dev->request(dev, device_req, 0, PRIORITY_QH_8, 0, &request_callback);

  if (dev->error_while_transfering) {
    dev->device_logger->error_c(dev->device_logger,
                                "Couldn't set address of device !");
    return -1;
  }

  return 1;
}

void init_device_logger(UsbDev *dev) {
  MemoryService_C *m =
      (MemoryService_C *)container_of(dev->mem_service, MemoryService_C, super);
  Logger_C *logger =
      (Logger_C *)m->allocateKernelMemory_c(m, sizeof(Logger_C), 0);
  logger->new_logger = &new_logger;
  logger->new_logger(logger, USB_DEVICE_LOGGER_TYPE, LOGGER_LEVEL_DEBUG);

  dev->device_logger = logger;
}

void request_callback(UsbDev *dev, uint32_t status, void *data) {
  if (status & E_TRANSFER) {
    dev->error_while_transfering = 1;
  } else if (status & S_TRANSFER) {
    dev->error_while_transfering = 0;
  }
}

void request_build(UsbDev *dev, UsbDeviceRequest *device_request,
                   int8_t rq_type, int8_t rq, int16_t value_high,
                   int16_t value_low, int16_t shift, int16_t index,
                   int16_t len) {
  device_request->bmRequestType = rq_type;
  device_request->bRequest = rq;
  device_request->wValue = value_high << shift | value_low;
  device_request->wIndex = index;
  device_request->wLength = len;
}

void request(UsbDev *dev, UsbDeviceRequest *device_request, void *data,
             uint8_t priority, Endpoint *endpoint, callback_function callback) {
  ((UsbController*)dev->controller)->control_entry_point(dev, device_request, data, priority, endpoint, callback);
}

// update struct Interface + active field in alternate setting
int request_switch_alternate_setting(UsbDev *dev, Interface *interface,
                                     int setting, callback_function callback) {
  if (dev->contain_interface(dev, interface))
    return -1;
  Alternate_Interface *current = interface->alternate_interfaces;
  while (current != (void *)0) {
    if (current->alternate_interface_desc.bAlternateSetting == setting) {
      UsbDeviceRequest *device_req = dev->get_free_device_request(dev);
      if (device_req == (void *)0)
        return -1;
      dev->request_build(dev, device_req, HOST_TO_DEVICE | RECIPIENT_INTERFACE,
                         SET_INTERFACE, 0, setting, 0,
                         interface->active_interface->alternate_interface_desc
                             .bInterfaceNumber,
                         0);
      dev->request(dev, device_req, 0, PRIORITY_QH_8, 0, callback);
      return 1;
    }
    current = current->next;
  }
  return -1;
}

// switch active config field in UsbDev
int request_switch_configuration(UsbDev *dev, int configuration,
                                 callback_function callback) {
  unsigned int config_count = dev->device_desc.bNumConfigurations;
  Configuration **allowed_configs = dev->supported_configs;

  if(config_count < configuration) return -1;

  for (int i = 0; i < config_count; i++) {
    Configuration *config = allowed_configs[i];
    if (config->config_desc.bConfigurationValue == configuration) {
      UsbDeviceRequest *device_req = dev->get_free_device_request(dev);
      if (device_req == (void *)0)
        return -1;
      dev->request_build(dev, device_req, HOST_TO_DEVICE, SET_CONFIGURATION, 0,
                         configuration, 0, 0, 0);
      dev->request(dev, device_req, 0, PRIORITY_QH_8, 0, callback);
      dev->active_config = allowed_configs[i];
      return 1;
    }
  }

  return -1;
}

int process_device_descriptor(UsbDev *dev, DeviceDescriptor *device_descriptor,
                              unsigned int len) {
  UsbDeviceRequest *request = dev->get_free_device_request(dev);

  if (request == (void *)0)
    return -1;

  dev->request_build(dev, request, DEVICE_TO_HOST, GET_DESCRIPTOR, DEVICE, 0, 8,
                     0, len);
  dev->request(dev, request, device_descriptor, PRIORITY_QH_8, 0,
               &request_callback);

  if (dev->error_while_transfering) {
    dev->device_logger->error_c(
        dev->device_logger, "Couldn't read device descriptor from device !");
    return -1;
  }

  return 1;
}

int process_configuration_descriptor(UsbDev *dev,
                                     ConfigurationDescriptor *config_descriptor,
                                     uint16_t configuration_index,
                                     unsigned int len) {
  DeviceDescriptor device_descriptor = dev->device_desc;
  if (configuration_index > device_descriptor.bNumConfigurations)
    return -1;
  if (dev->state == DEFAULT_STATE)
    return -1;

  UsbDeviceRequest *request = dev->get_free_device_request(dev);

  if (request == (void *)0)
    return -1;

  dev->request_build(dev, request, DEVICE_TO_HOST, GET_DESCRIPTOR,
                     CONFIGURATION, configuration_index, 8, 0, len);
  dev->request(dev, request, config_descriptor, PRIORITY_QH_8, 0,
               &request_callback);

  if (dev->error_while_transfering) {
    dev->device_logger->error_c(
        dev->device_logger,
        "Couldn't read configuration descriptor from device !");
    return -1;
  }

  return 1;
}

int process_whole_configuration(UsbDev *dev, uint8_t *config_buffer,
                                uint16_t configuration_index,
                                unsigned int len) {
  DeviceDescriptor device_descriptor = dev->device_desc;
  if (configuration_index > device_descriptor.bNumConfigurations)
    return -1;
  if (dev->state == DEFAULT_STATE)
    return -1;

  UsbDeviceRequest *request = dev->get_free_device_request(dev);

  if (request == (void *)0)
    return -1;

  dev->request_build(dev, request, DEVICE_TO_HOST, GET_DESCRIPTOR,
                     CONFIGURATION, configuration_index, 8, 0, len);
  dev->request(dev, request, config_buffer, PRIORITY_QH_8, 0,
               &request_callback);

  if (dev->error_while_transfering) {
    dev->device_logger->error_c(
        dev->device_logger,
        "Couldn't read configuration descriptor from device !");
    return -1;
  }

  return 1;
}

int process_string_descriptor(UsbDev *dev, uint8_t *string_buffer,
                                uint16_t index, uint16_t lang_id, char *s,
                                unsigned int len) {
  if (lang_id != 0 && lang_id != dev->lang_id)
    return -1;
  if (s == (void *)0)
    return -1;

  UsbDeviceRequest *request = dev->get_free_device_request(dev);

  if (request == (void *)0)
    return -1;

  dev->request_build(dev, request, DEVICE_TO_HOST, GET_DESCRIPTOR, STRING,
                     index, 8, lang_id, len);
  dev->request(dev, request, string_buffer, PRIORITY_QH_8, 0,
               &request_callback);

  if (dev->error_while_transfering) {
    dev->device_logger->error_c(
        dev->device_logger, "Couldn't read string descriptor from %s index!",
        s);
    return -1;
  }

  return 1;
}

int set_configuration(UsbDev *dev, uint8_t configuration) {
  if (configuration > dev->active_config->config_desc.bNumInterfaces)
    return -1;

  UsbDeviceRequest *device_req = dev->get_free_device_request(dev);

  if (device_req == (void *)0)
    return -1;

  dev->request_build(dev, device_req, HOST_TO_DEVICE, SET_CONFIGURATION, 0,
                     dev->active_config->config_desc.bConfigurationValue, 0, 0,
                     0);
  dev->request(dev, device_req, 0, PRIORITY_QH_8, 0, &request_callback);

  if (dev->error_while_transfering) {
    dev->device_logger->error_c(dev->device_logger,
                                "Couldn't set configuration of device !");
    return -1;
  }

  return 1;
}

int usb_dev_interface_lock(UsbDev *dev, Interface *interface, void* driver) {
  int interface_count = dev->active_config->config_desc.bNumInterfaces;
  Interface **interfaces = dev->active_config->interfaces;

  //dev->device_mutex->acquire_c(dev->device_mutex);

  int i_found = 0;
  for (int i = 0; i < interface_count; i++) {
    if (interfaces[i] == interface)
      i_found = 1;
  }

  if (!i_found)
    return E_INTERFACE_INV;

  if (interface->active == 1)
    return E_INTERFACE_IN_USE;

  interface->active = 1;
  interface->driver = driver;

  //dev->device_mutex->release_c(dev->device_mutex);

  return 1;
}

void usb_dev_free_interface(UsbDev *dev, Interface *interface) {
  interface->active = 0;
  interface->driver = 0;
}

UsbDeviceRequest *get_free_device_request(UsbDev *dev) {
  for (int i = 0; i < PAGE_SIZE / sizeof(UsbDeviceRequest); i++) {
    //dev->device_mutex->acquire_c(dev->device_mutex);
    if (dev->device_request_map_io_bitmap[i] == 0) {
      dev->device_request_map_io_bitmap[i] = 1;
      //dev->device_mutex->release_c(dev->device_mutex);
      return (UsbDeviceRequest *)(dev->device_request_map_io +
                                  (i * sizeof(UsbDeviceRequest)));
    }
    //dev->device_mutex->release_c(dev->device_mutex);
  }
  return (void *)0;
}

void free_device_request(UsbDev *dev, UsbDeviceRequest *device_request) {
  for (int i = 0; i < PAGE_SIZE; i += sizeof(UsbDeviceRequest)) {
    //dev->device_mutex->acquire_c(dev->device_mutex);
    if ((dev->device_request_map_io_bitmap + i) == (uint8_t *)device_request) {
      //dev->device_request_map_io_bitmap[i / sizeof(UsbDeviceRequest)] = 0;
      //dev->device_mutex->release_c(dev->device_mutex);
      return;
    }
    //dev->device_mutex->release_c(dev->device_mutex);
  }
}

int set_report(UsbDev *dev, Interface *interface, uint8_t type, void *data,
               unsigned int len, callback_function callback) {
  if (type != REQUEST_OUTPUT && type != REQUEST_INPUT)
    return -1;
  if (!dev->contain_interface(dev, interface))
    return -1;

  UsbDeviceRequest *request = dev->get_free_device_request(dev);

  if (request == (void *)0)
    return -1;

  dev->request_build(
      dev, request, HOST_TO_DEVICE | TYPE_REQUEST_CLASS | RECIPIENT_INTERFACE,
      SET_REPORT, type, 0, 8,
      interface->active_interface->alternate_interface_desc.bInterfaceNumber,
      len);
  dev->request(dev, request, data, PRIORITY_QH_8, 0, callback);

  return 1;
}

int set_protocol(UsbDev *dev, Interface *interface, uint16_t protocol_value,
                 callback_function callback) {
  if (protocol_value != USE_BOOT_PROTOCOL &&
      protocol_value != USE_REPORT_PROTOCOL)
    return -1;
  if (!dev->contain_interface(dev, interface))
    return -1;

  UsbDeviceRequest *request = dev->get_free_device_request(dev);

  if (request == (void *)0)
    return -1;

  dev->request_build(
      dev, request, HOST_TO_DEVICE | TYPE_REQUEST_CLASS | RECIPIENT_INTERFACE,
      SET_PROTOCOL, 0, USE_REPORT_PROTOCOL, 0,
      interface->active_interface->alternate_interface_desc.bInterfaceNumber,
      0);
  dev->request(dev, request, 0, PRIORITY_QH_8, 0, callback);

  return 1;
}

int set_idle(UsbDev *dev, Interface *interface) {
  if (!dev->contain_interface(dev, interface))
    return -1;

  UsbDeviceRequest *request = dev->get_free_device_request(dev);

  if (request == (void *)0)
    return -1;

  dev->request_build(
      dev, request, HOST_TO_DEVICE | TYPE_REQUEST_CLASS | RECIPIENT_INTERFACE,
      SET_IDLE, 0, 0, 0,
      interface->active_interface->alternate_interface_desc.bInterfaceNumber,
      0);
  dev->request(dev, request, 0, PRIORITY_QH_8, 0, &request_callback);

  if (dev->error_while_transfering) {
    dev->device_logger->error_c(dev->device_logger,
                                "Set idle request invalid ... ");
    return -1;
  }

  return 1;
}

// just support get descriptor, get configuration, get interface, get status,
// set interface
void usb_dev_control(UsbDev *dev, Interface *interface, unsigned int pipe,
                     uint8_t priority, void *data, uint8_t *setup,
                     callback_function callback) {
  if (!dev->contain_interface(dev, interface)) {
    callback(dev, E_INTERFACE_NOT_SUPPORTED, data);
    return;
  }

  unsigned int endpoint_count =
      interface->active_interface->alternate_interface_desc.bNumEndpoints;
  Endpoint **endpoints = interface->active_interface->endpoints;
  UsbDeviceRequest *device_req = (UsbDeviceRequest *)setup;

  for (int i = 0; i < endpoint_count; i++) {
    if (dev->is_pipe_buildable(endpoints[i], pipe)) {
      dev->request(dev, device_req, data, priority, endpoints[i], callback);
      return;
    }
  }
  callback(dev, E_ENDPOINT_INV, data);
}

void usb_dev_bulk(struct UsbDev *dev, Interface *interface, unsigned int pipe,
                  uint8_t priority, void *data, unsigned int len,
                  callback_function callback) {

  if (!dev->contain_interface(dev, interface)) {
    callback(dev, E_INTERFACE_NOT_SUPPORTED, data);
    return;
  }

  unsigned int endpoint_count =
      interface->active_interface->alternate_interface_desc.bNumEndpoints;
  Endpoint **endpoints = interface->active_interface->endpoints;

  for (int i = 0; i < endpoint_count; i++) {
    if (dev->is_pipe_buildable(endpoints[i], pipe)) {
      ((UsbController*)dev->controller)->bulk_entry_point(dev, endpoints[i], data, len, priority, callback);
      return;
    }
  }
  callback(dev, E_ENDPOINT_INV, data);
}

void usb_dev_interrupt(UsbDev *dev, Interface *interface, unsigned int pipe,
                       uint8_t priority, void *data, unsigned int len,
                       uint16_t interval, callback_function callback) {

  if (!dev->contain_interface(dev, interface)) {
    callback(dev, E_INTERFACE_NOT_SUPPORTED, data);
    return;
  }

  unsigned int endpoint_count =
      interface->active_interface->alternate_interface_desc.bNumEndpoints;
  Endpoint **endpoints = interface->active_interface->endpoints;

  for (int i = 0; i < endpoint_count; i++) {
    if (dev->is_pipe_buildable(endpoints[i], pipe)) {
      ((UsbController*)dev->controller)->interrupt_entry_point(dev, endpoints[i], data, len, priority, interval,
                            callback);
      return;
    }
  }
  callback(dev, E_ENDPOINT_INV, data);
}

int contain_interface(UsbDev *dev, Interface *interface) {
  Interface **interfaces = dev->active_config->interfaces;

  for (int i = 0; i < dev->active_config->config_desc.bNumInterfaces; i++) {
    if (interfaces[i] == interface)
      return 1;
  }

  return -1;
}

int is_pipe_buildable(Endpoint *endpoint, unsigned int pipe) {
  unsigned int type;
  unsigned int end_point;
  unsigned int direction;
  unsigned int reserved;

  unsigned int endpoint_dev;
  unsigned int type_dev;
  unsigned int direction_dev;

  type = (pipe & CONTROL_PIPE_MASK) >> 5;
  end_point = pipe & ENDPOINT_MASK;
  reserved = (pipe & 0x10) >> 4;
  direction = pipe & DIRECTION_MASK;

  endpoint_dev = endpoint->endpoint_desc.bEndpointAddress & ENDPOINT_MASK;
  direction_dev = endpoint->endpoint_desc.bEndpointAddress & DIRECTION_MASK;
  type_dev = endpoint->endpoint_desc.bmAttributes & TRANSFER_TYPE_MASK;

  return ((type == type_dev) && (end_point == endpoint_dev) &&
          (direction == direction_dev) && (reserved == 0));
}

int8_t support_bulk(UsbDev *dev, Interface *interface) {
  return dev->support_transfer_type(dev, interface, TRANSFER_TYPE_BULK);
}

int8_t support_isochronous(UsbDev *dev, Interface *interface) {
  return dev->support_transfer_type(dev, interface, TRANSFER_TYPE_ISO);
}

int8_t support_control(UsbDev *dev, Interface *interface) {
  return dev->support_transfer_type(dev, interface, TRANSFER_TYPE_CONTROL);
}

int8_t support_interrupt(UsbDev *dev, Interface *interface) {
  return dev->support_transfer_type(dev, interface, TRANSFER_TYPE_INTERRUPT);
}

int8_t support_transfer_type(UsbDev* dev, Interface* interface, uint8_t transfer_type){
  Endpoint **endpoints = interface->active_interface->endpoints;
  int8_t supports = 0;
  Alternate_Interface *active_interface = interface->active_interface;
  for (int i = 0; i < active_interface->alternate_interface_desc.bNumEndpoints;
       i++) {
    if ((endpoints[i]->endpoint_desc.bmAttributes & TRANSFER_TYPE_MASK) ==
        transfer_type)
      supports = 1;
  }
  return supports;
}

void dump_device_desc(UsbDev *dev) {
  DeviceDescriptor dev_desc = dev->device_desc;
  char *message =
      "Dumping Device Descriptor :\n"
      "\tbLength : %u\n\tbType : %u\n\tbcdUsb : %u\n\tbDeviceClass : %u\n"
      "\tbDeviceSubClass : %u\n\tbDeviceProtocol : %u\n\tbMaxPacketSize : %u\n"
      "\tidVendor : %u\n\tidProduct : %u\n\tbcdDevice : %u\n\tiManufacturer : "
      "%u\n"
      "\tiProduct : %u\n\tiSerialNumber : %u\n\tbNumConfigs : %u\n";

  dev->device_logger->debug_c(
      dev->device_logger, message, dev_desc.bLength, dev_desc.bDescriptorType,
      dev_desc.bcdUSB, dev_desc.bDeviceClass, dev_desc.bDeviceSubClass,
      dev_desc.bDeviceProtocol, dev_desc.bMaxPacketSize0, dev_desc.idVendor,
      dev_desc.idProduct, dev_desc.bcdDevice, dev_desc.iManufacturer,
      dev_desc.iProduct, dev_desc.iSerialNumber, dev_desc.bNumConfigurations);
}

void dump_configuration(UsbDev *dev) {
  ConfigurationDescriptor config_desc = dev->active_config->config_desc;
  char *message =
      "Config Descriptor : \n"
      "\tbLength : %u\n\tbType : %u\n\twTotalLength : %u\n"
      "\tbNumInterfaces : %u\n\tbConfigurationValue : %u\n"
      "\tiConfiguration : %u\n\tSelf-Power : %u\n\tRemote-Wakeup : %u\n"
      "\tbMaxPower : %u";

  dev->device_logger->debug_c(
      dev->device_logger, message, config_desc.bLength,
      config_desc.bDescriptorType, config_desc.wTotalLength,
      config_desc.bNumInterfaces, config_desc.bConfigurationValue,
      config_desc.iConfiguration, config_desc.bmAttributes & POWERED_MASK,
      config_desc.bmAttributes & REMOTE_WAKEUP_MASK, config_desc.bMaxPower);
}

void dump_interface(struct UsbDev *dev, Interface *interface) {
  InterfaceDescriptor interface_desc =
      interface->active_interface->alternate_interface_desc;

  char *message = "Interface : \n"
                  "\tactive : %u\n\tdriver : %x\n\tinterface-description : %s";

  dev->device_logger->debug_c(dev->device_logger, message, interface->active,
                              interface->driver,
                              interface->interface_description);

  message = "Interface Descriptor : \n"
            "\tbLength : %u\n\tbType : %u\n\tbInterfaceNumber : %u\n"
            "\tbAlternateSetting : %u\n\tbNumEndpoints : %u\n"
            "\tbInterfaceClass : %u\n\tbInterfaceSubClass : %u\n"
            "\tbInterfaceProtocol : %u\n\tiInterface : %u\n";

  dev->device_logger->debug_c(
      dev->device_logger, message, interface_desc.bLength,
      interface_desc.bDescriptorType, interface_desc.bInterfaceNumber,
      interface_desc.bAlternateSetting, interface_desc.bNumEndpoints,
      interface_desc.bInterfaceClass, interface_desc.bInterfaceSubClass,
      interface_desc.bInterfaceProtocol, interface_desc.iInterface);
}

void dump_endpoints(struct UsbDev *dev, Interface *interface) {
  Endpoint **endpoints = interface->active_interface->endpoints;
  uint8_t endpoint_num =
      interface->active_interface->alternate_interface_desc.bNumEndpoints;
  for (int i = 0; i < endpoint_num; i++) {
    dev->dump_endpoint(dev, endpoints[i]);
  }
}

void dump_endpoint(struct UsbDev *dev, Endpoint *endpoint) {
  EndpointDescriptor endpoint_desc = endpoint->endpoint_desc;
  char *message =
      "Endpoint Descriptor : \n"
      "\tbLength : %u\n\tbType : %u\n\tDirection : %u\n\tEndpoint : %u\n"
      "\tTransfer-Type : %u\n\twMaxPacketSize : %u\n\tbInterval : %u\n";

  dev->device_logger->debug_c(
      dev->device_logger, message, endpoint_desc.bLength,
      endpoint_desc.bDescriptorType,
      endpoint_desc.bEndpointAddress & DIRECTION_MASK,
      endpoint_desc.bEndpointAddress & ENDPOINT_MASK,
      endpoint_desc.bmAttributes & TRANSFER_TYPE_MASK,
      endpoint_desc.wMaxPacketSize, endpoint_desc.bInterval);
}

void dump_device(struct UsbDev *dev) {
  char *message = "Usb-Device : \n"
                  "\tspeed : %u\n\tport : %u\n\taddress : %u\n\tstate : %s\n"
                  "\tlang-id : 0x%04x\n\tmanufacturer : %s\n\tproduct : %s\n"
                  "\tserial-number : %s";
  char *s = "";
  if (dev->state == DEFAULT_STATE) {
    s = "default-state";
  } else if (dev->state == ADDRESS_STATE) {
    s = "address-state";
  } else if (dev->state == CONFIGURED_STATE) {
    s = "config-state";
  }

  dev->device_logger->debug_c(
      dev->device_logger, message, dev->speed, dev->port, dev->address, s,
      dev->lang_id, dev->manufacturer, dev->product, dev->serial_number);
  dev->dump_device_desc(dev);
  dev->dump_configuration(dev);

  int interface_num = dev->active_config->config_desc.bNumInterfaces;
  for (int i = 0; i < interface_num; i++) {
    dev->dump_interface(dev, dev->active_config->interfaces[i]);
    dev->dump_endpoints(dev, dev->active_config->interfaces[i]);
  }
}