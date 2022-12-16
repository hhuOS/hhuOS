#include "device/bios/Bios.h"
#include "kernel/paging/MemoryLayout.h"
#include "kernel/system/System.h"
#include "VesaBiosExtensions.h"
#include "kernel/log/Logger.h"
#include "kernel/service/MemoryService.h"
#include "lib/util/Exception.h"
#include "lib/util/graphic/LinearFrameBuffer.h"
#include "lib/util/memory/Address.h"

namespace Device::Graphic {

Kernel::Logger VesaBiosExtensions::log = Kernel::Logger::get("VBE");

VesaBiosExtensions::VesaBiosExtensions(bool prototypeInstance) {
    if (prototypeInstance) {
        return;
    }

    log.info("Retrieving video card info");
    if (!isAvailable()) {
        Util::Exception::throwException(Util::Exception::ILLEGAL_STATE, "VesaBiosExtensions: No VBE compatible card found!");
    }

    auto vbeInfo = getVbeInfo();

    // Get vendor name, device name and memory size
    const char *vendorName = reinterpret_cast<const char*>(((vbeInfo.vendor[1] << 4) + vbeInfo.vendor[0]) + Kernel::MemoryLayout::KERNEL_START);
    const char *deviceName = reinterpret_cast<const char*>(((vbeInfo.product_name[1] << 4) + vbeInfo.product_name[0]) + Kernel::MemoryLayout::KERNEL_START);
    uint32_t memorySize = vbeInfo.video_memory * 65536;

    if (vendorName == nullptr) {
        vendorName = "Unknown";
    }

    if (deviceName == nullptr) {
        deviceName = "Unknown";
    }

    log.info("VBE compatible card found (Vendor: [%s], Device: [%s], Memory: [%u KiB])", vendorName, deviceName, memorySize / 1024);

    // Get available resolutions
    auto modePointer = reinterpret_cast<uint16_t*>(((vbeInfo.video_modes[1] << 4) + vbeInfo.video_modes[0]) + Kernel::MemoryLayout::KERNEL_START);

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

        if (vbeModeInfo.physbase == 0 || vbeModeInfo.bpp < 15 || !(vbeModeInfo.attributes & MODE_ATTRIBUTES_HARDWARE_SUPPORT_BIT) ||
            !(vbeModeInfo.attributes & MODE_ATTRIBUTES_LFB_BIT) || (vbeModeInfo.memory_model != PACKED_PIXEL && vbeModeInfo.memory_model != DIRECT_COLOR)) {
            continue;
        }

        log.debug("VBE compatible mode found [%ux%u@%u]", vbeModeInfo.Xres, vbeModeInfo.Yres, vbeModeInfo.bpp);
        supportedModes.add(ModeInfo{vbeModeInfo.Xres, vbeModeInfo.Yres, vbeModeInfo.bpp, vbeModeInfo.pitch, vbeModes[i]});
    }

    delete[] vbeModes;
}

bool VesaBiosExtensions::isAvailable() {
    if (!Bios::isAvailable()) {
        return false;
    }

    // Allocate space for VBE info struct inside lower memory
    auto vbeInfo = reinterpret_cast<VbeInfo*>(Kernel::System::getService<Kernel::MemoryService>().allocateLowerMemory(VBE_CONTROLLER_INFO_SIZE, 16));
    auto vbeInfoPhysicalAddress = Kernel::MemoryLayout::VIRTUAL_TO_PHYSICAL(reinterpret_cast<uint32_t>(vbeInfo));

    // Prepare bios parameters: Store function code in AX and return data address in ES:DI
    Device::Bios::RealModeContext biosParameters{};
    biosParameters.ax = BiosFunction::GET_VBE_INFO;
    biosParameters.es = static_cast<uint16_t>(vbeInfoPhysicalAddress >> 4);

    // Initialize return data memory
    VbeInfo info{};
    Util::Memory::Address<uint32_t>(vbeInfo).copyRange(Util::Memory::Address<uint32_t>(&info), sizeof(VbeInfo));

    // Perform the bios call and check if it was successful
    auto biosReturn = Bios::interrupt(0x10, biosParameters);
    if (biosReturn.ax != BIOS_CALL_RETURN_CODE_SUCCESS) {
        return false;
    }

    // Free allocated space in lower memory and check if return data contains correct signature ('VESA')
    Util::Memory::String signature = vbeInfo->signature;
    Kernel::System::getService<Kernel::MemoryService>().freeLowerMemory(vbeInfo, 16);

    return signature == VESA_SIGNATURE;
}

