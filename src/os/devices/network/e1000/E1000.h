/*
 * Copyright (C) 2018/19 Thiemo Urselmann
 * Heinrich-Heine University
 *
 * This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public
 * License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any
 * later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied
 * warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>
 *
 * Note:
 * All references marked with [...] refer to the following developers manual.
 * Intel Corporation. PCI/PCI-X Family of Gigabit Ethernet Controllers Software Developer’s Manual.
 * 317453006EN.PDF Revision 4.0. 2009.
 */

#ifndef HHUOS_E1000_H
#define HHUOS_E1000_H


#include <devices/pci/PciDeviceDriver.h>
#include <kernel/services/EventBus.h>
#include "Mac.h"
#include "general/DeviceControl.h"
#include "transmit/TransmitControl.h"
#include "receive/ReceiveControl.h"
#include "interrupts/InterruptEnable.h"
#include "interrupts/InterruptDisable.h"
#include "transmit/TransmitRing.h"
#include "interrupts/InterruptCause.h"
#include "receive/ReceiveRing.h"
#include "transmit/descriptor/legacy/TransmitDescriptor.h"
#include "general/Descriptors.h"
#include "receive/descriptor/ReceiveDescriptor.h"
#include <kernel/Kernel.h>
#include <kernel/memory/Paging.h>
#include <devices/network/NetworkDevice.h>

/**
 * This abstract class extends and the InterruptHandler interface
 * it also extends the abstract class PciDeviceDriver and
 * implements it partially.
 *
 * This is the core of all E1000 drivers. It provides a standardized
 * method named initialize which can be called from extending
 * drivers.
 *
 * All driver specific methods called from within
 * this method have to be overridden from implementing drivers
 * if they decide to use the initialize method for setting up
 * its card. If they don't want to, they can implement their
 * own setup method without calling initialize on E1000.
 */
class E1000 : public NetworkDevice, public PciDeviceDriver {
private:
    /**
     * A logger to provide logging information on the kernel log.
     */
    Logger &log = Logger::get("E1000");

    EventBus *eventBus = nullptr;

public:
    ~E1000() override = default;

    E1000(E1000 const &) = delete;
    E1000 &operator=(E1000 const &) = delete;

    /**
     * The MAC-address of the corresponding Ethernet Card.
     * Outside classes should use this to write the MAC
     * source address to the packet they want to send, prior
     * calling the send method of the transmit ring.
     */
    Mac *mac = nullptr;

    /**
     * Classes can send packets via this class.
     */
    TransmitRing *transmitter = nullptr;

    /**
     * Writes 0's at a given amount of bytes to the region pointed to
     * bei address.
     * @param address A buffer to be cleared.
     * @param bytes Amount of bytes to be cleared.
     */
    void clearBufferExcerpt(uint8_t *address, uint32_t bytes);

    /**
     * Overriding function from NetworkDevice.
     */
    void sendPacket(void *address, uint16_t length) override;

    /**
     * Overriding function from NetworkDevice.
     */
    void getMacAddress(uint8_t *buf) override;

    bool hasInterruptData() override;

protected:
    E1000();

    /**
     * Provides sizes of the receive buffers of the
     * receive FIFO from the ethernet card.
     */
    enum rxBufferSize {
        small = 256,
        medium = 512,
        big = 1024,
        tall = 2048,
        ext_small = 4096,
        ext_medium = 8192,
        ext_big = 16384
    };

    /**
     * Provides minimum thresholds for emtpy
     * descriptors of the descriptor ring inside
     * the ethernet card.
     */
    enum rxRingLength {
        half = 0,
        quarter = 1,
        eighth = 2
    };

    /**
     * Size of one page from hhuOS.
     */
    const uint32_t pageSize = PAGESIZE;

    /**
     * The size of the MMIO-region mapped from
     * the ethernet card.
     */
    const uint32_t mmioSize = 0x20000;

    /**
     * The size of a descriptor is 16 bytes.
     */
    const uint32_t descriptorBytes = 16;

    /**
     * The amount of descriptors in a set is 8.
     */
    const uint16_t descriptorSet = 8;

    /**
     * Amount of transmit descriptors.
     */
    uint16_t transmitDescriptors = 0;

    /**
     * Amount of receive descriptors.
     */
    uint16_t receiveDescriptors = 0;

    /**
     * Size of the transmit descriptor block.
     */
    uint32_t transmitBlockSize = 0;

    /**
     * Size of the receive descriptor block.
     */
    uint32_t receiveBlockSize = 0;

    /**
     * The base address of the MMIO-space.
     */
    uint8_t * mmioBase = nullptr;

    /**
     * Buffer to store address and length of received packets.
     * These will be processed in the top half interrupt handler.
     */
    Util::RingBuffer<Util::Pair<void*, uint16_t>> interruptBuffer;

    /**
     * The main control options can be handled with this class..
     */
    DeviceControl *deviceControl = nullptr;

    /**
     * The receive control options can be handled with this class.
     */
    ReceiveControl *receive = nullptr;

    /**
     * With this class it is possible to enable interrupts.
     */
    InterruptEnable *interruptEnable = nullptr;

    /**
     * With this class one can block interrupts.
     */
    InterruptDisable *interruptDisable = nullptr;

    /**
     * The transmit control options can be handled with this class.
     */
    TransmitControl *transmit = nullptr;

    /**
     * Encapsulates processing interrupts.
     */
    InterruptCause *interruptCause = nullptr;

    /**
     * This class processes pending packets and sends them
     * via an event bus.
     */
    ReceiveRing *receiver = nullptr;

