#-------------------------------------------#
# Source files                              #
#-------------------------------------------#
STARTUP_SOURCE = ./startup.asm
CC_SOURCES = $(shell find . -name "*.cc")
C_SOURCES = $(shell find . -name "*.c")
ASM_SOURCES = $(shell find ./kernel ./lib -name "*.asm")

#-------------------------------------------#
# Paths                                     #
#-------------------------------------------#
OBJDIR = ./build
DEPDIR = ./dep
HDD_PATCH = ${CURDIR}/../hdd
INITRD_PATH = ${CURDIR}/../initrd
LOADERDIR = ${CURDIR}/../loader
BOOTDIR = $(LOADERDIR)/boot
SERIAL_TTY = /dev/pts/1
MODULE_PATH = ${CURDIR}/../modules
MODULE_OUT = $(INITRD_PATH)/mod
INCLUDEDIR = ${CURDIR}

#-------------------------------------------#
# Compiler binaries                         #
#-------------------------------------------#
ASM = nasm
CC = gcc
CXX = g++

#-------------------------------------------#
# C Compiler settings                       #
#-------------------------------------------#
CFLAGS := $(CFLAGS) -m32 -Wall -Wno-write-strings -Werror -fno-stack-protector -nostdlib \
								-ffreestanding -mno-sse -I$(INCLUDEDIR) -g #-DDEBUG

#-------------------------------------------#
# C++ Compiler settings                     #
#-------------------------------------------#
CXXFLAGS := $(CFLAGS) -Wno-non-virtual-dtor -fno-threadsafe-statics -fno-use-cxa-atexit \
 								-fno-rtti -fno-exceptions -std=c++11

#-------------------------------------------#
# ASM Compiler settings                     #
#-------------------------------------------#
ASMOBJFORMAT = elf
ASMFLAGS := $(ASMFLAGS) -f $(ASMOBJFORMAT) -F stabs

#-------------------------------------------#
# Utilities                                 #
#-------------------------------------------#
VERBOSE = @
DELETE = rm
COPY = cp
NM = nm
TAR = tar
MAKE = make

#-------------------------------------------#
# Modules (see src/modules)                 #
#-------------------------------------------#
MODULES := hello random zero
MODULE_SOURCES = $(MODULES:%=$(MODULE_PATH)/%)

#-------------------------------------------#
# QEMU and KVM defaults                     #
#-------------------------------------------#
QEMU_BIN = qemu-system-i386 -cpu core2duo
KVM_BIN = qemu-system-i386 -enable-kvm -no-kvm-irqchip
GRUB_BIN = grub-mkrescue /usr/lib/grub/i386-pc

QEMU_DEFAULT_PARAMS = -boot d -cdrom $(OBJDIR)/hhuOS.iso -k en-us -soundhw pcspk -vga std -monitor stdio \
					  -readconfig usb.cfg \
					  -drive format=raw,file=hdd.img,if=none,id=disk0 \
					  -device ich9-ahci,id=ahci \
					  -device ide-drive,drive=disk0,bus=ahci.0 \
					  -rtc base=localtime,clock=host \
					  -m 1G

#-------------------------------------------#
# Build dependencies                        #
#-------------------------------------------#
VPATH = $(sort $(dir $(STARTUP_SOURCE) $(CC_SOURCES) $(C_SOURCES) $(ASM_SOURCES)))

FIRST_OBJECT = $(addprefix $(OBJDIR)/,$(patsubst %.asm,_%.o, $(notdir $(STARTUP_SOURCE))))

C_OBJECTS = $(notdir $(C_SOURCES:.c=.o))
CC_OBJECTS = $(notdir $(CC_SOURCES:.cc=.o))

DEP_FILES = $(patsubst %.o,$(DEPDIR)/%.d,$(C_OBJECTS))
DEP_FILES += $(patsubst %.o,$(DEPDIR)/%.d,$(CC_OBJECTS))

ASM_OBJECTS = $(patsubst %.asm,_%.o, $(notdir $(ASM_SOURCES)))

OBJPRE = $(addprefix $(OBJDIR)/,$(ASM_OBJECTS) $(C_OBJECTS) $(CC_OBJECTS))

#-------------------------------------------#
# Default targets                           #
#-------------------------------------------#
all: $(OBJDIR)/hhuOS.iso hdd.img

