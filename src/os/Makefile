#-------------------------------------------#
# Source files                              #
#-------------------------------------------#
STARTUP_SOURCE = ./startup.asm
CC_SOURCES = $(shell find . -name "*.cpp")
C_SOURCES = $(shell find . -name "*.c")
ASM_SOURCES = $(shell find ./kernel ./lib -name "*.asm")
LINKER_SCRIPT = ${CURDIR}/link.ld

#-------------------------------------------#
# Paths                                     #
#-------------------------------------------#
OBJDIR = ./build
DEPDIR = ./dep
HDD_PATH = ${CURDIR}/../hdd
INITRD_PATH = ${CURDIR}/../initrd
LOADERDIR = ${CURDIR}/../loader
BOOTDIR = $(LOADERDIR)/boot
MODULE_PATH = ${CURDIR}/../modules
MODULE_OUT = $(INITRD_PATH)/mod
INCLUDEDIR = ${CURDIR}

#-------------------------------------------#
# Build configuration                       #
#-------------------------------------------#
GITCOMMIT = $(shell git rev-parse --short HEAD)
GITTAG = $(shell git describe --tags --abbrev=0)
BUILDDATE = $(shell date "+%Y-%m-%d %H:%M:%S")

BUILDCONFIG = -DGITCOMMIT="$(GITCOMMIT)" -DGITTAG="$(GITTAG)" -DBUILDDATE="$(BUILDDATE)"


#-------------------------------------------#
# Compiler binaries                         #
#-------------------------------------------#
ASM = nasm
CC? = gcc
CXX? = g++

#-------------------------------------------#
# C Compiler settings                       #
#-------------------------------------------#
CFLAGS := $(CFLAGS) -m32 -Wall -Wno-write-strings -fno-stack-protector -nostdlib -fno-pic -no-pie \
					-ffreestanding -mno-sse -march=i486 -I$(INCLUDEDIR) -g $(BUILDCONFIG) #-DDEBUG

#-------------------------------------------#
# C++ Compiler settings                     #
#-------------------------------------------#
CXXFLAGS := $(CFLAGS) -Wl,--build-id=none -Wno-non-virtual-dtor \
					  -fno-threadsafe-statics -fno-use-cxa-atexit -fno-rtti -fno-exceptions -std=c++17

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
MODULES := hello random zero fat vesa
MODULE_SOURCES = $(MODULES:%=$(MODULE_PATH)/%)
MODULE_BINARIES = $(addprefix $(MODULE_OUT)/,$(MODULES:%=%.ko))

#-------------------------------------------#
# QEMU and KVM defaults                     #
#-------------------------------------------#
QEMU_BIN = qemu-system-i386 -cpu 486,+sse,+sse2
KVM_BIN = qemu-system-i386 -enable-kvm
GRUB_BIN = grub-mkrescue /usr/lib/grub/i386-pc

QEMU_DEFAULT_PARAMS = -boot d -cdrom $(OBJDIR)/hhuOS.iso -m 1G -k de -soundhw pcspk -vga std -monitor stdio \
					  -fda fdd0.img \
					  -readconfig usb.cfg \
					  -drive format=raw,file=hdd0.img,if=none,id=disk0 \
					  -device ich9-ahci,id=ahci \
					  -device ide-drive,drive=disk0,bus=ahci.0 \
					  -rtc base=localtime,clock=host

#-------------------------------------------#
# Build dependencies                        #
#-------------------------------------------#
VPATH = $(sort $(dir $(STARTUP_SOURCE) $(CC_SOURCES) $(C_SOURCES) $(ASM_SOURCES)))

FIRST_OBJECT = $(addprefix $(OBJDIR)/,$(patsubst %.asm,_%.o, $(notdir $(STARTUP_SOURCE))))

C_OBJECTS = $(notdir $(C_SOURCES:.c=.o))
CC_OBJECTS = $(notdir $(CC_SOURCES:.cpp=.o))

DEP_FILES = $(patsubst %.o,$(DEPDIR)/%.d,$(C_OBJECTS))
DEP_FILES += $(patsubst %.o,$(DEPDIR)/%.d,$(CC_OBJECTS))

ASM_OBJECTS = $(patsubst %.asm,_%.o, $(notdir $(ASM_SOURCES)))

OBJPRE = $(addprefix $(OBJDIR)/,$(ASM_OBJECTS) $(C_OBJECTS) $(CC_OBJECTS))

#-------------------------------------------#
# Default targets                           #
#-------------------------------------------#
all: iso fdd0.img hdd0.img

#-------------------------------------------#
# Floppy disk image                         #
#-------------------------------------------#
fdd0.img:
	@echo "FDD		fdd0.img"
	$(VERBOSE) rm -f fdd0.img > /dev/null
	$(VERBOSE) mkfs.msdos -C fdd0.img 1440 > /dev/null

