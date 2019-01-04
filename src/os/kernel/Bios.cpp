/*
 * Copyright (C) 2018 Burak Akguel, Christian Gesse, Fabian Ruhland, Filip Krakowski, Michael Schoettner
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
 */

#include <kernel/memory/MemLayout.h>
#include <kernel/services/InputService.h>
#include "kernel/cpu/Cpu.h"
#include "lib/libc/printf.h"

#include "kernel/Bios.h"
#include "Kernel.h"


// extern code in interrupt.asm
extern "C" { void bios_call(); }   

// pointer to memory for parameters
struct BIOScall_params* BC_params = (struct BIOScall_params*)VIRT_BIOS16_PARAM_BASE;

void Bios::init() {
	// pointer to memory segment for 16 bit code
    uint8_t *codeAddr = (uint8_t*)VIRT_BIOS16_CODE_MEMORY_START;

    // the assembler instructions are placed manually into the memory
    // in the following steps

    // mov eax, 5000
    *codeAddr = 0x66;   codeAddr++;
    *codeAddr = 0xB8;   codeAddr++;
    *codeAddr = 0x00;   codeAddr++;
    *codeAddr = 0x50;   codeAddr++;
    *codeAddr = 0x00;   codeAddr++;
    *codeAddr = 0x00;   codeAddr++;

    // mov [eax], esp
    *codeAddr = 0x66;   codeAddr++;
    *codeAddr = 0x67;   codeAddr++;
    *codeAddr = 0x89;   codeAddr++;
    *codeAddr = 0x20;   codeAddr++;
    
    
    
    // mov eax,cr0
    *codeAddr = 0x0F;   codeAddr++;
    *codeAddr = 0x20;   codeAddr++;
    *codeAddr = 0xC0;   codeAddr++;
    
    // and eax, 7FFEFFFE
    *codeAddr = 0x66;   codeAddr++;
    *codeAddr = 0x25;   codeAddr++;
    *codeAddr = 0xFE;   codeAddr++;
    *codeAddr = 0xFF;   codeAddr++;
    *codeAddr = 0xFE;   codeAddr++;
    *codeAddr = 0x7F;   codeAddr++;
    
    // mov cr0, eax
    *codeAddr = 0x0F;   codeAddr++;
    *codeAddr = 0x22;   codeAddr++;
    *codeAddr = 0xC0;   codeAddr++;
    
    // jmp 2400:001B flush pipeline & switch decoding unit
    // 0400:001B (0400<<4 = 4000 + 1B)
    *codeAddr = 0xEA;   codeAddr++;
    *codeAddr = 0x1B;   codeAddr++;
    *codeAddr = 0x00;   codeAddr++;
    *codeAddr = 0x00;   codeAddr++;
    *codeAddr = 0x04;   codeAddr++;
    
    // mov ds,400
    *codeAddr = 0xBA;   codeAddr++;
    *codeAddr = 0x00;   codeAddr++;
    *codeAddr = 0x04;   codeAddr++;
    
    // mov ss,dx
    *codeAddr = 0x8E;   codeAddr++;
    *codeAddr = 0xD2;   codeAddr++;
    
    // mov gs,dx
    *codeAddr = 0x8E;   codeAddr++;
    *codeAddr = 0xEA;   codeAddr++;
    
    // mov esp,2000 -> BIOS16_PARAM_BASE 0x6000
    *codeAddr = 0x66;   codeAddr++;
    *codeAddr = 0xBC;   codeAddr++;
    *codeAddr = 0x00;   codeAddr++;
    *codeAddr = 0x20;   codeAddr++;
    *codeAddr = 0x00;   codeAddr++;
    *codeAddr = 0x00;   codeAddr++;
    
    // pop ds
    *codeAddr = 0x1F;   codeAddr++;
    
    // pop es
    *codeAddr = 0x07;   codeAddr++;
    
    // pop fs
    *codeAddr = 0x0f;   codeAddr++;
    *codeAddr = 0xa1;   codeAddr++;
    
    // pop ax -> we have to pop something for symmetry
    *codeAddr = 0x58;   codeAddr++;
    
    // popad
    *codeAddr = 0x66;   codeAddr++;
    *codeAddr = 0x61;   codeAddr++;
    
    // interrupt number (written here)
    *codeAddr = 0xCD;   codeAddr++;
    *codeAddr = 0x00;   codeAddr++;
    
    // pushad
    *codeAddr = 0x66;   codeAddr++;
    *codeAddr = 0x60;   codeAddr++;
    
    // pushf
    *codeAddr = 0x9C;   codeAddr++;
    
    // push fs
    *codeAddr = 0x0f;   codeAddr++;
    *codeAddr = 0xa0;   codeAddr++;
    
    // push es
    *codeAddr = 0x06;   codeAddr++;
    
    // push ds
    *codeAddr = 0x1E;   codeAddr++;
    
    // mov eax,cr0
    *codeAddr = 0x0F;   codeAddr++;
    *codeAddr = 0x20;   codeAddr++;
    *codeAddr = 0xC0;   codeAddr++;
    
    // or eax, 00010001 (protected mode without paging)
    *codeAddr = 0x66;   codeAddr++;
    *codeAddr = 0x0D;   codeAddr++;
    *codeAddr = 0x01;   codeAddr++;
    *codeAddr = 0x00;   codeAddr++;
    *codeAddr = 0x01;   codeAddr++;
    *codeAddr = 0x00;   codeAddr++;

    // mov cr0, eax
    *codeAddr = 0x0F;   codeAddr++;
    *codeAddr = 0x22;   codeAddr++;
    *codeAddr = 0xC0;   codeAddr++;
    
    // jmp 0018:0049, flush pipeline & switch decoding
    // 0018:0049
    *codeAddr = 0xEA;   codeAddr++;
    *codeAddr = 0x49;   codeAddr++;
    *codeAddr = 0x00;   codeAddr++;
    *codeAddr = 0x18;   codeAddr++;
    *codeAddr = 0x00;   codeAddr++;
    
    // mov dx,0010
    *codeAddr = 0xBA;   codeAddr++;
    *codeAddr = 0x10;   codeAddr++;
    *codeAddr = 0x00;   codeAddr++;
    
    // mov ds,dx
    *codeAddr = 0x8E;   codeAddr++;
    *codeAddr = 0xDA;   codeAddr++;
    
    // mov es,dx
    *codeAddr = 0x8E;   codeAddr++;
    *codeAddr = 0xC2;   codeAddr++;
    
    // mov es,dx
    *codeAddr = 0x8E;   codeAddr++;
    *codeAddr = 0xE2;   codeAddr++;
    
    // mov fs,dx
    *codeAddr = 0x8E;   codeAddr++;
    *codeAddr = 0xEA;   codeAddr++;
    
    // mov ss,dx
    *codeAddr = 0x8E;   codeAddr++;
    *codeAddr = 0xD2;   codeAddr++;
    
    // mov eax, 25000
    *codeAddr = 0x66;   codeAddr++;
    *codeAddr = 0xB8;   codeAddr++;
    *codeAddr = 0x00;   codeAddr++;
    *codeAddr = 0x50;   codeAddr++;
    *codeAddr = 0x00;   codeAddr++;
    *codeAddr = 0x00;   codeAddr++;
    
    // mov esp, [eax]
    *codeAddr = 0x66;   codeAddr++;
    *codeAddr = 0x67;   codeAddr++;
    *codeAddr = 0x8B;   codeAddr++;
    *codeAddr = 0x20;   codeAddr++;
    
    // far ret
    *codeAddr = 0x66;	codeAddr++;
    *codeAddr = 0xCB;
}


