cmake_minimum_required(VERSION 3.14)

target_sources(kernel PUBLIC 
        ${HHUOS_SRC_DIR}/kernel/usb/interface/UsbRegistry.cpp
        ${HHUOS_SRC_DIR}/kernel/usb/node/UsbNode.cpp
        ${HHUOS_SRC_DIR}/kernel/usb/node/hid/KeyBoardNode.cpp
        ${HHUOS_SRC_DIR}/kernel/usb/node/hid/MouseNode.cpp
        ${HHUOS_SRC_DIR}/kernel/usb/driver/hid/KernelKbdDriver.cpp
        ${HHUOS_SRC_DIR}/kernel/usb/driver/hid/KernelMouseDriver.cpp
        ${HHUOS_SRC_DIR}/kernel/usb/driver/KernelUsbDriver.cpp
        ${HHUOS_SRC_DIR}/kernel/usb/driver/hub/KernelHubDriver.cpp
        ${HHUOS_SRC_DIR}/kernel/usb/driver/storage/KernelMassStorageDriver.cpp
        ${HHUOS_SRC_DIR}/kernel/usb/driver/audio/KernelAudioDriver.cpp
        ${HHUOS_SRC_DIR}/kernel/usb/node/storage/MassStorageNode.cpp
        ${HHUOS_SRC_DIR}/kernel/usb/node/audio/AudioNode.cpp
        ${HHUOS_SRC_DIR}/kernel/usb/node/UsbMemoryNode.cpp
        ${HHUOS_SRC_DIR}/kernel/usb/node/UsbStreamNode.cpp)