#-------------------------------------------#
# Hard drive image                          #
#-------------------------------------------#
hdd0.img: ../hdd/* modules
	@echo "HDD		hdd0.img"
	$(VERBOSE) bash ./mkhdd.sh

#-------------------------------------------#
# Dependencies                              #
#-------------------------------------------#
$(DEPDIR)/%.d : %.c
	@echo "DEP		$(@F)"
	@if test \( ! \( -d $(@D) \) \) ;then mkdir -p $(@D);fi
	$(VERBOSE) $(CC) $(CFLAGS) -MM -MT $(OBJDIR)/$*.o -MF $@ $<

$(DEPDIR)/%.d : %.cpp
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

$(OBJDIR)/%.o : %.cpp
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
	$(VERBOSE) $(CXX) $(CXXFLAGS) -T $(LINKER_SCRIPT) -o $(OBJDIR)/system $(FIRST_OBJECT) $(OBJPRE)

#-------------------------------------------#
# Compiled modules                          #
#-------------------------------------------#
$(MODULE_OUT)/%.ko:
	@if test \( ! \( -d $(@D) \) \) ;then mkdir -p $(@D);fi
	$(VERBOSE) $(MAKE) --no-print-directory -C ../modules/build M=$(MODULE_PATH)/$(basename $(@F)) module

modules: $(MODULE_BINARIES)

#-------------------------------------------#
# Music files                               #
#-------------------------------------------#
music:
	$(VERBOSE) $(MAKE) --no-print-directory -C ../music

#-------------------------------------------#
# Initial Ramdisk                           #
#-------------------------------------------#
$(BOOTDIR)/hhuOS.initrd : modules music
	@echo "TAR		$(@F)"
	@if test \( ! \( -d $(@D) \) \) ;then mkdir -p $(@D);fi
	$(VERBOSE) $(TAR) -C $(INITRD_PATH) --xform s:'./':: -cf $@ ./

initrd : $(BOOTDIR)/hhuOS.initrd

#-------------------------------------------#
# Kernel image                              #
#-------------------------------------------#
$(BOOTDIR)/hhuOS.bin : $(OBJDIR)/system
	@echo "KERNEL		$(@F)"
	@if test \( ! \( -d $(@D) \) \) ;then mkdir -p $(@D);fi
	$(VERBOSE) $(COPY) $< $@

kernel : $(BOOTDIR)/hhuOS.bin
#
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
qemu: $(OBJDIR)/hhuOS.iso modules fdd0.img hdd0.img
	$(QEMU_BIN) $(QEMU_DEFAULT_PARAMS)

qemu64: $(OBJDIR)/hhuOS.iso modules fdd0.img hdd0.img
	qemu-system-x86_64 $(QEMU_DEFAULT_PARAMS)

kvm: $(OBJDIR)/hhuOS.iso modules fdd0.img hdd0.img
	$(KVM_BIN) $(QEMU_DEFAULT_PARAMS)

#-------------------------------------------#
# Debugging                                 #
#-------------------------------------------#
qemu-gdb: $(OBJDIR)/hhuOS.iso modules fdd0.img hdd0.img
	$(VERBOSE) echo "set architecture i386" > /tmp/gdbcommands.$(shell id -u)
	$(VERBOSE) echo "set disassembly-flavor intel" >> /tmp/gdbcommands.$(shell id -u)
	$(VERBOSE) echo "break main" >> /tmp/gdbcommands.$(shell id -u)
	$(VERBOSE) echo "target remote 127.0.0.1:1234" >> /tmp/gdbcommands.$(shell id -u)
	$(VERBOSE) echo "continue" >> /tmp/gdbcommands.$(shell id -u)
	($(QEMU_BIN) -s -S $(QEMU_DEFAULT_PARAMS)) &

qemu-gdb-startup: $(OBJDIR)/hhuOS.iso modules fdd0.img hdd0.img
	$(VERBOSE) echo "set architecture i8086" > /tmp/gdbcommands.$(shell id -u)
	$(VERBOSE) echo "set disassembly-flavor intel" >> /tmp/gdbcommands.$(shell id -u)
	$(VERBOSE) echo "target remote 127.0.0.1:1234" >> /tmp/gdbcommands.$(shell id -u)
	$(VERBOSE) echo "break *(startup - 0xC0000000)" >> /tmp/gdbcommands.$(shell id -u)
	$(VERBOSE) echo "continue" >> /tmp/gdbcommands.$(shell id -u)
	($(QEMU_BIN) -s -S $(QEMU_DEFAULT_PARAMS)) &

kvm-gdb: $(OBJDIR)/hhuOS.iso modules fdd0.img hdd0.img
	$(VERBOSE) echo "set architecture i386" > /tmp/gdbcommands.$(shell id -u)
	$(VERBOSE) echo "set disassembly-flavor intel" >> /tmp/gdbcommands.$(shell id -u)
	$(VERBOSE) echo "break main" >> /tmp/gdbcommands.$(shell id -u)
	$(VERBOSE) echo "target remote 127.0.0.1:1234" >> /tmp/gdbcommands.$(shell id -u)
	$(VERBOSE) echo "continue" >> /tmp/gdbcommands.$(shell id -u)
	($(KVM_BIN) -s -S $(QEMU_DEFAULT_PARAMS)) &

gdb:
	gdb -x /tmp/gdbcommands.$(shell id -u) $(OBJDIR)/system

ddd:
	ddd --gdb -x /tmp/gdbcommands.$(shell id -u) $(OBJDIR)/system

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
	@echo "RM		fdd0.img"
	$(VERBOSE) rm -f  fdd0.img
	@echo "RM		hdd0.img"
	$(VERBOSE) rm -f  hdd0.img
	@echo "RM	        $(MODULE_OUT)/*"
	$(VERBOSE) rm -f  $(MODULE_OUT)/*
	$(VERBOSE) for module in $(MODULES) ; do \
		$(MAKE) --no-print-directory -C ../modules/build M=$(MODULE_PATH)/$$module clean ; \
	done
	$(VERBOSE) $(MAKE) --no-print-directory -C ../music clean

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