void Bios::Int(int inter) {
    // get pointer to bios call segment
    uint8_t *ptr = (uint8_t*)VIRT_BIOS16_CODE_MEMORY_START;
    // write number of bios interrupt manually into the segment
    *(ptr+48) = (uint8_t)inter;
    
    // no interrupts during the bios call
    Cpu::disableInterrupts();
    // jump into assembler code
    bios_call ();
    // bios call is returned, interrupts are allowed now
    Cpu::enableInterrupts();
}

uint32_t Bios::calcPhysMemory() {

	uint32_t physMemory = 0;

	// request amount of memory
    BC_params->AX = 0xE801;
    BC_params->CX = 0;
    BC_params->DX = 0;
    Bios::Int(0x15);

    // was there a problem?
    if ( (BC_params->AX & 0xFF) == 0x86 || (BC_params->AX & 0xFF) == 0x80) {
        printf("[SYSTEMMANAGEMENT] Physical memory could not be calculated.");
        Cpu::halt ();
    } else {
        // calculate amopunt of usable physivcal memory
        physMemory = 1024 * 1024;                     // 1. MB
        physMemory += (BC_params->CX * 1024);         // 2 - 16MB
        physMemory += (BC_params->DX * 64 * 1024);    // >16MB
    }

    // if there is more than 3,75GB memory apply a cap
    if(physMemory > PHYS_MEM_CAP) {
        physMemory = PHYS_MEM_CAP;
    }

    // We need at least 10MB physical memory to run properly
    if(physMemory < 10 * 1024 * 1024){
        printf("[MEMORYMANAGEMENT] Kernel Panic: not enough RAM\n");
        Cpu::halt();
    }
    printf("[SYSTEMMANAGEMENT] Total Physical Memory: %dMB\n", physMemory/(1024*1024));

    return physMemory;
}
