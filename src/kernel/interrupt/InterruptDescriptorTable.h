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

#ifndef HHUOS_INTERRUPTDESCRIPTORTABLE_H
#define HHUOS_INTERRUPTDESCRIPTORTABLE_H

#include <cstdint>

#include "device/cpu/Cpu.h"

namespace Kernel {
enum InterruptVector : uint8_t;
struct InterruptFrame;
}  // namespace Kernel

#define STRINGIFY(a) #a

#define CAT(a,b) a##b

#define INTERRUPT_HANDLER(VECTOR, HANDLER) __attribute__ ((interrupt)) static void CAT(handleInterrupt, VECTOR)(InterruptFrame *frame) { \
         HANDLER(*frame, static_cast<InterruptVector>(VECTOR));\
    }

#define EXCEPTION_HANDLER(VECTOR, HANDLER) __attribute__ ((interrupt)) static void CAT(handleInterrupt, VECTOR)(InterruptFrame *frame) { \
         HANDLER(*frame, 0, static_cast<InterruptVector>(VECTOR));\
    }

#define EXCEPTION_HANDLER_WITH_ERROR_CODE(VECTOR, HANDLER) __attribute__ ((interrupt)) static void CAT(handleInterrupt, VECTOR)(InterruptFrame *frame, uint32_t errorCode) { \
         HANDLER(*frame, errorCode, static_cast<InterruptVector>(VECTOR));\
    }

#define SET_IDT_ENTRY(IDT, INDEX, HANDLER) IDT[INDEX] = GateDescriptor(reinterpret_cast<uint32_t>(HANDLER), Device::Cpu::SegmentSelector(Device::Cpu::Ring0, 1), GateType::INTERRUPT_32, Device::Cpu::PrivilegeLevel::Ring0);

namespace Kernel {

class InterruptDescriptorTable {

public:

    enum GateType : uint8_t {
        TASK = 0x5,
        INTERRUPT_16 = 0x6,
        TRAP_16 = 0x7,
        INTERRUPT_32 = 0xe,
        TRAP_32 = 0xf
    };

    struct GateDescriptor {
    public:

        GateDescriptor(uint32_t address, const Device::Cpu::SegmentSelector &segmentSelector, GateType gateType, Device::Cpu::PrivilegeLevel privilegeLevel);

        operator uint64_t() const;

    private:
        uint16_t offset1;
        uint16_t segmentSelector;
        uint8_t reserved1;
        uint8_t gateType: 4;
        uint8_t reserved2: 1;
        uint8_t privilegeLevel: 2;
        uint8_t present: 1;
        uint16_t offset2;
    };

    struct Descriptor {
        uint16_t size = (IDT_ENTRIES * sizeof(GateDescriptor)) - 1;
        uint32_t offset;

        Descriptor(void *address, uint16_t entries);
        void load();
    } __attribute__((packed));

    /**
     * Default-Constructor.
     */
    InterruptDescriptorTable();

    /**
     * Copy Constructor.
     */
    InterruptDescriptorTable(const InterruptDescriptorTable &copy) = delete;

    /**
     * Assignment operator.
     */
    InterruptDescriptorTable &operator=(const InterruptDescriptorTable &other) = delete;

    /**
     * Destructor.
     */
    ~InterruptDescriptorTable() = default;

    Descriptor getDescriptor();

    void load();

private:

    static const constexpr uint32_t IDT_ENTRIES = 256;

#pragma GCC push_options
#pragma GCC target("general-regs-only")

    static void handleException(const InterruptFrame &frame, uint32_t errorCode, InterruptVector vector);

    static void handleInterrupt(const InterruptFrame &frame, InterruptVector vector);

    __attribute__ ((interrupt)) static void handlePageFault(InterruptFrame *frame, uint32_t errorCode);

    __attribute__ ((interrupt)) static void handleFpuException(InterruptFrame *frame);

    __attribute__ ((interrupt)) static void handleSystemCall(InterruptFrame *frame);

