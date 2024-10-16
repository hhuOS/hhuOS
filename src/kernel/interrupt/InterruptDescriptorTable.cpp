/*
 * Copyright (C) 2018-2024 Heinrich-Heine-Universitaet Duesseldorf,
 * Institute of Computer Science, Department Operating Systems
 * Burak Akguel, Christian Gesse, Fabian Ruhland, Filip Krakowski, Michael Schoettner
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
 */

#include "InterruptDescriptorTable.h"

#include <stdarg.h>

#include "kernel/service/Service.h"
#include "kernel/service/InterruptService.h"
#include "kernel/service/MemoryService.h"
#include "lib/util/base/System.h"
#include "kernel/service/ProcessService.h"
#include "kernel/process/Scheduler.h"

namespace Kernel {
enum InterruptVector : uint8_t;
struct InterruptFrame;

InterruptDescriptorTable::InterruptDescriptorTable() {
    // CPU Exceptions
    SET_IDT_ENTRY(table, 0, handleInterrupt0)
    SET_IDT_ENTRY(table, 1, handleInterrupt1)
    SET_IDT_ENTRY(table, 2, handleInterrupt2)
    SET_IDT_ENTRY(table, 3, handleInterrupt3)
    SET_IDT_ENTRY(table, 4, handleInterrupt4)
    SET_IDT_ENTRY(table, 5, handleInterrupt5)
    SET_IDT_ENTRY(table, 6, handleInterrupt6)
    SET_IDT_ENTRY(table, 7, handleFpuException)
    SET_IDT_ENTRY(table, 8, handleInterrupt8)
    SET_IDT_ENTRY(table, 9, handleInterrupt9)
    SET_IDT_ENTRY(table, 10, handleInterrupt10)
    SET_IDT_ENTRY(table, 11, handleInterrupt11)
    SET_IDT_ENTRY(table, 12, handleInterrupt12)
    SET_IDT_ENTRY(table, 13, handleInterrupt13)
    SET_IDT_ENTRY(table, 14, handlePageFault)
    SET_IDT_ENTRY(table, 15, handleInterrupt15)
    SET_IDT_ENTRY(table, 16, handleInterrupt16)
    SET_IDT_ENTRY(table, 17, handleInterrupt17)
    SET_IDT_ENTRY(table, 18, handleInterrupt18)
    SET_IDT_ENTRY(table, 19, handleInterrupt19)
    SET_IDT_ENTRY(table, 20, handleInterrupt20)
    SET_IDT_ENTRY(table, 21, handleInterrupt21)
    SET_IDT_ENTRY(table, 22, handleInterrupt22)
    SET_IDT_ENTRY(table, 23, handleInterrupt23)
    SET_IDT_ENTRY(table, 24, handleInterrupt24)
    SET_IDT_ENTRY(table, 25, handleInterrupt25)
    SET_IDT_ENTRY(table, 26, handleInterrupt26)
    SET_IDT_ENTRY(table, 27, handleInterrupt27)
    SET_IDT_ENTRY(table, 28, handleInterrupt28)
    SET_IDT_ENTRY(table, 29, handleInterrupt29)
    SET_IDT_ENTRY(table, 30, handleInterrupt30)
    SET_IDT_ENTRY(table, 31, handleInterrupt31)

    // Hardware Interrupts
    SET_IDT_ENTRY(table, 32, handleInterrupt32)
    SET_IDT_ENTRY(table, 33, handleInterrupt33)
    SET_IDT_ENTRY(table, 34, handleInterrupt34)
    SET_IDT_ENTRY(table, 35, handleInterrupt35)
    SET_IDT_ENTRY(table, 36, handleInterrupt36)
    SET_IDT_ENTRY(table, 37, handleInterrupt37)
    SET_IDT_ENTRY(table, 38, handleInterrupt38)
    SET_IDT_ENTRY(table, 39, handleInterrupt39)
    SET_IDT_ENTRY(table, 40, handleInterrupt40)
    SET_IDT_ENTRY(table, 41, handleInterrupt41)
    SET_IDT_ENTRY(table, 42, handleInterrupt42)
    SET_IDT_ENTRY(table, 43, handleInterrupt43)
    SET_IDT_ENTRY(table, 44, handleInterrupt44)
    SET_IDT_ENTRY(table, 45, handleInterrupt45)
    SET_IDT_ENTRY(table, 46, handleInterrupt46)
    SET_IDT_ENTRY(table, 47, handleInterrupt47)
    SET_IDT_ENTRY(table, 48, handleInterrupt48)
    SET_IDT_ENTRY(table, 49, handleInterrupt49)
    SET_IDT_ENTRY(table, 50, handleInterrupt50)
    SET_IDT_ENTRY(table, 51, handleInterrupt51)
    SET_IDT_ENTRY(table, 52, handleInterrupt52)
    SET_IDT_ENTRY(table, 53, handleInterrupt53)
    SET_IDT_ENTRY(table, 54, handleInterrupt54)
    SET_IDT_ENTRY(table, 55, handleInterrupt55)
    SET_IDT_ENTRY(table, 56, handleInterrupt56)
    SET_IDT_ENTRY(table, 57, handleInterrupt57)
    SET_IDT_ENTRY(table, 58, handleInterrupt58)
    SET_IDT_ENTRY(table, 59, handleInterrupt59)
    SET_IDT_ENTRY(table, 60, handleInterrupt60)
    SET_IDT_ENTRY(table, 61, handleInterrupt61)
    SET_IDT_ENTRY(table, 62, handleInterrupt62)
    SET_IDT_ENTRY(table, 63, handleInterrupt63)
    SET_IDT_ENTRY(table, 64, handleInterrupt64)
    SET_IDT_ENTRY(table, 65, handleInterrupt65)
    SET_IDT_ENTRY(table, 66, handleInterrupt66)
    SET_IDT_ENTRY(table, 67, handleInterrupt67)
    SET_IDT_ENTRY(table, 68, handleInterrupt68)
    SET_IDT_ENTRY(table, 69, handleInterrupt69)
    SET_IDT_ENTRY(table, 70, handleInterrupt70)
    SET_IDT_ENTRY(table, 71, handleInterrupt71)
    SET_IDT_ENTRY(table, 72, handleInterrupt72)
    SET_IDT_ENTRY(table, 73, handleInterrupt73)
    SET_IDT_ENTRY(table, 74, handleInterrupt74)
    SET_IDT_ENTRY(table, 75, handleInterrupt75)
    SET_IDT_ENTRY(table, 76, handleInterrupt76)
    SET_IDT_ENTRY(table, 77, handleInterrupt77)
    SET_IDT_ENTRY(table, 78, handleInterrupt78)
    SET_IDT_ENTRY(table, 79, handleInterrupt79)
    SET_IDT_ENTRY(table, 80, handleInterrupt80)
    SET_IDT_ENTRY(table, 81, handleInterrupt81)
    SET_IDT_ENTRY(table, 82, handleInterrupt82)
    SET_IDT_ENTRY(table, 83, handleInterrupt83)
    SET_IDT_ENTRY(table, 84, handleInterrupt84)
    SET_IDT_ENTRY(table, 85, handleInterrupt85)
    SET_IDT_ENTRY(table, 86, handleInterrupt86)
    SET_IDT_ENTRY(table, 87, handleInterrupt87)
    SET_IDT_ENTRY(table, 88, handleInterrupt88)
    SET_IDT_ENTRY(table, 89, handleInterrupt89)
    SET_IDT_ENTRY(table, 90, handleInterrupt90)
    SET_IDT_ENTRY(table, 91, handleInterrupt91)
    SET_IDT_ENTRY(table, 92, handleInterrupt92)
    SET_IDT_ENTRY(table, 93, handleInterrupt93)
    SET_IDT_ENTRY(table, 94, handleInterrupt94)
    SET_IDT_ENTRY(table, 95, handleInterrupt95)
    SET_IDT_ENTRY(table, 96, handleInterrupt96)
    SET_IDT_ENTRY(table, 97, handleInterrupt97)
    SET_IDT_ENTRY(table, 98, handleInterrupt98)
    SET_IDT_ENTRY(table, 99, handleInterrupt99)
    SET_IDT_ENTRY(table, 100, handleInterrupt100)
    SET_IDT_ENTRY(table, 101, handleInterrupt101)
    SET_IDT_ENTRY(table, 102, handleInterrupt102)
    SET_IDT_ENTRY(table, 103, handleInterrupt103)
    SET_IDT_ENTRY(table, 104, handleInterrupt104)
    SET_IDT_ENTRY(table, 105, handleInterrupt105)
    SET_IDT_ENTRY(table, 106, handleInterrupt106)
    SET_IDT_ENTRY(table, 107, handleInterrupt107)
    SET_IDT_ENTRY(table, 108, handleInterrupt108)
    SET_IDT_ENTRY(table, 109, handleInterrupt109)
    SET_IDT_ENTRY(table, 110, handleInterrupt110)
    SET_IDT_ENTRY(table, 111, handleInterrupt111)
    SET_IDT_ENTRY(table, 112, handleInterrupt112)
    SET_IDT_ENTRY(table, 113, handleInterrupt113)
    SET_IDT_ENTRY(table, 114, handleInterrupt114)
    SET_IDT_ENTRY(table, 115, handleInterrupt115)
    SET_IDT_ENTRY(table, 116, handleInterrupt116)
    SET_IDT_ENTRY(table, 117, handleInterrupt117)
    SET_IDT_ENTRY(table, 118, handleInterrupt118)
    SET_IDT_ENTRY(table, 119, handleInterrupt119)
    SET_IDT_ENTRY(table, 120, handleInterrupt120)
    SET_IDT_ENTRY(table, 121, handleInterrupt121)
    SET_IDT_ENTRY(table, 122, handleInterrupt122)
    SET_IDT_ENTRY(table, 123, handleInterrupt123)
    SET_IDT_ENTRY(table, 124, handleInterrupt124)
    SET_IDT_ENTRY(table, 125, handleInterrupt125)
    SET_IDT_ENTRY(table, 126, handleInterrupt126)
    SET_IDT_ENTRY(table, 127, handleInterrupt127)
    SET_IDT_ENTRY(table, 128, handleInterrupt128)
    SET_IDT_ENTRY(table, 129, handleInterrupt129)
    SET_IDT_ENTRY(table, 130, handleInterrupt130)
    SET_IDT_ENTRY(table, 131, handleInterrupt131)
    SET_IDT_ENTRY(table, 132, handleInterrupt132)
    SET_IDT_ENTRY(table, 133, handleInterrupt133)
    // System call handler is set up below (0x86 = 134)
    SET_IDT_ENTRY(table, 135, handleInterrupt135)
    SET_IDT_ENTRY(table, 136, handleInterrupt136)
    SET_IDT_ENTRY(table, 137, handleInterrupt137)
    SET_IDT_ENTRY(table, 138, handleInterrupt138)
    SET_IDT_ENTRY(table, 139, handleInterrupt139)
    SET_IDT_ENTRY(table, 140, handleInterrupt140)
    SET_IDT_ENTRY(table, 141, handleInterrupt141)
    SET_IDT_ENTRY(table, 142, handleInterrupt142)
    SET_IDT_ENTRY(table, 143, handleInterrupt143)
    SET_IDT_ENTRY(table, 144, handleInterrupt144)
    SET_IDT_ENTRY(table, 145, handleInterrupt145)
    SET_IDT_ENTRY(table, 146, handleInterrupt146)
    SET_IDT_ENTRY(table, 147, handleInterrupt147)
    SET_IDT_ENTRY(table, 148, handleInterrupt148)
    SET_IDT_ENTRY(table, 149, handleInterrupt149)
    SET_IDT_ENTRY(table, 150, handleInterrupt150)
    SET_IDT_ENTRY(table, 151, handleInterrupt151)
    SET_IDT_ENTRY(table, 152, handleInterrupt152)
    SET_IDT_ENTRY(table, 153, handleInterrupt153)
    SET_IDT_ENTRY(table, 154, handleInterrupt154)
    SET_IDT_ENTRY(table, 155, handleInterrupt155)
    SET_IDT_ENTRY(table, 156, handleInterrupt156)
    SET_IDT_ENTRY(table, 157, handleInterrupt157)
    SET_IDT_ENTRY(table, 158, handleInterrupt158)
    SET_IDT_ENTRY(table, 159, handleInterrupt159)
    SET_IDT_ENTRY(table, 160, handleInterrupt160)
    SET_IDT_ENTRY(table, 161, handleInterrupt161)
    SET_IDT_ENTRY(table, 162, handleInterrupt162)
    SET_IDT_ENTRY(table, 163, handleInterrupt163)
    SET_IDT_ENTRY(table, 164, handleInterrupt164)
    SET_IDT_ENTRY(table, 165, handleInterrupt165)
    SET_IDT_ENTRY(table, 166, handleInterrupt166)
    SET_IDT_ENTRY(table, 167, handleInterrupt167)
    SET_IDT_ENTRY(table, 168, handleInterrupt168)
    SET_IDT_ENTRY(table, 169, handleInterrupt169)
    SET_IDT_ENTRY(table, 170, handleInterrupt170)
    SET_IDT_ENTRY(table, 171, handleInterrupt171)
    SET_IDT_ENTRY(table, 172, handleInterrupt172)
    SET_IDT_ENTRY(table, 173, handleInterrupt173)
    SET_IDT_ENTRY(table, 174, handleInterrupt174)
    SET_IDT_ENTRY(table, 175, handleInterrupt175)
    SET_IDT_ENTRY(table, 176, handleInterrupt176)
    SET_IDT_ENTRY(table, 177, handleInterrupt177)
    SET_IDT_ENTRY(table, 178, handleInterrupt178)
    SET_IDT_ENTRY(table, 179, handleInterrupt179)
    SET_IDT_ENTRY(table, 180, handleInterrupt180)
    SET_IDT_ENTRY(table, 181, handleInterrupt181)
    SET_IDT_ENTRY(table, 182, handleInterrupt182)
    SET_IDT_ENTRY(table, 183, handleInterrupt183)
    SET_IDT_ENTRY(table, 184, handleInterrupt184)
    SET_IDT_ENTRY(table, 185, handleInterrupt185)
    SET_IDT_ENTRY(table, 186, handleInterrupt186)
    SET_IDT_ENTRY(table, 187, handleInterrupt187)
    SET_IDT_ENTRY(table, 188, handleInterrupt188)
    SET_IDT_ENTRY(table, 189, handleInterrupt189)
    SET_IDT_ENTRY(table, 190, handleInterrupt190)
    SET_IDT_ENTRY(table, 191, handleInterrupt191)
    SET_IDT_ENTRY(table, 192, handleInterrupt192)
    SET_IDT_ENTRY(table, 193, handleInterrupt193)
    SET_IDT_ENTRY(table, 194, handleInterrupt194)
    SET_IDT_ENTRY(table, 195, handleInterrupt195)
    SET_IDT_ENTRY(table, 196, handleInterrupt196)
    SET_IDT_ENTRY(table, 197, handleInterrupt197)
    SET_IDT_ENTRY(table, 198, handleInterrupt198)
    SET_IDT_ENTRY(table, 199, handleInterrupt199)
    SET_IDT_ENTRY(table, 200, handleInterrupt200)
    SET_IDT_ENTRY(table, 201, handleInterrupt201)
    SET_IDT_ENTRY(table, 202, handleInterrupt202)
    SET_IDT_ENTRY(table, 203, handleInterrupt203)
    SET_IDT_ENTRY(table, 204, handleInterrupt204)
    SET_IDT_ENTRY(table, 205, handleInterrupt205)
    SET_IDT_ENTRY(table, 206, handleInterrupt206)
    SET_IDT_ENTRY(table, 207, handleInterrupt207)
    SET_IDT_ENTRY(table, 208, handleInterrupt208)
    SET_IDT_ENTRY(table, 209, handleInterrupt209)
    SET_IDT_ENTRY(table, 210, handleInterrupt210)
    SET_IDT_ENTRY(table, 211, handleInterrupt211)
    SET_IDT_ENTRY(table, 212, handleInterrupt212)
    SET_IDT_ENTRY(table, 213, handleInterrupt213)
    SET_IDT_ENTRY(table, 214, handleInterrupt214)
    SET_IDT_ENTRY(table, 215, handleInterrupt215)
    SET_IDT_ENTRY(table, 216, handleInterrupt216)
    SET_IDT_ENTRY(table, 217, handleInterrupt217)
    SET_IDT_ENTRY(table, 218, handleInterrupt218)
    SET_IDT_ENTRY(table, 219, handleInterrupt219)
    SET_IDT_ENTRY(table, 220, handleInterrupt220)
    SET_IDT_ENTRY(table, 221, handleInterrupt221)
    SET_IDT_ENTRY(table, 222, handleInterrupt222)
    SET_IDT_ENTRY(table, 223, handleInterrupt223)
    SET_IDT_ENTRY(table, 224, handleInterrupt224)
    SET_IDT_ENTRY(table, 225, handleInterrupt225)
    SET_IDT_ENTRY(table, 226, handleInterrupt226)
    SET_IDT_ENTRY(table, 227, handleInterrupt227)
    SET_IDT_ENTRY(table, 228, handleInterrupt228)
    SET_IDT_ENTRY(table, 229, handleInterrupt229)
    SET_IDT_ENTRY(table, 230, handleInterrupt230)
    SET_IDT_ENTRY(table, 231, handleInterrupt231)
    SET_IDT_ENTRY(table, 232, handleInterrupt232)
    SET_IDT_ENTRY(table, 233, handleInterrupt233)
    SET_IDT_ENTRY(table, 234, handleInterrupt234)
    SET_IDT_ENTRY(table, 235, handleInterrupt235)
    SET_IDT_ENTRY(table, 236, handleInterrupt236)
    SET_IDT_ENTRY(table, 237, handleInterrupt237)
    SET_IDT_ENTRY(table, 238, handleInterrupt238)
    SET_IDT_ENTRY(table, 239, handleInterrupt239)
    SET_IDT_ENTRY(table, 240, handleInterrupt240)
    SET_IDT_ENTRY(table, 241, handleInterrupt241)
    SET_IDT_ENTRY(table, 242, handleInterrupt242)
    SET_IDT_ENTRY(table, 243, handleInterrupt243)
    SET_IDT_ENTRY(table, 244, handleInterrupt244)
    SET_IDT_ENTRY(table, 245, handleInterrupt245)
    SET_IDT_ENTRY(table, 246, handleInterrupt246)
    SET_IDT_ENTRY(table, 247, handleInterrupt247)
    SET_IDT_ENTRY(table, 248, handleInterrupt248)
    SET_IDT_ENTRY(table, 249, handleInterrupt249)
    SET_IDT_ENTRY(table, 250, handleInterrupt250)
    SET_IDT_ENTRY(table, 251, handleInterrupt251)
    SET_IDT_ENTRY(table, 252, handleInterrupt252)
    SET_IDT_ENTRY(table, 253, handleInterrupt253)
    SET_IDT_ENTRY(table, 254, handleInterrupt254)
    SET_IDT_ENTRY(table, 255, handleInterrupt255)

    // Set system call handler
    table[0x86] = GateDescriptor(reinterpret_cast<uint32_t>(handleSystemCall), Device::Cpu::SegmentSelector(Device::Cpu::Ring0, 1), GateType::TRAP_32, Device::Cpu::PrivilegeLevel::Ring3);
}

InterruptDescriptorTable::Descriptor::Descriptor(void *address, uint16_t entries) : size((entries * sizeof(uint64_t)) - 1), offset(reinterpret_cast<uint32_t>(address)) {}

InterruptDescriptorTable::GateDescriptor::GateDescriptor(uint32_t address, const Device::Cpu::SegmentSelector &segmentSelector, InterruptDescriptorTable::GateType gateType, Device::Cpu::PrivilegeLevel privilegeLevel) :
        offset1(static_cast<uint16_t>(address & 0x0000ffff)),
        segmentSelector(static_cast<uint16_t>(segmentSelector)),
        reserved1(0),
        gateType(gateType),
        reserved2(0),
        privilegeLevel(privilegeLevel),
        present(1),
        offset2(static_cast<uint16_t>((address & 0xffff0000) >> 16)) {}

InterruptDescriptorTable::GateDescriptor::operator uint64_t() const {
    return *reinterpret_cast<const uint64_t*>(this);
}

InterruptDescriptorTable::Descriptor InterruptDescriptorTable::getDescriptor() {
    return Descriptor(table, IDT_ENTRIES);
}

void InterruptDescriptorTable::Descriptor::load() {
    asm volatile(
            "lidt (%0)"
            : :
            "r"(this)
            :
            );
}

void InterruptDescriptorTable::load() {
    getDescriptor().load();
}

void InterruptDescriptorTable::handleException(const Kernel::InterruptFrame &frame, uint32_t errorCode, InterruptVector vector) {
    Service::getService<InterruptService>().handleException(frame, errorCode, vector);
}

void InterruptDescriptorTable::handleInterrupt(const InterruptFrame &frame, InterruptVector vector) {
    auto &interruptService = Service::getService<InterruptService>();

    // Ignore spurious interrupts
    if (interruptService.checkSpuriousInterrupt(vector)) {
        return;
    }

    // Dispatch interrupt
    Service::getService<InterruptService>().dispatchInterrupt(frame, vector);

    // Send EOI to interrupt controller
    interruptService.sendEndOfInterrupt(vector);
}

void InterruptDescriptorTable::handlePageFault([[maybe_unused]] InterruptFrame *frame, uint32_t errorCode) {
    Service::getService<MemoryService>().handlePageFault(errorCode);
}

void InterruptDescriptorTable::handleFpuException([[maybe_unused]] InterruptFrame *frame) {
    Kernel::Service::getService<Kernel::ProcessService>().getScheduler().switchFpuContext();
}

void InterruptDescriptorTable::handleSystemCall([[maybe_unused]] InterruptFrame *frame) {
    uint32_t ebxValue;
    uint32_t ecxValue;
    uint32_t edxValue;

    asm volatile (
            "mov %%ebx, (%0);"
            "mov %%ecx, (%1);"
            "mov %%edx, (%2);"
            : :
            "r"(&ebxValue), "r"(&ecxValue), "r"(&edxValue)
            :
            "ebx", "ecx", "edx"
            );

    Service::getService<InterruptService>().dispatchSystemCall(static_cast<Util::System::Code>(ebxValue), ebxValue >> 8, reinterpret_cast<va_list>(ecxValue), *reinterpret_cast<bool *>(edxValue));
}

}