/*****************************************************************************
 *                                                                           *
 *                                 E H C I                                   *
 *                                                                           *
 *---------------------------------------------------------------------------*
 * Beschreibung:    Enhanced Host Controller Interface                       *
 *                                                                           *
 *                                                                           *
 * Autor:           Filip Krakowski, 07.11.2017                              *
 *****************************************************************************/

#ifndef __Ehci_include__
#define __Ehci_include__

#include "devices/Pci.h"
#include "devices/usb/Usb.h"
#include "devices/usb/UsbDevice.h"
#include "devices/usb/UsbEvent.h"
#include "devices/usb/ehci/AsyncListQueue.h"
#include "devices/usb/ehci/UsbMassStorage.h"
#include "kernel/memory/MemLayout.h"
#include "kernel/interrupts/InterruptHandler.h"
#include "kernel/KernelService.h"
#include "kernel/events/Receiver.h"
#include "kernel/services/EventBus.h"
#include "kernel/services/TimeService.h"

#include <cstdint>

#define EHCI_DEBUG 0
#define DEBUG_BIOS_QH 0
#define ALLOW_USB_EXCHANGE 0

#if EHCI_DEBUG
#define     EHCI_TRACE(...) printf("[EHCI] " __VA_ARGS__)
#else
#define     EHCI_TRACE(...)
#endif

/**
 * @author Filip Krakowski
 */
class Ehci : public InterruptHandler, public KernelService, public Receiver {

    typedef struct tagFR_LIST {
        uint32_t  entries[1024];
    } PeriodicFrameList;

    typedef volatile struct {
        /* 0x00 - CAPLENGTH */
        uint8_t     length;       // Capability Registers Length

        /* 0x01 - RESERVED */
        uint8_t     reserved;

        /* 0x02 - HCIVERSION */
        uint16_t    version;

        /* 0x04 - HCSPARAMS */
        uint32_t    hcsParams;

        /* 0x08 - HCCPARAMS */
        uint32_t    hccParams;

        /* 0x0C - HCSP-PORTROUTE */
        uint8_t     portRoute[8];   // Port Routes (15 nibbles, last one unused)

    } __attribute__((packed)) HostCap;

    typedef volatile struct {
        /* 0x00 - USBCMD */
        uint32_t        command;

        /* 0x04 - USBSTS */
        uint32_t        status;

        /* 0x08 - USBINTR */
        uint32_t        interrupt;

        /* 0x0C - FRINDEX */
        uint32_t        frameIndex;

        /* 0x10 - CTRLDSSEGMENT */
        uint32_t        ctrlDsSegment;

        /* 0x14 - PERIODICLISTBASE */
        uint32_t        periodicListBase;

        /* 0x18 - ASYNCLISTADDR */
        uint32_t        asyncListAddress;

        /* 0x1C - RESERVED */
        uint32_t        reserved[9];

        /* 0x40 - CONFIGFLAG */
        uint32_t        configFlag;

        /* 0x44 - PORTSTSCTL */
        uint32_t        ports[1];

    } __attribute__((packed)) HostOp;

public:

    enum EhciStatus { OK = 0, TIMEOUT = 1, ERROR = 2 };

    Ehci();

    /**
     * Sets up this host controller.
     *
     * @param dev the PciDevice representing this host controller
     */
    void setup(const Pci::Device &dev);

    void plugin();

    void trigger();

    /**
     * Indicates how many devices were found by the host controller.
     *
     * @return the number of devices detected by the host controller
     */
    uint32_t  getNumDevices();

    /**
     * Returns the mass storage device at the specified index.
     *
     * @param index array index
     * @return a mass storage device
     */
    UsbMassStorage *getDevice(uint32_t  index);

    /**
     * Prints all registers and ports belonging to this host controller.
     */
    void printSummary();

    void printPciStatus();

    static void printQueueHead(AsyncListQueue::QueueHead* queueHead);

    void onEvent(const Event &event) override;

    /* Line Status */
    const static uint8_t    PORTSC_LS_SE0               = 0x00;
    const static uint8_t    PORTSC_LS_KSTATE            = 0x01;
    const static uint8_t    PORTSC_LS_JSTATE            = 0x02;
    const static uint8_t    PORTSC_LS_UNDEFINED         = 0x03;

private:

