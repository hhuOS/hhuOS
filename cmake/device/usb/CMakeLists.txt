cmake_minimum_required(VERSION 3.14)

target_sources(device PUBLIC 
        ${HHUOS_SRC_DIR}/device/usb/include/UsbControllerInclude.c
        ${HHUOS_SRC_DIR}/device/usb/controller/UsbController.c
        ${HHUOS_SRC_DIR}/device/usb/controller/uhci/UHCI.c
        ${HHUOS_SRC_DIR}/device/usb/dev/UsbDevice.c
        ${HHUOS_SRC_DIR}/device/usb/controller/uhci/components/UHCIMemory.c
        ${HHUOS_SRC_DIR}/device/usb/controller/uhci/components/UHCIRegister.c
        ${HHUOS_SRC_DIR}/device/usb/interfaces/SystemInterface.cpp
        ${HHUOS_SRC_DIR}/device/usb/interfaces/IoPortInterface.cpp
        ${HHUOS_SRC_DIR}/device/usb/interfaces/LoggerInterface.cpp
        ${HHUOS_SRC_DIR}/device/usb/interfaces/PciDeviceInterface.cpp
        ${HHUOS_SRC_DIR}/device/usb/interfaces/PciInterface.cpp
        #${HHUOS_SRC_DIR}/device/usb/driver/UsbDriver.c
        ${HHUOS_SRC_DIR}/device/usb/driver/hid/MouseDriver.c
        ${HHUOS_SRC_DIR}/device/usb/driver/hid/KeyBoardDriver.c
        ${HHUOS_SRC_DIR}/device/usb/driver/storage/MassStorageDriver.c
        ${HHUOS_SRC_DIR}/device/usb/interfaces/MapInterface.cpp
        ${HHUOS_SRC_DIR}/device/usb/interfaces/TimeInterface.cpp
        ${HHUOS_SRC_DIR}/device/usb/interfaces/ThreadInterface.cpp
        ${HHUOS_SRC_DIR}/device/usb/interfaces/MutexInterface.cpp
        ${HHUOS_SRC_DIR}/device/usb/controller/UsbInterruptHandler.cpp
        ${HHUOS_SRC_DIR}/device/usb/events/EventDispatcher.c
        ${HHUOS_SRC_DIR}/device/usb/events/listeners/hid/KeyBoardListener.c
        ${HHUOS_SRC_DIR}/device/usb/events/listeners/hid/MouseListener.c
        ${HHUOS_SRC_DIR}/device/usb/events/listeners/storage/MassStorageListener.c
        ${HHUOS_SRC_DIR}/device/usb/events/listeners/EventListener.c
        ${HHUOS_SRC_DIR}/device/usb/include/UsbGeneral.c
        ${HHUOS_SRC_DIR}/device/usb/include/UsbInterface.cpp
        ${HHUOS_SRC_DIR}/device/usb/include/UsbErrors.c
        ${HHUOS_SRC_DIR}/device/usb/controller/UsbRunnable.cpp)