    /**
     * Depending on the sizes of the sets, descriptor block sizes will be set.
     * @param receiveSets Amount of sets of receive descriptors.
     * @param transmitSets Amount of sets of the transmit descriptors.
     */
    void setUpDescriptorParams(uint16_t receiveSets, uint16_t transmitSets);

    /**
     * Calculates the size of a descriptor block depending
     * on the amount of descriptors.
     * @param descriptors Amount of descriptors inside a block.
     * @return The corresponding size of the block.
     */
    uint32_t calculateBlockSize(uint32_t descriptors);

    /**
     * Initializes the extending driver in a standardized manner.
     * Usually called from the setup-method of the extending
     * driver (short driver).
     * @param dev Device struct of the driver.
     * @param log Logger of the driver.
     * @param driver Reference to the driver.
     */
    void initialize(const Pci::Device &dev, Logger &log, E1000 *driver);


    /**
     * Inherited methods from PciDeviceDriver.
     * This methods are meant to be overridden and
     * implemented by this class.
     */

    /**
     * Fetches the pci device class id.
     * @return NetworkController: 0x02
     */
    uint8_t getBaseClass() const final;

    /**
     * Fetches the pci device subclass id.
     * @return Ethernet Controller: 0x00
     */
    uint8_t getSubClass() const final;

    /**
     * Determines how to find the right setup method for the
     * registered ethernet card. The relevant ethernet cards for
     * the E1000 are unique by a triple of id's (Class, Vendor, Device).
     * Those of the registered Hardware devices are compared to
     * that ones given by drivers. Don't use byClass since it is not
     * unique.
     * @return Type that determines how find the driver (has to be unique).
     */
    PciDeviceDriver::SetupMethod getSetupMethod() const final;


    /**
     * Inherited methods from PciDeviceDriver.
     * This methods are meant to be overridden and
     * implemented by extending this class.
     */

    /**
     * This method is for connecting this driver with the corresponding Ethernet card,
     * due pci-driver initialization and calling the setup-method on it.
     * @return Array of (vendor-ID, device-ID) pairs.
     */
    Util::Array<Util::Pair<uint16_t, uint16_t>> getIdPairs () const override = 0;

    /**
     * Sets the ethernet card up, i.e. it initializes it and makes
     * it ready for usage of other classes. Usually it calls the initialize
     * method of E1000 to let it process its standardized procedure of
     * setting up the card.
     * @param dev A struct that stores main information read from pci registers
     * of the ethernet card.
     */
    void setup(const Pci::Device &dev) override = 0;



    /**
     * Inherited method from InterruptHandler.
     * This method is meant to be overridden and
     * implemented by extending this class.
     */

    /**
     * This method handles interrupts asserted by the corresponding
     * ethernet card.
     * @param frame Interrupt-types (no need to inspect in this driver
     * because there is the interrupt cause read register of the card, where
     * specific asserted interrupts should be read from)
     */
    void trigger(InterruptFrame &frame) override = 0;

    void parseInterruptData() final;

    /**
     * The following methods are meant to be
     * overridden and implemented by extending drivers.
     */

    /**
     * Makes the driver capable of detecting interrupts asserted
     * by the ethernet card.
     * The appropriate trigger method will be called, if an
     * interrupt of the connected ethernet card is asserted.
     */
    virtual void plugin() = 0;

    /**
     * Sets up all general device specific options.
     * @param control That class where the controls should be applied to.
     */
    virtual void setDeviceControl(DeviceControl *control) = 0;

    /**
     * Sets up specific options according to transmit.
     * @param control That class where the controls should be applied to.
     */
    virtual void setTransmitControl(TransmitControl *control) = 0;

    /**
     * Sets up specific options according to receive.
     * @param control That class where the controls should be applied to.
     */
    virtual void setReceiveControl(ReceiveControl *control) = 0;

    /**
     * Sets up specific options according to receiving interrupts.
     * @param enable Interrupts to enable.
     * @param disable Interrupts to disable.
     */
    virtual void setInterruptControl(InterruptEnable *enable, InterruptDisable *disable) = 0;

    /**
     * Initializes a transmit descriptor block for a descriptor ring.
     * @param buffer An allocated buffer to store descriptors into.
     * @return A class for handling the prepared descriptor block.
     */
    virtual Descriptors<TransmitDescriptor *> * createTransmitDescriptorBlock(uint8_t *buffer) = 0;

    /**
     * Initializes a receive descriptor block for a descriptor ring.
     * @param buffer An allocated buffer to store descriptors into.
     * @return A class for handling the prepared descriptor block.
     */
    virtual Descriptors<ReceiveDescriptor *> *createReceiveDescriptorBlock(uint8_t *buffer) = 0;


    /**
     * This method initializes all attributes of a non-primitive type.
     * @param transmitDescriptors A transmit descriptor block handler.
     * @param receiveDescriptors A receive descriptor block handler.
     * @param phyTransmitBlock  The physical address of the transmit descriptor block.
     * @param phyReceiveBlock  The physical address of the receive descriptor block.
     */
    virtual void initializeAttributeClasses(Descriptors<TransmitDescriptor *> *transmitDescriptors,
                                            Descriptors<ReceiveDescriptor *> *receiveDescriptors,
                                            uint64_t phyTransmitBlock,
                                            uint64_t phyReceiveBlock) = 0;

    /**
     * Loads the MAC-address into the Mac-Attribute.
     * @return
     */
    virtual void loadMac() = 0;

    /**
     * Creates virtual nodes, which can be accessed from the Shell.
     */
    virtual void createNodes() = 0;
};


#endif //HHUOS_E1000_H