    HostCap* cap;

    HostOp* op;

    Pci::Device pciDevice;

    PeriodicFrameList *frameList;

    AsyncListQueue *asyncListQueue;

    EventBus *eventBus;

    TimeService *timeService;

    Util::RingBuffer<UsbEvent> eventBuffer;

    uint8_t numPorts = 0;
    uint16_t version = 0;
    uint16_t frameListEntries = 0;
    uint8_t frameListSize = 0;

    bool transferCompleted = false;

    /**
     * Reads configuration data from the host controllers registers.
     */
    void readConfig();

    /**
     * Resets the host controller.
     *
     * @return a status indicating the result
     */
    Ehci::EhciStatus reset();

    /**
     * Performs a handoff, if supported by the BIOS and host controller.
     */
    void handoff();

    /**
     * Starts the host controller.
     */
    void start();

    /**
     * Stops the host controller.
     *
     * @return a status indicating the result
     */
    Ehci::EhciStatus stop();

    /**
     * Starts all ports associated with this host controller.
     */
    void startPorts();

    /**
     * Resets the specified port. This method will block for at least 100ms.
     *
     * @param port the port to reset
     */
    void resetPort(uint8_t portNumber);

    /**
     * Enables all interrupts.
     */
    void enableInterrupts();

    /**
     * Disables all interrupts.
     */
    void disableInterrupts();

    /**
     * Sets up the periodic schedule.
     */
    void setupPeriodicSchedule();

    /**
     * Sets up the asynchronous schedule.
     */
    void setupAsyncSchedule();

    /**
     * Enables the asynchronous schedule.
     */
    void enableAsyncSchedule();

    /**
     * Enables the periodic schedule.
     */
    void enablePeriodicSchedule();

    /**
     * Sets up a usb mass storage device.
     *
     * @param portNumber the devices port number
     */
    void setupUsbDevice(uint8_t portNumber);

    void acknowledgeAll();

    void onPortChangeDetected();

    static const uint8_t    ENABLED            = 0x1;
    static const uint8_t    DISABLED           = 0x0;

    static const uint8_t    QTD_STATUS_ERROR   = 0x40 - 1;

    LinkedList<UsbMassStorage> massStorageDevices;

    // HCSPARAMS - Structural Parameters //

    /* Number of Ports */
    static const uint32_t   HCSPARAMS_NP                = (0xF << 0);

    /* Port Power Control */
    static const uint32_t   HCSPARAMS_PPC               = (1 << 4);

    /* Port Routing Rules */
    static const uint32_t   HCSPARAMS_PRR               = (1 << 7);

    /* Number of Ports per Companion Controller */
    static const uint32_t   HCSPARAMS_NPCC              = (0xF << 8);

    /* Number of Companion Controllers */
    static const uint32_t   HCSPARAMS_NCC               = (0xF << 12);

    /* Port Indicators */
    static const uint32_t   HCSPARAMS_PI                = (1 << 16);

    /* Debug Port Number */
    static const uint32_t   HCSPARAMS_DPN               = (0xF << 20);

    // HCCPARAMS - Capability Parameters //

    /* 64-bit Addressing Capability */
    static const uint32_t   HCCPARAMS_ADDR              = (1 << 0);

    /* Programmable Frame List Flag */
    static const uint32_t   HCCPARAMS_PFLF              = (1 << 1);

    /* Asyncrhonous Schedule Park Capability */
    static const uint32_t   HCCPARAMS_ASPC              = (1 << 2);

    /* Isochronous Scheduling Threshold */
    static const uint32_t   HCCPARAMS_IST               = (0xF << 4);

    /* EHCI Extended Capabilities Pointer */
    static const uint32_t   HCCPARAMS_EECP              = (0xFF << 8);

    // USBCMD - USB Command Register //

    /* Run/Stop */
    static const uint32_t   USBCMD_RS                   = (1 << 0);

    /* Host Controller Reset */
    static const uint32_t   USBCMD_HCRESET              = (1 << 1);

    /* Frame List Size */
    static const uint32_t   USBCMD_FLS                  = (2 << 2);

    /* Periodic Schedule Enable */
    static const uint32_t   USBCMD_PSE                  = (1 << 4);

    /* Asynchronous Schedule Enable */
    static const uint32_t   USBCMD_ASE                  = (1 << 5);