Util::Graphic::LinearFrameBuffer* VesaBiosExtensions::initializeLinearFrameBuffer(const ModeInfo &modeInfo) {
    if (!isAvailable()) {
        Util::Exception::throwException(Util::Exception::UNSUPPORTED_OPERATION, "VBE is not available on this machine!");
    }

    // Get mode info and set mode
    auto vbeModeInfo = getModeInfo(modeInfo.modeNumber);
    setMode(modeInfo.modeNumber);

    return new Util::Graphic::LinearFrameBuffer(reinterpret_cast<void*>(vbeModeInfo.physbase), vbeModeInfo.Xres, vbeModeInfo.Yres, vbeModeInfo.bpp, vbeModeInfo.pitch);
}

Util::Data::Array<LinearFrameBufferProvider::ModeInfo> VesaBiosExtensions::getAvailableModes() const {
    return supportedModes.toArray();
}

void VesaBiosExtensions::setMode(uint16_t mode) {
    // Prepare bios parameters: Store function code in AX and mode number in BX
    Device::Bios::RealModeContext biosParameters{};
    biosParameters.ax = BiosFunction::SET_MODE;
    biosParameters.bx = mode | MODE_NUMBER_LFB_BIT;

    // Perform the bios call and check if it was successful
    auto biosReturn = Bios::interrupt(0x10, biosParameters);
    if (biosReturn.ax != BIOS_CALL_RETURN_CODE_SUCCESS) {
        Util::Exception::throwException(Util::Exception::UNSUPPORTED_OPERATION, "VesaBiosExtensions: Mode not supported!");
    }
}

VesaBiosExtensions::VbeInfo VesaBiosExtensions::getVbeInfo() {
    // Allocate space for VBE info struct inside lower memory
    auto vbeInfo = reinterpret_cast<VbeInfo*>(Kernel::System::getService<Kernel::MemoryService>().allocateLowerMemory(VBE_CONTROLLER_INFO_SIZE, 16));
    auto vbeInfoPhysicalAddress = Kernel::MemoryLayout::VIRTUAL_TO_PHYSICAL(reinterpret_cast<uint32_t>(vbeInfo));

    // Prepare bios parameters: Store function code in AX and return data address in ES:DI
    Device::Bios::RealModeContext biosParameters{};
    biosParameters.ax = BiosFunction::GET_VBE_INFO;
    biosParameters.es = static_cast<uint16_t>(vbeInfoPhysicalAddress >> 4);

    // Initialize return data memory
    VbeInfo info{};
    Util::Memory::Address<uint32_t>(vbeInfo).copyRange(Util::Memory::Address<uint32_t>(&info), sizeof(VbeInfo));

    // Perform the bios call and check if it was successful
    auto biosReturn = Bios::interrupt(0x10, biosParameters);
    if (biosReturn.ax != BIOS_CALL_RETURN_CODE_SUCCESS) {
        Util::Exception::throwException(Util::Exception::UNSUPPORTED_OPERATION, "VesaBiosExtensions: VesaBiosExtensions Bios Extensions are not supported!");
    }

    // Create a copy of the VBE info struct and free the allocated space in lower memory
    VbeInfo ret = *vbeInfo;
    Kernel::System::getService<Kernel::MemoryService>().freeLowerMemory(vbeInfo, 16);

    return ret;
}

VesaBiosExtensions::VbeModeInfo VesaBiosExtensions::getModeInfo(uint16_t mode) {
    // Allocate space for VBE mode info struct inside lower memory
    auto modeInfo = reinterpret_cast<VbeModeInfo*>(Kernel::System::getService<Kernel::MemoryService>().allocateLowerMemory(VBE_MODE_INFO_SIZE, 16));
    auto modeInfoPhysicalAddress = Kernel::MemoryLayout::VIRTUAL_TO_PHYSICAL(reinterpret_cast<uint32_t>(modeInfo));

    // Prepare bios parameters: Store function code in AX, mode number in CX and return data address in ES:DI
    Device::Bios::RealModeContext biosParameters{};
    biosParameters.ax = BiosFunction::GET_MODE_INFO;
    biosParameters.cx = mode;
    biosParameters.es = static_cast<uint16_t>(modeInfoPhysicalAddress >> 4);

    // Initialize return data memory
    Util::Memory::Address<uint32_t>(modeInfo).setRange(0, VBE_MODE_INFO_SIZE);

    // Perform the bios call and check if it was successful
    auto biosReturn = Bios::interrupt(0x10, biosParameters);
    if (biosReturn.ax != BIOS_CALL_RETURN_CODE_SUCCESS) {
        Util::Exception::throwException(Util::Exception::UNSUPPORTED_OPERATION, "VesaBiosExtensions: Mode not supported!");
    }

    // Create a copy of the VBE mode info struct and free the allocated space in lower memory
    VbeModeInfo ret = *modeInfo;
    Kernel::System::getService<Kernel::MemoryService>().freeLowerMemory(modeInfo, 16);

    return ret;
}

}