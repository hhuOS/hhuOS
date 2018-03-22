/*
 * Copyright (C) 2018  Filip Krakowski
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <kernel/services/DebugService.h>
#include <kernel/Kernel.h>
#include "UsbMassStorage.h"

#include "UsbMassStorage.h"

extern "C" {
    #include "lib/libc/string.h"
}

UsbMassStorage::UsbMassStorage(AsyncListQueue::QueueHead *control, uint8_t portNumber) : UsbDevice(control, portNumber) {
    init();
}

void UsbMassStorage::init() {

    debugService = Kernel::getService<DebugService>();

    UsbEndpoint *endpointIn = findEndpoint(UsbEndpoint::TransferType::BULK, UsbEndpoint::Direction::IN);
    UsbEndpoint *endpointOut = findEndpoint(UsbEndpoint::TransferType::BULK, UsbEndpoint::Direction::OUT);

    bulkInToggle = false;
    bulkOutToggle = false;

    bulkIn = AsyncListQueue::createQueueHead(false, getAddress(), endpointIn->getAddress(),
                                             endpointIn->getMaxPacketSize(), 0x1, 0x2, true);
    bulkOut = AsyncListQueue::createQueueHead(false, getAddress(), endpointOut->getAddress(),
                                              endpointOut->getMaxPacketSize(), 0x1, 0x2, true);

    bulkOut->link = (uint32_t) bulkIn | 0x2;
    bulkIn->link = control->link;
    control->link = (uint32_t) bulkOut | 0x2;

    bulkReset(0x0);

    // maxLun = getMaxLun(0x0);

    inquiryData = getInquiryData();

    testUnityReady();

    getCapacity();
}

UsbMassStorage::CommandBlockWrapper UsbMassStorage::getScsiInquiry() {
    CommandBlockWrapper cbw;
    memset(&cbw, 0, sizeof(CommandBlockWrapper));

    cbw.signature = 0x43425355;
    cbw.tag = 0x59525149;
    cbw.length = 36;
    cbw.direction = 0x80;
    cbw.cmdLength = 6;
    cbw.unit = 0;

    Scsi::Command *command = Scsi::Command::inquiry();

    memcpy(cbw.cmdData, command->data, 16);

    delete command;

    return cbw;
}

UsbMassStorage::CommandBlockWrapper UsbMassStorage::testUnitReady() {
    CommandBlockWrapper cbw;
    memset(&cbw, 0, sizeof(CommandBlockWrapper));

    cbw.signature = 0x43425355;
    cbw.tag = 0x59445255;
    cbw.direction = 0x00;
    cbw.cmdLength = 6;

    Scsi::Command *command = Scsi::Command::testUnitReady();

    memcpy(cbw.cmdData, command->data, 16);

    delete command;

    return cbw;
}

UsbMassStorage::CommandBlockWrapper UsbMassStorage::readCapacity() {
    CommandBlockWrapper cbw;
    memset(&cbw, 0, sizeof(CommandBlockWrapper));

    cbw.signature = 0x43425355;
    cbw.tag = 0x50414352;
    cbw.length = 32;
    cbw.direction = 0x80;
    cbw.cmdLength = 16;

    Scsi::Command *command = Scsi::Command::readCapacity();

    memcpy(cbw.cmdData, command->data, 16);

    delete command;

    return cbw;
}

UsbMassStorage::CommandBlockWrapper UsbMassStorage::read(uint32_t blockHigh, uint32_t blockLow, uint32_t length) {
    CommandBlockWrapper cbw;
    memset(&cbw, 0, sizeof(CommandBlockWrapper));

    cbw.signature = 0x43425355;
    cbw.tag = 0x44414552;
    cbw.length = length * 512;
    cbw.direction = 0x80;
    cbw.cmdLength = 10;

    Scsi::Command *command = Scsi::Command::read(blockHigh, blockLow, length);

    memcpy(cbw.cmdData, command->data, 16);

    delete command;

    return cbw;
}

UsbMassStorage::CommandBlockWrapper UsbMassStorage::write(uint32_t blockHigh, uint32_t blockLow, uint32_t length) {
    CommandBlockWrapper cbw;
    memset(&cbw, 0, sizeof(CommandBlockWrapper));

    cbw.signature = 0x43425355;
    cbw.tag = 0x54495257;
    cbw.length = length * 512;
    cbw.direction = 0x00;
    cbw.cmdLength = 10;

    Scsi::Command *command = Scsi::Command::write(blockHigh, blockLow, length);

    memcpy(cbw.cmdData, command->data, 16);

    delete command;

    return cbw;
}

UsbMassStorage::CommandBlockWrapper UsbMassStorage::requestSense() {
    CommandBlockWrapper cbw;
    memset(&cbw, 0, sizeof(CommandBlockWrapper));

    cbw.signature = 0x43425355;
    cbw.tag = 0x534E4553;
    cbw.length = 18;
    cbw.direction = 0x80;
    cbw.cmdLength = 6;

    Scsi::Command *command = Scsi::Command::requestSense();

    memcpy(cbw.cmdData, command->data, 16);

    delete command;

    return cbw;
}

Scsi::InquiryData UsbMassStorage::getInquiryData() {

    USB_TRACE("Requesting inquiry data from %s %s\n", manufacturer, product);

    UsbMassStorage::CommandBlockWrapper command = getScsiInquiry();

    UsbTransaction *transaction = new UsbTransaction();
    transaction->add( AsyncListQueue::createOutTD(31, bulkOutToggle, (uint32_t*) &command));
    bulkOutToggle = !bulkOutToggle;

    issueTransaction(bulkOut, transaction);

    uint32_t bufferAddr;
    uint32_t statusAddr;
    transaction = new UsbTransaction();
    transaction->add( AsyncListQueue::createInTD(36, bulkInToggle, &bufferAddr));
    bulkInToggle = !bulkInToggle;
    transaction->add( AsyncListQueue::createInTD(13, bulkInToggle, &statusAddr));
    bulkInToggle = !bulkInToggle;

    issueTransaction(bulkIn, transaction);

    Scsi::InquiryData ret = *(Scsi::InquiryData*) bufferAddr;

    delete transaction;

    memcpy(scsiVendor, ret.vendorId, 8);
    scsiVendor[8] = '\0';

    memcpy(scsiProduct, ret.productId, 16);
    scsiProduct[16] = '\0';

    memcpy(scsiRevision, ret.productRevision, 4);
    scsiRevision[4] = '\0';

    USB_TRACE("|--------------------------------------------------------------|\n");
    USB_TRACE("| Inquiry Data\n");
    USB_TRACE("|--------------------------------------------------------------|\n");
    USB_TRACE("| Device Type:                    %x\n", ret.deviceType);
    USB_TRACE("| Device Qualifier:               %x\n", ret.deviceQualifier);
    USB_TRACE("| Removable:                      %d\n", ret.removable);
    USB_TRACE("| Version:                        %x\n", ret.version);
    USB_TRACE("| Vendor:                         %s\n", scsiVendor);
    USB_TRACE("| Product:                        %s\n", scsiProduct);
    USB_TRACE("| Revision:                       %s\n", scsiRevision);
    USB_TRACE("|--------------------------------------------------------------|\n");
    USB_TRACE("\n");

    return ret;
}

UsbDevice::Status UsbMassStorage::bulkReset(uint16_t interface) {

    USB_TRACE("Performing bulk reset on %s %s\n", manufacturer, product);

    Usb::Request request = Usb::bulkReset(interface);

    UsbTransaction *transaction = new UsbTransaction();
    transaction->add( AsyncListQueue::createSetupTD((uint32_t*) &request) );
    transaction->add( AsyncListQueue::createInTD(0, true, nullptr) );

    issueTransaction(control, transaction);

    delete transaction;

    return OK;
}

uint8_t UsbMassStorage::getMaxLun(uint16_t interface) {
    uint32_t bufferAddr;
    Usb::Request request = Usb::getMaxLun(interface);

    UsbTransaction *transaction = new UsbTransaction();
    transaction->add( AsyncListQueue::createSetupTD((uint32_t*) &request) );
    transaction->add( AsyncListQueue::createInTD(1, true, &bufferAddr) );

    issueTransaction(control, transaction);

    uint8_t ret = *(uint8_t*) bufferAddr;

    delete transaction;

    return ret;
}

UsbDevice::Status UsbMassStorage::testUnityReady() {
    UsbMassStorage::CommandBlockWrapper command = testUnitReady();

    UsbTransaction *transaction = new UsbTransaction();
    transaction->add( AsyncListQueue::createOutTD(31, bulkOutToggle, (uint32_t*) &command));
    bulkOutToggle = !bulkOutToggle;

    issueTransaction(bulkOut, transaction);

    uint32_t statusAddr;
    transaction = new UsbTransaction();
    transaction->add( AsyncListQueue::createInTD(13, bulkInToggle, &statusAddr));
    bulkInToggle = !bulkInToggle;

    issueTransaction(bulkIn, transaction);

    delete transaction;

    return OK;
}

UsbDevice::Status UsbMassStorage::getCapacity() {
    UsbMassStorage::CommandBlockWrapper command = readCapacity();

    UsbTransaction *transaction = new UsbTransaction();
    transaction->add( AsyncListQueue::createOutTD(31, bulkOutToggle, (uint32_t*) &command));
    bulkOutToggle = !bulkOutToggle;

    issueTransaction(bulkOut, transaction);

    uint32_t bufferAddr;
    uint32_t statusAddr;
    transaction = new UsbTransaction();
    transaction->add( AsyncListQueue::createInTD(32, bulkInToggle, &bufferAddr));
    bulkInToggle = !bulkInToggle;
    transaction->add( AsyncListQueue::createInTD(13, bulkInToggle, &statusAddr));
    bulkInToggle = !bulkInToggle;

    issueTransaction(bulkIn, transaction);

    delete transaction;

    return OK;
}

UsbDevice::Status UsbMassStorage::readData(uint32_t blockHigh, uint32_t blockLow, uint32_t blocks, uint8_t *buffer) {
    UsbMassStorage::CommandBlockWrapper command = read(blockHigh, blockLow, blocks);

    UsbTransaction *transaction = new UsbTransaction();
    transaction->add( AsyncListQueue::createOutTD(31, bulkOutToggle, (uint32_t*) &command));
    bulkOutToggle = !bulkOutToggle;

    issueTransaction(bulkOut, transaction);

    delete transaction;

    uint32_t bufferAddr;
    uint32_t statusAddr;
    transaction = new UsbTransaction();
    transaction->add( AsyncListQueue::createInTD(blocks * 512, bulkInToggle, &bufferAddr));
    bulkInToggle = !bulkInToggle;
    transaction->add( AsyncListQueue::createInTD(13, bulkInToggle, &statusAddr));
    bulkInToggle = !bulkInToggle;

    issueTransaction(bulkIn, transaction);

    memcpy(buffer, (uint8_t*) bufferAddr, blocks * 512);

    delete transaction;

    return OK;
}

UsbDevice::Status UsbMassStorage::writeData(uint32_t blockHigh, uint32_t blockLow, uint32_t blocks, uint8_t *buffer) {
    UsbMassStorage::CommandBlockWrapper command = write(blockHigh, blockLow, blocks);

    UsbTransaction *transaction = new UsbTransaction();
    transaction->add( AsyncListQueue::createOutTD(31, bulkOutToggle, (uint32_t*) &command));
    bulkOutToggle = !bulkOutToggle;
    transaction->add( AsyncListQueue::createOutTD(blocks * 512, bulkOutToggle, (uint32_t*) buffer));
    bulkOutToggle = !bulkOutToggle;

    issueTransaction(bulkOut, transaction);

    delete transaction;

    uint32_t statusAddr;
    transaction = new UsbTransaction();
    transaction->add( AsyncListQueue::createInTD(13, bulkInToggle, &statusAddr));
    bulkInToggle = !bulkInToggle;

    issueTransaction(bulkIn, transaction);

    delete transaction;

    return OK;
}

UsbDevice::Status UsbMassStorage::getSense() {
    UsbMassStorage::CommandBlockWrapper command = requestSense();

    UsbTransaction *transaction = new UsbTransaction();
    transaction->add( AsyncListQueue::createOutTD(31, bulkOutToggle, (uint32_t*) &command));
    bulkOutToggle = !bulkOutToggle;

    issueTransaction(bulkOut, transaction);

    uint32_t bufferAddr;
    uint32_t statusAddr;
    transaction = new UsbTransaction();
    transaction->add( AsyncListQueue::createInTD(18, bulkInToggle, &bufferAddr));
    bulkInToggle = !bulkInToggle;
    transaction->add( AsyncListQueue::createInTD(13, bulkInToggle, &statusAddr));
    bulkInToggle = !bulkInToggle;

    issueTransaction(bulkIn, transaction);

    return OK;
}

void UsbMassStorage::printCommandBlockWrapper(UsbMassStorage::CommandBlockWrapper *commandBlockWrapper) {
    USB_TRACE("|--------------------------------------------------------------|\n");
    USB_TRACE("| Command Block Wrapper @ %x\n", commandBlockWrapper);
    USB_TRACE("|--------------------------------------------------------------|\n");
    USB_TRACE("| Signature:                      %08x\n", commandBlockWrapper->signature);
    USB_TRACE("| Tag:                            %08x\n", commandBlockWrapper->tag);
    USB_TRACE("| Transfer Length:                %d\n", commandBlockWrapper->length);
    USB_TRACE("| Direction:                      %08x\n", commandBlockWrapper->direction);
    USB_TRACE("| Logical Unit:                   %08x\n", commandBlockWrapper->unit);
    USB_TRACE("| Command Length:                 %d\n", commandBlockWrapper->cmdLength);
    USB_TRACE("|--------------------------------------------------------------|\n");
    USB_TRACE("| Command Data                   \n");
    USB_TRACE("|--------------------------------------------------------------|\n");

    for (uint8_t i = 0; i < 4; i++) {
        USB_TRACE("|  %02x  %02x  %02x  %02x\n",
                  commandBlockWrapper->cmdData[i * 4 + 0],
                  commandBlockWrapper->cmdData[i * 4 + 1],
                  commandBlockWrapper->cmdData[i * 4 + 2],
                  commandBlockWrapper->cmdData[i * 4 + 3]);
    }

    USB_TRACE("|--------------------------------------------------------------|\n");
    USB_TRACE("\n");
}

void UsbMassStorage::printCommandStatusWrapper(UsbMassStorage::CommandStatusWrapper *commandStatusWrapper) {
    USB_TRACE("|--------------------------------------------------------------|\n");
    USB_TRACE("| Command Staus Wrapper @ %x\n", commandStatusWrapper);
    USB_TRACE("|--------------------------------------------------------------|\n");
    USB_TRACE("| Signature:                      %08x\n", commandStatusWrapper->signature);
    USB_TRACE("| Tag:                            %08x\n", commandStatusWrapper->tag);
    USB_TRACE("| Data Residue:                   %d\n", commandStatusWrapper->dataResidue);
    USB_TRACE("| Status:                         %08x\n", commandStatusWrapper->status);
    USB_TRACE("|--------------------------------------------------------------|\n");
    USB_TRACE("\n");
}