    /* Interrupt on Async Advance Doorbell */
    static const uint32_t   USBCMD_IAAD                 = (1 << 6);

    /* Light Host Controller Reset */
    static const uint32_t   USBCMD_LHCRESET             = (1 << 7);

    /* Asynchronous Schedule Park Mode Count */
    static const uint32_t   USBCMD_ASPMC                = (2 << 8);

    /* Asynchronous Schedule Park Mode Enabled */
    static const uint32_t   USBCMD_ASPME                = (1 << 11);

    /* Interrupt Threshold Control */
    static const uint32_t   USBCMD_ITC                  = (0xFF << 16);

    // USBSTS - USB Status Register //

    /* USB Interrupt */
    static const uint32_t   USBSTS_USBINT               = (1 << 0);

    /* USB Error Interrupt */
    static const uint32_t   USBSTS_USBEINT              = (1 << 1);

    /* Port Change Detected */
    static const uint32_t   USBSTS_PCD                  = (1 << 2);

    /* Frame List Rollover */
    static const uint32_t   USBSTS_FLR                  = (1 << 3);

    /* Host System Error */
    static const uint32_t   USBSTS_HSE                  = (1 << 4);

    /* Interrupt on Async Advance */
    static const uint32_t   USBSTS_IAA                  = (1 << 5);

    /* Host Controller Halted */
    static const uint32_t   USBSTS_HCH                  = (1 << 12);

    /* Reclamation */
    static const uint32_t   USBSTS_RECL                 = (1 << 13);

    /* Periodic Schedule Status */
    static const uint32_t   USBSTS_PSS                  = (1 << 14);

    /* Asynchronous Schedule Status */
    static const uint32_t   USBSTS_ASS                  = (1 << 15);

    // USBINTR - USB Interrupt Enable Register //

    /* USB Interrupt */
    static const uint32_t   USBINTR_USBINT              = (1 << 0);

    /* USB Error Interrupt */
    static const uint32_t   USBINTR_USBEINT             = (1 << 1);

    /* Port Change Detected */
    static const uint32_t   USBINTR_PCD                 = (1 << 2);

    /* Frame List Rollover */
    static const uint32_t   USBINTR_FLR                 = (1 << 3);

    /* Host System Error */
    static const uint32_t   USBINTR_HSE                 = (1 << 4);

    /* Interrupt on Async Advance */
    static const uint32_t   USBINTR_IAA                 = (1 << 5);

    // PORTSC - Port Status and Control Register //

    /* Current Connect Status */
    static const uint32_t   PORTSC_CCS                  = (1 << 0);

    /* Connect Status Changed */
    static const uint32_t   PORTSC_CSC                  = (1 << 1);

    /* Port Enabled */
    static const uint32_t   PORTSC_PE                   = (1 << 2);

    /* Port Enable Change */
    static const uint32_t   PORTSC_PEC                  = (1 << 3);

    /* Over-current Active */
    static const uint32_t   PORTSC_OCA                  = (1 << 4);

    /* Over-current Active Changed */
    static const uint32_t   PORTSC_OCC                  = (1 << 5);

    /* Force Port Resume */
    static const uint32_t   PORTSC_FPR                  = (1 << 6);

    /* Suspend */
    static const uint32_t   PORTSC_SPD                  = (1 << 7);

    /* Port Reset */
    static const uint32_t   PORTSC_PR                   = (1 << 8);

    /* Line Status */
    static const uint32_t   PORTSC_LS                   = (2 << 10);

    /* Port Power */
    static const uint32_t   PORTSC_PP                   = (1 << 12);

    /* Port Owner */
    static const uint32_t   PORTSC_PO                   = (1 << 13);

    /* Port Indicator Control */
    static const uint32_t   PORTSC_PIC                  = (1 << 14);

    /* Port Test Control */
    static const uint32_t   PORTSC_PTC                  = (1 << 16);

    /* Wake on Connect Enable */
    static const uint32_t   PORTSC_WCE                  = (1 << 20);

    /* Wake on Disconnect Enable */
    static const uint32_t   PORTSC_WDE                  = (1 << 21);

    /* Wake on Over-current Enable */
    static const uint32_t   PORTSC_WOCE                 = (1 << 22);
};

#endif