#-------------------------------------------#
# Hard drive image                          #
#-------------------------------------------#
hdd.img: ../hdd/*
	$(VERBOSE) qemu-img create hdd.img 32M
	$(VERBOSE) udisksctl loop-setup --file hdd.img --offset 0 --size 33554432
	$(VERBOSE) (echo n; echo p; echo 1;echo ""; echo ""; echo t; echo 58; echo w) | sudo fdisk /dev/loop0
	$(VERBOSE) sudo mkfs.vfat /dev/loop0p1
	$(VERBOSE) sudo mount /dev/loop0p1 /mnt
	$(VERBOSE) sudo cp -r ../hdd/* /mnt
	$(VERBOSE) sudo umount /mnt
	$(VERBOSE) udisksctl loop-delete -b /dev/loop0

gitversion.c: ../../.git/HEAD ../../.git/index
	@echo "const char *gitversion = \"$(shell git rev-parse --short HEAD)\";" > $@

#-------------------------------------------#
# Dependencies                              #
#-------------------------------------------#
$(DEPDIR)/%.d : gitversion.c %.c
	@echo "DEP		$(@F)"
	@if test \( ! \( -d $(@D) \) \) ;then mkdir -p $(@D);fi
	$(VERBOSE) $(CC) $(CFLAGS) -MM -MT $(OBJDIR)/$*.o -MF $@ $<

$(DEPDIR)/%.d : %.cc
	@echo "DEP		$(@F)"
	@if test \( ! \( -d $(@D) \) \) ;then mkdir -p $(@D);fi
	$(VERBOSE) $(CXX) $(CXXFLAGS) -MM -MT $(OBJDIR)/$*.o -MF $@ $<

#-------------------------------------------#
# Object files                              #
#-------------------------------------------#
$(OBJDIR)/%.o : %.c
	@echo "CC		$(@F)"
	@if test \( ! \( -d $(@D) \) \) ;then mkdir -p $(@D);fi
	$(VERBOSE) $(CC) -c $(CFLAGS) -o $@ $<

$(OBJDIR)/%.o : %.cc
	@echo "CXX		$(@F)"
	@if test \( ! \( -d $(@D) \) \) ;then mkdir -p $(@D);fi
	$(VERBOSE) $(CXX) -c $(CXXFLAGS) -o $@ $<

$(OBJDIR)/_%.o : %.asm
	@echo "ASM		$(@F)"
	@if test \( ! \( -d $(@D) \) \) ;then mkdir -p $(@D);fi
	$(VERBOSE) $(ASM) $(ASMFLAGS) -o $@ $<

#-------------------------------------------#
# Linked system                             #
#-------------------------------------------#
$(OBJDIR)/system: $(FIRST_OBJECT) $(OBJPRE)
	@echo "LD		$(@F)"
	@if test \( ! \( -d $(@D) \) \) ;then mkdir -p $(@D);fi
	$(VERBOSE) $(CXX) $(CXXFLAGS) -static -T sections -o $(OBJDIR)/system $(FIRST_OBJECT) $(OBJPRE)

#-------------------------------------------#
# Compiled modules                          #
#-------------------------------------------#
$(MODULE_SOURCES):
	$(VERBOSE) $(MAKE) --no-print-directory -C $@

modules: $(MODULE_SOURCES)

#-------------------------------------------#
# Initial Ramdisk                           #
#-------------------------------------------#
$(BOOTDIR)/hhuOS.initrd : modules
	@echo "TAR		$(@F)"
	@if test \( ! \( -d $(@D) \) \) ;then mkdir -p $(@D);fi
	$(VERBOSE) $(TAR) -cf $@ $(INITRD_PATH)

initrd : $(BOOTDIR)/hhuOS.initrd

#-------------------------------------------#
# Kernel image                              #
#-------------------------------------------#
$(BOOTDIR)/hhuOS.bin : $(OBJDIR)/system
	@echo "KERNEL		$(@F)"
	@if test \( ! \( -d $(@D) \) \) ;then mkdir -p $(@D);fi
	$(VERBOSE) $(COPY) $< $@

kernel : $(BOOTDIR)/hhuOS.bin

#-------------------------------------------#
# Kernel image with GRUB                    #
#-------------------------------------------#
$(OBJDIR)/hhuOS.iso : kernel initrd
	@echo "BUILD		$(@F)"
	@if test \( ! \( -d $(@D) \) \) ;then mkdir -p $(@D);fi
	$(VERBOSE) $(GRUB_BIN) -o $@ $(LOADERDIR)

iso : $(OBJDIR)/hhuOS.iso

#-------------------------------------------#
# Bootdisk creation                         #
#-------------------------------------------#
bootdisk: $(OBJDIR)/hhuOS.iso
	sudo dd if=$< of=$(of) bs=512K && sync

bootdisk-commit:
	git checkout $(commit)
	$(MAKE) clean
	$(MAKE)
	sudo dd if=$(OBJDIR)/hhuOS.iso of=$(of) bs=512K && sync
	git checkout master

#-------------------------------------------#
# QEMU / KVM                                #
#-------------------------------------------#
qemu: $(OBJDIR)/hhuOS.iso hdd.img
	$(QEMU_BIN) -m 1G $(QEMU_DEFAULT_PARAMS)

qemu64: $(OBJDIR)/hhuOS.iso
	qemu-system-x86_64 -m 1G $(QEMU_DEFAULT_PARAMS)

qemu-serial: $(OBJDIR)/hhuOS.iso hdd.img
#	xfce4-terminal -e 'sleep 10d'
	$(QEMU_BIN) -m 1G $(QEMU_DEFAULT_PARAMS) \
		-chardev tty,id=ptshere,path=$(SERIAL_TTY) \
		-device isa-serial,chardev=ptshere

kvm: $(OBJDIR)/hhuOS.iso hdd.img
	$(KVM_BIN) -m 1G $(QEMU_DEFAULT_PARAMS)

#-------------------------------------------#
# Debugging                                 #
#-------------------------------------------#
qemu-gdb: $(OBJDIR)/hhuOS.iso hdd.img
	$(VERBOSE) echo "set architecture i386" > /tmp/gdbcommands.$(shell id -u)
	$(VERBOSE) echo "set disassembly-flavor intel" >> /tmp/gdbcommands.$(shell id -u)
	$(VERBOSE) echo "break main" >> /tmp/gdbcommands.$(shell id -u)
	$(VERBOSE) echo "target remote 127.0.0.1:1234" >> /tmp/gdbcommands.$(shell id -u)
	$(VERBOSE) echo "continue" >> /tmp/gdbcommands.$(shell id -u)
	($(QEMU_BIN) -m 1G -s -S $(QEMU_DEFAULT_PARAMS)) &

qemu-gdb-bootloader: $(OBJDIR)/hhuOS.iso hdd.img
	$(VERBOSE) echo "set architecture i8086" > /tmp/gdbcommands.$(shell id -u)
	$(VERBOSE) echo "set disassembly-flavor intel" >> /tmp/gdbcommands.$(shell id -u)
	$(VERBOSE) echo "target remote 127.0.0.1:1234" >> /tmp/gdbcommands.$(shell id -u)
	$(VERBOSE) echo "break on_paging_enabled" >> /tmp/gdbcommands.$(shell id -u)
	$(VERBOSE) echo "continue" >> /tmp/gdbcommands.$(shell id -u)
	$(QEMU_BIN) -m 1G -s -S $(QEMU_DEFAULT_PARAMS)

kvm-gdb: $(OBJDIR)/hhuOS.iso hdd.img
	$(VERBOSE) echo "break main" > /tmp/gdbcommands.$(shell id -u)
	$(VERBOSE) echo "set disassembly-flavor intel" >> /tmp/gdbcommands.$(shell id -u)
	$(VERBOSE) echo "target remote 127.0.0.1:1234" >> /tmp/gdbcommands.$(shell id -u)
	$(VERBOSE) echo "continue" >> /tmp/gdbcommands.$(shell id -u)
	$(KVM_BIN) -m 1G -s -S $(QEMU_DEFAULT_PARAMS)

gdb:
	gdb -x /tmp/gdbcommands.$(shell id -u) $(OBJDIR)/system

ddd:
	ddd --gdb -x /tmp/gdbcommands.$(shell id -u) $(OBJDIR)/system

#-------------------------------------------#
# Mount hdd.img on the host system          #
#-------------------------------------------#
mount-hdd:
	udisksctl loop-setup --file ./hdd.img --offset 1048576 --size 126877696

#-------------------------------------------#
# Mount usb.img on the host system          #
#-------------------------------------------#
mount-usb:
	udisksctl loop-setup --file ./usb.img --offset 1048576 --size 62914560

#-------------------------------------------#
# Clean                                     #
#-------------------------------------------#
clean:
	@echo "RM		$(OBJDIR)"
	$(VERBOSE) rm -rf $(OBJDIR)
	@echo "RM		$(DEPDIR)"
	$(VERBOSE) rm -rf $(DEPDIR)
	@echo "RM		$(BOOTDIR)/hhuOS.bin"
	$(VERBOSE) rm -f  $(BOOTDIR)/hhuOS.bin
	@echo "RM		$(BOOTDIR)/hhuOS.initrd"
	$(VERBOSE) rm -f  $(BOOTDIR)/hhuOS.initrd
	@echo "RM		$(MODULE_OUT)/*"
	$(VERBOSE) rm -f  $(MODULE_OUT)/*
	@echo "RM		gitversion.c"
	$(VERBOSE) rm -f gitversion.c

#-------------------------------------------#
# Exports                                   #
#-------------------------------------------#
export MODULE_OUT
export INCLUDEDIR
export ASM
export CC
export CXX
export VERBOSE
export DELETE
export COPY
export NM
export TAR
export MAKE

#-------------------------------------------#
# Include dependencies                      #
#-------------------------------------------#
ifneq ($(MAKECMDGOALS),clean)
-include $(DEP_FILES)
endif

.PHONY: clean bootdisk gdb ddd $(MODULE_SOURCES)
