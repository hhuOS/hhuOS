cmake_minimum_required(VERSION 3.14)

target_sources(kernel PUBLIC 
        ${HHUOS_SRC_DIR}/kernel/usb/UsbRegistry.cpp
        ${HHUOS_SRC_DIR}/kernel/usb/UsbNode.cpp
        ${HHUOS_SRC_DIR}/kernel/usb/hid/KeyBoardNode.cpp
        ${HHUOS_SRC_DIR}/kernel/usb/hid/MouseNode.cpp
        ${HHUOS_SRC_DIR}/kernel/usb/driver/KernelKbdDriver.cpp
        ${HHUOS_SRC_DIR}/kernel/usb/driver/KernelMouseDriver.cpp
        ${HHUOS_SRC_DIR}/kernel/usb/driver/KernelUsbDriver.cpp
        ${HHUOS_SRC_DIR}/kernel/usb/driver/KernelHubDriver.cpp
        ${HHUOS_SRC_DIR}/kernel/usb/driver/KernelMassStorageDriver.cpp
        ${HHUOS_SRC_DIR}/kernel/usb/driver/KernelAudioDriver.cpp
        ${HHUOS_SRC_DIR}/kernel/usb/storage/MassStorageNode.cpp
        ${HHUOS_SRC_DIR}/kernel/usb/audio/AudioNode.cpp
        ${HHUOS_SRC_DIR}/kernel/usb/UsbMemoryNode.cpp
        ${HHUOS_SRC_DIR}/kernel/usb/UsbStreamNode.cpp)