    // CPU Exceptions
    EXCEPTION_HANDLER(0, handleException)
    EXCEPTION_HANDLER(1, handleException)
    EXCEPTION_HANDLER(2, handleException)
    EXCEPTION_HANDLER(3, handleException)
    EXCEPTION_HANDLER(4, handleException)
    EXCEPTION_HANDLER(5, handleException)
    EXCEPTION_HANDLER(6, handleException)
    EXCEPTION_HANDLER(7, handleException)
    EXCEPTION_HANDLER_WITH_ERROR_CODE(8, handleException)
    EXCEPTION_HANDLER(9, handleException)
    EXCEPTION_HANDLER_WITH_ERROR_CODE(10, handleException)
    EXCEPTION_HANDLER_WITH_ERROR_CODE(11, handleException)
    EXCEPTION_HANDLER_WITH_ERROR_CODE(12, handleException)
    EXCEPTION_HANDLER_WITH_ERROR_CODE(13, handleException)
    // Interrupt vector 14 is used for page faults and has a specific handler
    EXCEPTION_HANDLER(15, handleException)
    EXCEPTION_HANDLER(16, handleException)
    EXCEPTION_HANDLER_WITH_ERROR_CODE(17, handleException)
    EXCEPTION_HANDLER(18, handleException)
    EXCEPTION_HANDLER(19, handleException)
    EXCEPTION_HANDLER(20, handleException)
    EXCEPTION_HANDLER_WITH_ERROR_CODE(21, handleException)
    EXCEPTION_HANDLER(22, handleException)
    EXCEPTION_HANDLER(23, handleException)
    EXCEPTION_HANDLER(24, handleException)
    EXCEPTION_HANDLER(25, handleException)
    EXCEPTION_HANDLER(26, handleException)
    EXCEPTION_HANDLER(27, handleException)
    EXCEPTION_HANDLER(28, handleException)
    EXCEPTION_HANDLER_WITH_ERROR_CODE(29, handleException)
    EXCEPTION_HANDLER_WITH_ERROR_CODE(30, handleException)
    EXCEPTION_HANDLER(31, handleException)

