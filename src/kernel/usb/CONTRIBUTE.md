# USB Driver Implementation

This section provides insights in how to extend the USB System, with a new custom USB Driver.

## Supported USB Drivers

Currently the below listed drivers are integrated :

- Audio Device Driver
- Mass Storage Device Driver
- Keyboard Driver
- Mouse Driver
- Hub Driver

## Key Points

The entry point for each USB driver is **[kernel_usb_folder][2]**. In here the USB driver can choose which device should be handled by the driver itself,
if it should contain a specific driver listener or if it should contain a specific sub routine.

### USB Device Matching

To match a specific device, the `UsbDevice_ID` is used, which allow to specify a custom query to find the desire device.

Most of the following predefined match queries are enough to search for the specific devices :

```shell
USB_DEVICE(vendor, product)
USB_DEVICE_VER(vendor, product, low, high)
USB_DEVICE_INFO(class_d, subclass, protocol)
USB_INTERFACE_INFO(class_i, subclass, protocol) 
```

An example would be an audio driver, which would fill the request as following, to find devices that have either an audio control interface or 
and audio streaming interface.

```shell
{USB_INTERFACE_INFO(AUDIO, AUDIO_CONTROL, 0xFF),
USB_INTERFACE_INFO(AUDIO, AUDIO_STREAMING, 0xFF),
{}}
```
**The UsbDevice_ID array always has to contain an zero filled `UsbDevice_ID` at the end!**

Most of the codes are already specified in **[here][3]**.

Is the code for your device not supported yet ? No problem just extend the header with your specific codes.

### USB Driver Registration

After creating the specific query/queries described in [here](#usb-device-matching), the `UsbDriver` has to be registered, using the
`add_driver`, located inside the `UsbService` component. This registration will match all the specified match queries, with the devices/interfaces.
If in this process all checks pass, this signals that there might be a chance this device could be a candidat for the driver to use.

To check if the driver, should lock this interface the following routine
specified inside the `UsbDriver` is called.

```shell
int16_t (*probe)(UsbDev* dev, Interface* interface);
```

This probe function can return >= 0 indicating that this interface should be supported by the driver or < 0 indicating that this interface should not be used.

If the driver specifies a >= 0 for that interface, this interface is locked and binded to this driver.

### USB Driver Transfer Submission

After registering the specific driver described in [here](#usb-driver-registration), and assuming that a device was found the last step is to create a USB Transfer and submit it to the USB controller.

For this purpose the `UsbService` component contains four different transfers -- control, bulk, interrupt and iso transefer -- that can be used dependent on the endpoint

The following routine function, is required to be inserted in every transfer.

```shell
void (*callback_function)(UsbDev* dev, Interface* itf, uint32_t status, void *data);
```

This function is called by the controller, if the transfer was successful or not.
In here the actual handling of the transfer should occur.

---

#### Bulk Transfer

```shell
uint32_t submit_bulk_transfer(Interface *interface, unsigned int pipe,
    uint8_t prio, void *data, unsigned int len, callback_function callback);
```

---

#### Interrupt Transfer

```shell
uint32_t submit_interrupt_transfer(Interface *interface, unsigned int pipe,
    uint8_t prio, uint16_t interval, void *data, unsigned int len, callback_function callback);
```

---

#### Iso Transfer

```shell
uint32_t submit_iso_transfer(Interface* interface, unsigned int pipe, 
    uint8_t prio, uint16_t interval, void* data, unsigned int len, callback_function callback);
```

---

#### Control Transfer

```shell
uint32_t submit_control_transfer(Interface *interface, unsigned int pipe,
    uint8_t prio, void *data, uint8_t *setup, callback_function callback);
```

## Extend Supported USB Devices

When running the `--usb-fast` option inside the `run.sh` script, all available devices on the host system are matched with all supported devices 
listed in **[usb_supported_devices.yml][1]**.

To pass a specific USB device to the system, it is therefore required to extend the yml-file to include your device.

To retrieve the information about your devices connected to your host system, just run `lsusb`.
This will display entries in the following format :

`[Bus [f1] Device [f2]: ID [f3]:[f4] [f5]]`

Required fields are `f3,f4 and f5`.
`[f3:f4]` has to be inserted as entry inside `id` and 
`[f5]` as entry inside `name`.

Another point to notice is that the devices should be enumerated in increasing order. Meaning that if audio0 is already in use, the next audio device **must contain audio1**.

## Further Information

For those gathering more information about the whole systems design and workings, you're welcome to have a look at following [thesis][4].

[1]: <../../device/usb/usb_supported_devices.yml> "supported usb devices"
[2]: <../usb/> "usb entry point"
[3]: <../../device/usb/driver/UsbDriver.h> "Usb Driver"
[4]: <../../../media/documents/thesis.pdf> "Thesis"