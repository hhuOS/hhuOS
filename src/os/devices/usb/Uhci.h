#ifndef HHUOS_UHCI_H
#define HHUOS_UHCI_H


#include <devices/PciDeviceDriver.h>
#include <kernel/services/TimeService.h>
#include <cstdint>

class Uhci : public PciDeviceDriver {

private:

    enum IORegisterOffset : uint8_t {
        USB_COMMAND_REGISTER = 0x00,
        USB_STATUS_REGISTER = 0x02,
        USB_INTERRUPT_REGISTER = 0x04,
        FRAME_NUMBER_REGISTER = 0x06,
        FRAME_LIST_BASE_ADDRESS_REGISTER = 0x08,
        START_OF_FRAME_MODIFY_REGISTER = 0x0c,
        PORT_1_STATUS_CONTROL_REGISTER = 0x10,
        PORT_2_STATUS_CONTROL_REGISTER = 0x12
    };

    enum UsbCommand : uint16_t {
        RUN_STOP = 0x01,
        HOST_CONTROLLER_RESET = 0x02,
        GLOBAL_RESET = 0x04,
        ENTER_GLOBAL_SUSPEND_MODE = 0x08,
        FORCE_GLOBAL_RESUME = 0x10,
        SOFTWARE_DEBUG = 0x20,
        CONFIGURE_FLAG = 0x40,
        MAX_PACKET = 0x80
    };

    enum UsbStatus : uint16_t {
        USB_INTERRUPT = 0x01,
        USB_ERROR_INTERRUPT = 0x02,
        HOST_CONTROLLER_RESUME_DETECT = 0x04,
        HOST_SYSTEM_ERROR = 0x08,
        HOST_CONTROLLER_PROCESS_ERROR = 0x10,
        HOST_CONTROLLER_HALTED = 0x20
    };

    enum UsbInterrupt : uint16_t {
        TIMEOUT_CRC_INTERRUPT = 0x01,
        RESUME_INTERRUPT = 0x02,
        INTERRUPT_ON_COMPLETE = 0x04,
        SHORT_PACKET_INTERRUPT = 0x08
    };

    enum UsbPortControlStatus : uint16_t {
        CURRENT_CONNECT_STATUS = 0x01,
        CONNECT_STATUS_CHANGE = 0x02,
        PORT_ENABLED_DISABLED = 0x04,
        PORT_ENABLED_DISABLED_CHANGE = 0x08,
        LINE_STATUS = 0x30,
        PORT_RESUME_DETECT = 0x40,
        LOW_SPEED_DEVICE_ATTACHED = 0x100,
        PORT_RESET = 0x200,
        SUSPEND = 0x1000
    };

private:

    struct FrameListPointer {
        uint32_t terminate : 1;
        uint32_t qhTdSelect : 1;
        uint32_t : 2;
        uint32_t frameListPointer : 28;
    }__attribute__((packed));

    struct QueueHeadLinkPointer {
        uint32_t terminate : 1;
        uint32_t qhTdSelect : 1;
        uint32_t : 2;
        uint32_t queueHeadLinkPointer : 28;
    }__attribute__((packed));

    struct QueueElementLinkPointer {
        uint32_t terminate : 1;
        uint32_t qhTdSelect : 1;
        uint32_t : 2;
        uint32_t queueElementLinkPointer : 28;
    }__attribute__((packed));

    struct TransferDescriptorLinkPointer {
        uint32_t terminate : 1;
        uint32_t qhTdSelect : 1;
        uint32_t depthBreadthSelect : 1;
        uint32_t : 1;
        uint32_t linkPointer : 28;
    }__attribute__((packed));

    struct TransferDescriptorControlStatus {
        uint32_t actualLength : 11;
        uint32_t : 5;
        uint32_t : 1;
        uint32_t bitstuffError : 1;
        uint32_t crcTimeoutError : 1;
        uint32_t nakReceived : 1;
        uint32_t babbleDetected : 1;
        uint32_t dataBufferError : 1;
        uint32_t stalled : 1;
        uint32_t active : 1;
        uint32_t interruptOnComplete : 1;
        uint32_t isochronousSelect : 1;
        uint32_t lowSpeedDevice : 1;
        uint32_t errorLimit : 2;
        uint32_t shortPacketDetected : 1;
        uint32_t : 2;
    }__attribute__((packed));

    struct TransferDescriptorControlToken {
        uint32_t packetIdentification : 8;
        uint32_t deviceAddress : 7;
        uint32_t endpoint : 4;
        uint32_t dataToggle : 1;
        uint32_t : 1;
        uint32_t maximumLength : 11;
    }__attribute__((packed));

    struct TransferDescriptorControlEndPointer {
        uint32_t bufferPointer : 32;
    }__attribute__((packed));

private:

    struct QueueHead {
        QueueHeadLinkPointer headLinkPointer;
        QueueElementLinkPointer elementLinkPointer;
    };

    struct TransferDescriptor {
        TransferDescriptorLinkPointer linkPointer;
        TransferDescriptorControlStatus controlStatus;
        TransferDescriptorControlToken token;
        TransferDescriptorControlEndPointer endPointer;

        uint32_t reserved1;
        uint32_t reserved2;
        uint32_t reserved3;
        uint32_t reserved4;
    }__attribute__((packed));

private:

    static Logger &log;

    TimeService *timeService = nullptr;

    IOport *usbCommandPort = nullptr;
    IOport *usbStatusPort = nullptr;
    IOport *usbInterruptEnablePort = nullptr;
    IOport *frameNumberPort = nullptr;
    IOport *frameListBaseAddressPort = nullptr;
    IOport *startOfFrameModifyPort = nullptr;
    IOport *statusControlPort1 = nullptr;
    IOport *statusControlPort2 = nullptr;

    FrameListPointer *frameList = nullptr;

public:

    Uhci() = default;

    ~Uhci() override;

    void resetHostController();

    void startHostController();

    void stopHostController();

    void disableInterrupts();

    void enableInterrupt(UsbInterrupt interrupt);

    void resetPort(uint8_t portNum);

    uint8_t getBaseClass() const override {
        return Pci::CLASS_SERIAL_BUS_DEVICE;
    };

    uint8_t getSubClass() const override {
        return Pci::SUBCLASS_USB;
    };

    uint8_t getProgramInterface() const override {
        return Pci::PROGIF_UHCI;
    }

    PciDeviceDriver::SetupMethod getSetupMethod() const override {
        return PciDeviceDriver::BY_PROGRAM_INTERFACE;
    }

    void setup(const Pci::Device &device) override;

    PCI_DEVICE_DRIVER_IMPLEMENT_CREATE_INSTANCE(Uhci);
};


#endif
