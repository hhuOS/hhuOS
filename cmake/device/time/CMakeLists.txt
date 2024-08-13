# Copyright (C) 2018-2024 Heinrich-Heine-Universitaet Duesseldorf,
# Institute of Computer Science, Department Operating Systems
# Burak Akguel, Christian Gesse, Fabian Ruhland, Filip Krakowski, Michael Schoettner
#
#
# This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public
# License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any
# later version.
#
# This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied
# warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
# details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>

cmake_minimum_required(VERSION 3.14)
 
target_sources(device PUBLIC
        ${HHUOS_SRC_DIR}/device/time/acpi/AcpiTimer.cpp
        ${HHUOS_SRC_DIR}/device/time/apic/ApicTimer.cpp
        ${HHUOS_SRC_DIR}/device/time/hpet/Hpet.cpp
        ${HHUOS_SRC_DIR}/device/time/hpet/SystemTimerInterruptHandler.cpp
        ${HHUOS_SRC_DIR}/device/time/hpet/Timer.cpp
        ${HHUOS_SRC_DIR}/device/time/pit/Pit.cpp
        ${HHUOS_SRC_DIR}/device/time/rtc/AlarmRunnable.cpp
        ${HHUOS_SRC_DIR}/device/time/rtc/Cmos.cpp
        ${HHUOS_SRC_DIR}/device/time/rtc/Rtc.cpp)
