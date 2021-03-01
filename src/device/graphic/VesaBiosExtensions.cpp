#include <device/bios/Bios.h>
#include <kernel/memory/MemLayout.h>
#include <kernel/core/Management.h>
#include "VesaBiosExtensions.h"

namespace Device::Graphic {

const Util::Memory::Address<uint32_t> VesaBiosExtensions::BIOS_CALL_RETURN_DATA_PHYSICAL_ADDRESS(PHYS_BIOS_RETURN_MEM);
const Util::Memory::Address<uint32_t> VesaBiosExtensions::BIOS_CALL_RETURN_DATA_VIRTUAL_ADDRESS(BIOS_CALL_RETURN_DATA_PHYSICAL_ADDRESS.get() + KERNEL_START);

VesaBiosExtensions::VesaBiosExtensions() {
    auto vbeInfo = getVbeInfo();

    // Get vendor name, device name and memory size
    const char *vendorNameAddress = reinterpret_cast<const char*>(PHYS2VIRT((vbeInfo->vendor[1] << 4) + vbeInfo->vendor[0]));
    const char *deviceNameAddress = reinterpret_cast<const char*>(PHYS2VIRT((vbeInfo->product_name[1] << 4) + vbeInfo->product_name[0]));

    vendorName = vendorNameAddress == nullptr ? LinearFrameBufferProvider::getVendorName() : vendorNameAddress;
    deviceName = deviceNameAddress == nullptr ? LinearFrameBufferProvider::getDeviceName() : deviceNameAddress;
    memorySize = vbeInfo->video_memory * 65536;

    // Get available resolutions
    auto modePointer = reinterpret_cast<uint16_t*>(((vbeInfo->video_modes[1] << 4) + vbeInfo->video_modes[0]) + KERNEL_START);

    // Calculate amount of modes
    uint32_t modeCount;
    for (modeCount = 0; modePointer[modeCount] != MODE_LIST_END_MARKER; modeCount++);

    // Copy mode numbers to a new array
    auto vbeModes = new uint16_t[modeCount];
    for (uint32_t i = 0; i < modeCount; i++) {
        vbeModes[i] = modePointer[i];
    }

    // Check details of each mode and it to the supported modes, if it is as a valid lfb mode
    for (uint32_t i = 0; i < modeCount; i++) {
        auto vbeModeInfo = getModeInfo(vbeModes[i]);

        if (vbeModeInfo->physbase == 0 || vbeModeInfo->bpp < 15 || !(vbeModeInfo->attributes & MODE_ATTRIBUTES_HARDWARE_SUPPORT_BIT) ||
            !(vbeModeInfo->attributes & MODE_ATTRIBUTES_LFB_BIT) || (vbeModeInfo->memory_model != PACKED_PIXEL && vbeModeInfo->memory_model != DIRECT_COLOR)) {
            continue;
        }

        supportedModes.add({vbeModeInfo->Xres, vbeModeInfo->Yres, vbeModeInfo->bpp, vbeModeInfo->pitch, vbeModes[i]});
    }

    delete[] vbeModes;
}

bool VesaBiosExtensions::isAvailable() {
    if (!Bios::isAvailable()) {
        return false;
    }

    // Prepare bios parameters: Store function code in AX and return data address in ES:DI
    Device::Bios::CallParameters biosParameters{};
    biosParameters.ax = BiosFunction::GET_VBE_INFO;
    biosParameters.es = static_cast<uint16_t>(BIOS_CALL_RETURN_DATA_VIRTUAL_ADDRESS.get() >> 4);
    biosParameters.di = BIOS_CALL_RETURN_DATA_VIRTUAL_ADDRESS.get() & 0x000F;

    // Initialize return data memory
    VbeInfo info{};
    BIOS_CALL_RETURN_DATA_VIRTUAL_ADDRESS.copyRange(Util::Memory::Address<uint32_t>(&info), sizeof(VbeInfo));

    // Perform the bios call and check if it was successful
    Bios::interrupt(0x10, biosParameters);

    if (biosParameters.ax != BIOS_CALL_RETURN_CODE_SUCCESS) {
        return false;
    }

    // Check if return data contains correct signature ('VESA')
    Util::Memory::Address<uint32_t> signature(reinterpret_cast<VbeInfo *>(BIOS_CALL_RETURN_DATA_VIRTUAL_ADDRESS.get())->signature);
    return signature.compareString(Util::Memory::Address<uint32_t>(VESA_SIGNATURE)) == 0;
}

Util::Graphic::LinearFrameBuffer& VesaBiosExtensions::initializeLinearFrameBuffer(LinearFrameBufferProvider::ModeInfo &modeInfo) {
    // Get mode info and set mode
    auto vbeModeInfo = getModeInfo(modeInfo.modeNumber);
    setMode(modeInfo.modeNumber);

    // Map frame buffer into IO memory
    auto frameBuffer = Kernel::Management::getInstance().mapIO(vbeModeInfo->physbase, static_cast<uint32_t>(vbeModeInfo->pitch * vbeModeInfo->Yres));
    return *new Util::Graphic::LinearFrameBuffer(frameBuffer, vbeModeInfo->Xres, vbeModeInfo->Yres, vbeModeInfo->bpp, vbeModeInfo->pitch);
}

void VesaBiosExtensions::destroyLinearFrameBuffer(Util::Graphic::LinearFrameBuffer &lfb) {
    Kernel::Management::getInstance().freeIO(reinterpret_cast<void*>(lfb.getBuffer().get()));
    delete &lfb;
}

Util::Data::Array<LinearFrameBufferProvider::ModeInfo> VesaBiosExtensions::getAvailableModes() const {
    return supportedModes.toArray();
}

uint32_t VesaBiosExtensions::getVideoMemorySize() const {
    return memorySize;
}

Util::Memory::String VesaBiosExtensions::getVendorName() const {
    return vendorName;
}

Util::Memory::String VesaBiosExtensions::getDeviceName() const {
    return deviceName;
}

void VesaBiosExtensions::setMode(uint16_t mode) {
    // Prepare bios parameters: Store function code in AX and mode number in BX
    Device::Bios::CallParameters biosParameters{};
    biosParameters.ax = BiosFunction::SET_MODE;
    biosParameters.bx = mode | MODE_NUMBER_LFB_BIT;

    // Perform the bios call and check if it was successful
    Bios::interrupt(0x10, biosParameters);

    if (biosParameters.ax != BIOS_CALL_RETURN_CODE_SUCCESS) {
        Cpu::throwException(Cpu::Exception::UNSUPPORTED_OPERATION, "VesaBiosExtensions: Mode not supported!");
    }
}

VesaBiosExtensions::VbeInfo* VesaBiosExtensions::getVbeInfo() {
    // Prepare bios parameters: Store function code in AX and return data address in ES:DI
    Device::Bios::CallParameters biosParameters{};
    biosParameters.ax = BiosFunction::GET_VBE_INFO;
    biosParameters.es = static_cast<uint16_t>(BIOS_CALL_RETURN_DATA_VIRTUAL_ADDRESS.get() >> 4);
    biosParameters.di = BIOS_CALL_RETURN_DATA_VIRTUAL_ADDRESS.get() & 0x000F;

    // Initialize return data memory
    VbeInfo info{};
    BIOS_CALL_RETURN_DATA_VIRTUAL_ADDRESS.copyRange(Util::Memory::Address<uint32_t>(&info), sizeof(VbeInfo));

    // Perform the bios call and check if it was successful
    Bios::interrupt(0x10, biosParameters);

    if (biosParameters.ax != BIOS_CALL_RETURN_CODE_SUCCESS) {
        Cpu::throwException(Cpu::Exception::UNSUPPORTED_OPERATION, "VesaBiosExtensions: VesaBiosExtensions Bios Extensions are not supported!");
    }

    return reinterpret_cast<VbeInfo*>(BIOS_CALL_RETURN_DATA_VIRTUAL_ADDRESS.get());
}

VesaBiosExtensions::VbeModeInfo* VesaBiosExtensions::getModeInfo(uint16_t mode) {
    // Prepare bios parameters: Store function code in AX, mode number in CX and return data address in ES:DI
    Device::Bios::CallParameters biosParameters{};
    biosParameters.ax = BiosFunction::GET_MODE_INFO;
    biosParameters.cx = mode;
    biosParameters.es = static_cast<uint16_t>(BIOS_CALL_RETURN_DATA_VIRTUAL_ADDRESS.get() >> 4);
    biosParameters.di = BIOS_CALL_RETURN_DATA_VIRTUAL_ADDRESS.get() & 0x000F;

    // Initialize return data memory
    BIOS_CALL_RETURN_DATA_VIRTUAL_ADDRESS.setRange(0, sizeof(VbeModeInfo));

    // Perform the bios call and check if it was successful
    Bios::interrupt(0x10, biosParameters);

    if (biosParameters.ax != BIOS_CALL_RETURN_CODE_SUCCESS) {
        Cpu::throwException(Cpu::Exception::UNSUPPORTED_OPERATION, "VesaBiosExtensions: Mode not supported!");
    }

    return reinterpret_cast<VbeModeInfo*>(BIOS_CALL_RETURN_DATA_VIRTUAL_ADDRESS.get());
}

}