    // Hardware Interrupts
    INTERRUPT_HANDLER(32, handleInterrupt)
    INTERRUPT_HANDLER(33, handleInterrupt)
    INTERRUPT_HANDLER(34, handleInterrupt)
    INTERRUPT_HANDLER(35, handleInterrupt)
    INTERRUPT_HANDLER(36, handleInterrupt)
    INTERRUPT_HANDLER(37, handleInterrupt)
    INTERRUPT_HANDLER(38, handleInterrupt)
    INTERRUPT_HANDLER(39, handleInterrupt)
    INTERRUPT_HANDLER(40, handleInterrupt)
    INTERRUPT_HANDLER(41, handleInterrupt)
    INTERRUPT_HANDLER(42, handleInterrupt)
    INTERRUPT_HANDLER(43, handleInterrupt)
    INTERRUPT_HANDLER(44, handleInterrupt)
    INTERRUPT_HANDLER(45, handleInterrupt)
    INTERRUPT_HANDLER(46, handleInterrupt)
    INTERRUPT_HANDLER(47, handleInterrupt)
    INTERRUPT_HANDLER(48, handleInterrupt)
    INTERRUPT_HANDLER(49, handleInterrupt)
    INTERRUPT_HANDLER(50, handleInterrupt)
    INTERRUPT_HANDLER(51, handleInterrupt)
    INTERRUPT_HANDLER(52, handleInterrupt)
    INTERRUPT_HANDLER(53, handleInterrupt)
    INTERRUPT_HANDLER(54, handleInterrupt)
    INTERRUPT_HANDLER(55, handleInterrupt)
    INTERRUPT_HANDLER(56, handleInterrupt)
    INTERRUPT_HANDLER(57, handleInterrupt)
    INTERRUPT_HANDLER(58, handleInterrupt)
    INTERRUPT_HANDLER(59, handleInterrupt)
    INTERRUPT_HANDLER(60, handleInterrupt)
    INTERRUPT_HANDLER(61, handleInterrupt)
    INTERRUPT_HANDLER(62, handleInterrupt)
    INTERRUPT_HANDLER(63, handleInterrupt)
    INTERRUPT_HANDLER(64, handleInterrupt)
    INTERRUPT_HANDLER(65, handleInterrupt)
    INTERRUPT_HANDLER(66, handleInterrupt)
    INTERRUPT_HANDLER(67, handleInterrupt)
    INTERRUPT_HANDLER(68, handleInterrupt)
    INTERRUPT_HANDLER(69, handleInterrupt)
    INTERRUPT_HANDLER(70, handleInterrupt)
    INTERRUPT_HANDLER(71, handleInterrupt)
    INTERRUPT_HANDLER(72, handleInterrupt)
    INTERRUPT_HANDLER(73, handleInterrupt)
    INTERRUPT_HANDLER(74, handleInterrupt)
    INTERRUPT_HANDLER(75, handleInterrupt)
    INTERRUPT_HANDLER(76, handleInterrupt)
    INTERRUPT_HANDLER(77, handleInterrupt)
    INTERRUPT_HANDLER(78, handleInterrupt)
    INTERRUPT_HANDLER(79, handleInterrupt)
    INTERRUPT_HANDLER(80, handleInterrupt)
    INTERRUPT_HANDLER(81, handleInterrupt)
    INTERRUPT_HANDLER(82, handleInterrupt)
    INTERRUPT_HANDLER(83, handleInterrupt)
    INTERRUPT_HANDLER(84, handleInterrupt)
    INTERRUPT_HANDLER(85, handleInterrupt)
    INTERRUPT_HANDLER(86, handleInterrupt)
    INTERRUPT_HANDLER(87, handleInterrupt)
    INTERRUPT_HANDLER(88, handleInterrupt)
    INTERRUPT_HANDLER(89, handleInterrupt)
    INTERRUPT_HANDLER(90, handleInterrupt)
    INTERRUPT_HANDLER(91, handleInterrupt)
    INTERRUPT_HANDLER(92, handleInterrupt)
    INTERRUPT_HANDLER(93, handleInterrupt)
    INTERRUPT_HANDLER(94, handleInterrupt)
    INTERRUPT_HANDLER(95, handleInterrupt)
    INTERRUPT_HANDLER(96, handleInterrupt)
    INTERRUPT_HANDLER(97, handleInterrupt)
    INTERRUPT_HANDLER(98, handleInterrupt)
    INTERRUPT_HANDLER(99, handleInterrupt)
    INTERRUPT_HANDLER(100, handleInterrupt)
    INTERRUPT_HANDLER(101, handleInterrupt)
    INTERRUPT_HANDLER(102, handleInterrupt)
    INTERRUPT_HANDLER(103, handleInterrupt)
    INTERRUPT_HANDLER(104, handleInterrupt)
    INTERRUPT_HANDLER(105, handleInterrupt)
    INTERRUPT_HANDLER(106, handleInterrupt)
    INTERRUPT_HANDLER(107, handleInterrupt)
    INTERRUPT_HANDLER(108, handleInterrupt)
    INTERRUPT_HANDLER(109, handleInterrupt)
    INTERRUPT_HANDLER(110, handleInterrupt)
    INTERRUPT_HANDLER(111, handleInterrupt)
    INTERRUPT_HANDLER(112, handleInterrupt)
    INTERRUPT_HANDLER(113, handleInterrupt)
    INTERRUPT_HANDLER(114, handleInterrupt)
    INTERRUPT_HANDLER(115, handleInterrupt)
    INTERRUPT_HANDLER(116, handleInterrupt)
    INTERRUPT_HANDLER(117, handleInterrupt)
    INTERRUPT_HANDLER(118, handleInterrupt)
    INTERRUPT_HANDLER(119, handleInterrupt)
    INTERRUPT_HANDLER(120, handleInterrupt)
    INTERRUPT_HANDLER(121, handleInterrupt)
    INTERRUPT_HANDLER(122, handleInterrupt)
    INTERRUPT_HANDLER(123, handleInterrupt)
    INTERRUPT_HANDLER(124, handleInterrupt)
    INTERRUPT_HANDLER(125, handleInterrupt)
    INTERRUPT_HANDLER(126, handleInterrupt)
    INTERRUPT_HANDLER(127, handleInterrupt)
    INTERRUPT_HANDLER(128, handleInterrupt)
    INTERRUPT_HANDLER(129, handleInterrupt)
    INTERRUPT_HANDLER(130, handleInterrupt)
    INTERRUPT_HANDLER(131, handleInterrupt)
    INTERRUPT_HANDLER(132, handleInterrupt)
    INTERRUPT_HANDLER(133, handleInterrupt)
    // Interrupt vector 134 (0x86) is used for system calls and has a specific handler
    INTERRUPT_HANDLER(135, handleInterrupt)
    INTERRUPT_HANDLER(136, handleInterrupt)
    INTERRUPT_HANDLER(137, handleInterrupt)
    INTERRUPT_HANDLER(138, handleInterrupt)
    INTERRUPT_HANDLER(139, handleInterrupt)
    INTERRUPT_HANDLER(140, handleInterrupt)
    INTERRUPT_HANDLER(141, handleInterrupt)
    INTERRUPT_HANDLER(142, handleInterrupt)
    INTERRUPT_HANDLER(143, handleInterrupt)
    INTERRUPT_HANDLER(144, handleInterrupt)
    INTERRUPT_HANDLER(145, handleInterrupt)
    INTERRUPT_HANDLER(146, handleInterrupt)
    INTERRUPT_HANDLER(147, handleInterrupt)
    INTERRUPT_HANDLER(148, handleInterrupt)
    INTERRUPT_HANDLER(149, handleInterrupt)
    INTERRUPT_HANDLER(150, handleInterrupt)
    INTERRUPT_HANDLER(151, handleInterrupt)
    INTERRUPT_HANDLER(152, handleInterrupt)
    INTERRUPT_HANDLER(153, handleInterrupt)
    INTERRUPT_HANDLER(154, handleInterrupt)
    INTERRUPT_HANDLER(155, handleInterrupt)
    INTERRUPT_HANDLER(156, handleInterrupt)
    INTERRUPT_HANDLER(157, handleInterrupt)
    INTERRUPT_HANDLER(158, handleInterrupt)
    INTERRUPT_HANDLER(159, handleInterrupt)
    INTERRUPT_HANDLER(160, handleInterrupt)
    INTERRUPT_HANDLER(161, handleInterrupt)
    INTERRUPT_HANDLER(162, handleInterrupt)
    INTERRUPT_HANDLER(163, handleInterrupt)
    INTERRUPT_HANDLER(164, handleInterrupt)
    INTERRUPT_HANDLER(165, handleInterrupt)
    INTERRUPT_HANDLER(166, handleInterrupt)
    INTERRUPT_HANDLER(167, handleInterrupt)
    INTERRUPT_HANDLER(168, handleInterrupt)
    INTERRUPT_HANDLER(169, handleInterrupt)
    INTERRUPT_HANDLER(170, handleInterrupt)
    INTERRUPT_HANDLER(171, handleInterrupt)
    INTERRUPT_HANDLER(172, handleInterrupt)
    INTERRUPT_HANDLER(173, handleInterrupt)
    INTERRUPT_HANDLER(174, handleInterrupt)
    INTERRUPT_HANDLER(175, handleInterrupt)
    INTERRUPT_HANDLER(176, handleInterrupt)
    INTERRUPT_HANDLER(177, handleInterrupt)
    INTERRUPT_HANDLER(178, handleInterrupt)
    INTERRUPT_HANDLER(179, handleInterrupt)
    INTERRUPT_HANDLER(180, handleInterrupt)
    INTERRUPT_HANDLER(181, handleInterrupt)
    INTERRUPT_HANDLER(182, handleInterrupt)
    INTERRUPT_HANDLER(183, handleInterrupt)
    INTERRUPT_HANDLER(184, handleInterrupt)
    INTERRUPT_HANDLER(185, handleInterrupt)
    INTERRUPT_HANDLER(186, handleInterrupt)
    INTERRUPT_HANDLER(187, handleInterrupt)
    INTERRUPT_HANDLER(188, handleInterrupt)
    INTERRUPT_HANDLER(189, handleInterrupt)
    INTERRUPT_HANDLER(190, handleInterrupt)
    INTERRUPT_HANDLER(191, handleInterrupt)
    INTERRUPT_HANDLER(192, handleInterrupt)
    INTERRUPT_HANDLER(193, handleInterrupt)
    INTERRUPT_HANDLER(194, handleInterrupt)
    INTERRUPT_HANDLER(195, handleInterrupt)
    INTERRUPT_HANDLER(196, handleInterrupt)
    INTERRUPT_HANDLER(197, handleInterrupt)
    INTERRUPT_HANDLER(198, handleInterrupt)
    INTERRUPT_HANDLER(199, handleInterrupt)
    INTERRUPT_HANDLER(200, handleInterrupt)
    INTERRUPT_HANDLER(201, handleInterrupt)
    INTERRUPT_HANDLER(202, handleInterrupt)
    INTERRUPT_HANDLER(203, handleInterrupt)
    INTERRUPT_HANDLER(204, handleInterrupt)
    INTERRUPT_HANDLER(205, handleInterrupt)
    INTERRUPT_HANDLER(206, handleInterrupt)
    INTERRUPT_HANDLER(207, handleInterrupt)
    INTERRUPT_HANDLER(208, handleInterrupt)
    INTERRUPT_HANDLER(209, handleInterrupt)
    INTERRUPT_HANDLER(210, handleInterrupt)
    INTERRUPT_HANDLER(211, handleInterrupt)
    INTERRUPT_HANDLER(212, handleInterrupt)
    INTERRUPT_HANDLER(213, handleInterrupt)
    INTERRUPT_HANDLER(214, handleInterrupt)
    INTERRUPT_HANDLER(215, handleInterrupt)
    INTERRUPT_HANDLER(216, handleInterrupt)
    INTERRUPT_HANDLER(217, handleInterrupt)
    INTERRUPT_HANDLER(218, handleInterrupt)
    INTERRUPT_HANDLER(219, handleInterrupt)
    INTERRUPT_HANDLER(220, handleInterrupt)
    INTERRUPT_HANDLER(221, handleInterrupt)
    INTERRUPT_HANDLER(222, handleInterrupt)
    INTERRUPT_HANDLER(223, handleInterrupt)
    INTERRUPT_HANDLER(224, handleInterrupt)
    INTERRUPT_HANDLER(225, handleInterrupt)
    INTERRUPT_HANDLER(226, handleInterrupt)
    INTERRUPT_HANDLER(227, handleInterrupt)
    INTERRUPT_HANDLER(228, handleInterrupt)
    INTERRUPT_HANDLER(229, handleInterrupt)
    INTERRUPT_HANDLER(230, handleInterrupt)
    INTERRUPT_HANDLER(231, handleInterrupt)
    INTERRUPT_HANDLER(232, handleInterrupt)
    INTERRUPT_HANDLER(233, handleInterrupt)
    INTERRUPT_HANDLER(234, handleInterrupt)
    INTERRUPT_HANDLER(235, handleInterrupt)
    INTERRUPT_HANDLER(236, handleInterrupt)
    INTERRUPT_HANDLER(237, handleInterrupt)
    INTERRUPT_HANDLER(238, handleInterrupt)
    INTERRUPT_HANDLER(239, handleInterrupt)
    INTERRUPT_HANDLER(240, handleInterrupt)
    INTERRUPT_HANDLER(241, handleInterrupt)
    INTERRUPT_HANDLER(242, handleInterrupt)
    INTERRUPT_HANDLER(243, handleInterrupt)
    INTERRUPT_HANDLER(244, handleInterrupt)
    INTERRUPT_HANDLER(245, handleInterrupt)
    INTERRUPT_HANDLER(246, handleInterrupt)
    INTERRUPT_HANDLER(247, handleInterrupt)
    INTERRUPT_HANDLER(248, handleInterrupt)
    INTERRUPT_HANDLER(249, handleInterrupt)
    INTERRUPT_HANDLER(250, handleInterrupt)
    INTERRUPT_HANDLER(251, handleInterrupt)
    INTERRUPT_HANDLER(252, handleInterrupt)
    INTERRUPT_HANDLER(253, handleInterrupt)
    INTERRUPT_HANDLER(254, handleInterrupt)
    INTERRUPT_HANDLER(255, handleInterrupt)

#pragma GCC pop_options

    uint64_t table[IDT_ENTRIES]{};
};

}

#endif
