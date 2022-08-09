/*
 * Copyright (C) 2018-2022 Heinrich-Heine-Universitaet Duesseldorf,
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

#include "LvglDriver.h"
#include "lib/util/math/Math.h"
#include "lib/interface.h"

extern "C" {
uint64_t __udivdi3(uint64_t first, uint64_t second) {
    return static_cast<uint32_t>(first) / static_cast<uint32_t>(second);
}

uint64_t __umoddi3(uint64_t first, uint64_t second) {
    return static_cast<uint32_t>(first) % static_cast<uint32_t>(second);
}

void* malloc(uint32_t size) {
    return ::allocateMemory(size);
}

void free(void *pointer) {
    ::freeMemory(pointer);
}

void* realloc(void *pointer, uint32_t size) {
    return ::reallocateMemory(pointer, size);
}
}

uint32_t strlen(const char *str) noexcept {
    return Util::Memory::Address<uint32_t>(str).stringLength();
}

char* strcpy(char *dest, const char *src) noexcept {
    Util::Memory::Address<uint32_t>(dest).copyString(Util::Memory::Address<uint32_t>(src));
    return dest;
}

int32_t strcmp(const char *str1, const char *str2) noexcept {
    return Util::Memory::Address<uint32_t>(str1).compareString(Util::Memory::Address<uint32_t>(str2));
}

int32_t memcmp(const void *str1, const void *str2, uint32_t n) {
    return Util::Memory::Address<uint32_t>(str1).compareRange(Util::Memory::Address<uint32_t>(str2), n);
}

char* strcat(char *dest, const char *src) {
    auto targetAddress = Util::Memory::Address<uint32_t>(dest);
    targetAddress = targetAddress.add(targetAddress.stringLength());
    targetAddress.copyString(Util::Memory::Address<uint32_t>(src));
    return dest;
}

LvglDriver::LvglDriver(Util::Graphic::LinearFrameBuffer &lfb) :
        bufferSize(lfb.getPitch() * lfb.getResolutionY()),
        colorBuffer(new lv_color_t[bufferSize]), lfb(lfb),
        lfbAddress(*Util::Memory::Address<uint32_t>::createAcceleratedAddress(lfb.getBuffer().get(), useMmx)),
        colorBufferAddress(colorBuffer) {
    lv_disp_draw_buf_init(&displayBuffer, colorBuffer, nullptr, lfb.getResolutionX() * lfb.getResolutionY());
}

LvglDriver::~LvglDriver() {
    delete[] colorBuffer;
    delete &lfbAddress;
}

void LvglDriver::initialize() {
    lv_disp_drv_init(&driver);
    driver.draw_buf = &displayBuffer;
    driver.hor_res = static_cast<int16_t>(lfb.getResolutionX());
    driver.ver_res = static_cast<int16_t>(lfb.getResolutionY());
    driver.full_refresh = true;
    driver.user_data = this;
    driver.flush_cb = &flush;
    lv_disp_drv_register(&driver);
}

void LvglDriver::flush(struct _lv_disp_drv_t *displayDriver, const lv_area_t *area, lv_color_t *pixels) {
    auto &driver = *reinterpret_cast<LvglDriver*>(displayDriver->user_data);
    driver.flush();
    lv_disp_flush_ready(displayDriver);
}

void LvglDriver::flush() {
    lfbAddress.copyRange(colorBufferAddress, bufferSize);
    if (useMmx) Util::Math::Math::endMmx();
}
