# -----------------------------------------------------------------------------
#
#                              M A K E F I L E
#
# zum HHUos der Lehrveranstaltung 'Betriebssystem-Entwicklung'.
# -----------------------------------------------------------------------------
#
# Compilieren: ‘make'
# Ausfuehren mit Qemu: ‘make qemu’
# GDB mit Qemu: ‘make qemu-gdb &’ und danach ‘make gdb’
# DDD mit Qemu: ‘make qemu-gdb &’ und danach ‘make ddd’
#
# Aufraeumen mit: ‘make clean’
#
# Erzeugen einer bootfaehigen Diskette mit: ‘make bootdisk’
# Erzeugen eines bootfaehigen USB-Sticks mit: ‘make bootdisk-usb’
# Erzeugen eines bootfaehigen Festplatte mit: ‘make bootdisk-hd’
#
# -----------------------------------------------------------------------------
# Liste der Quelltexte:

STARTUP_SOURCE = ./startup.asm
CC_SOURCES = $(shell find . -name "*.cc")
C_SOURCES = $(shell find . -name "*.c")
ASM_SOURCES = $(shell find ./kernel ./lib -name "*.asm")

OBJDIR = ./build
DEPDIR = ./dep
HDD_PATH = ../hdd
LOADERDIR = ../loader
BOOTDIR = $(LOADERDIR)/boot

# Einstellungen in Abhaengigkeit vom Generierungssystem:
#
#    ASMOBJFORMAT: Ausgabeformat fuer den Assembler. Das Format muss dem
#                  jeweiligen Format des verwendeten C++ Compilers angepasst
#                  werde, damit gemischte Objektdateien gelinkt werden koennen.
#    OBJDIR:       Verzeichnis, in dem die Objektdateien abgelegt werden
#                  sollen.
#    DEPDIR:       Verzeichnis, in dem die Abhaengigkeitsdateien abgelegt werden
#                  sollen
#    DELETE:       Name des Kommandos zum Loeschen von Dateien
#    ASM:          Zu benutzender Assembler
#    CC/CXX:       Zu benutzender C/C++-Compiler
#    CFLAGS:       Flags fuer den C-Compileraufruf
#    CXXFLAGS:     Flags fuer den C++-Compileraufruf

VERBOSE = @
ASMOBJFORMAT = elf
DELETE = rm
COPY = cp
NM = nm
ASM = nasm
CC ?= gcc
CXX ?= g++
MAKE = make
CFLAGS := $(CFLAGS) -m32 -Wall -Wno-write-strings -fno-stack-protector -nostdlib -ffreestanding -mno-sse -I. -g #-DDEBUG
CXXFLAGS := $(CFLAGS) -Wno-non-virtual-dtor -fno-threadsafe-statics -fno-use-cxa-atexit -fno-rtti -fno-exceptions -std=c++11
ASMFLAGS := $(ASMFLAGS) -f $(ASMOBJFORMAT) -F stabs
SERIAL_TTY = /dev/pts/1

# --------------------------------------------------------------------------
# QEMU and KVM defaults

QEMU_BIN = qemu-system-i386 -cpu core2duo
#QEMU_BIN = qemu-system-x86_64
KVM_BIN = qemu-system-i386 -enable-kvm -no-kvm-irqchip
#KVM_BIN = kvm
GRUB_BIN = grub-mkrescue /usr/lib/grub/i386-pc

QEMU_DEFAULT_PARAMS = -boot d -cdrom $(OBJDIR)/hhuOS.iso -k en-us -soundhw pcspk -vga std -monitor stdio \
		-readconfig usb.cfg \
		-drive format=raw,file=hdd.img,if=none,id=disk0 \
		-device ich9-ahci,id=ahci \
		-device ide-drive,drive=disk0,bus=ahci.0 \
		-rtc base=localtime,clock=host \
		-m 1G

# -------------------------------------------------------------------------
# Namen der Unterverzeichnisse mit den Quelltexten

VPATH = $(sort $(dir $(STARTUP_SOURCE) $(CC_SOURCES) $(C_SOURCES) $(ASM_SOURCES)))

# -------------------------------------------------------------------------
# Listen mit den Objektdateien, die beim Kompilieren entstehen:

FIRST_OBJECT = $(addprefix $(OBJDIR)/,$(patsubst %.asm,_%.o, $(notdir $(STARTUP_SOURCE))))
C_OBJECTS = $(notdir $(C_SOURCES:.c=.o))
CC_OBJECTS = $(notdir $(CC_SOURCES:.cc=.o))

DEP_FILES = $(patsubst %.o,$(DEPDIR)/%.d,$(C_OBJECTS))
DEP_FILES += $(patsubst %.o,$(DEPDIR)/%.d,$(CC_OBJECTS))

ASM_OBJECTS = $(patsubst %.asm,_%.o, $(notdir $(ASM_SOURCES)))
OBJPRE = $(addprefix $(OBJDIR)/,$(ASM_OBJECTS) $(C_OBJECTS) $(CC_OBJECTS))

# --------------------------------------------------------------------------
# Default targets (einfaches Image, Image fuer USB Sticks, Image fuer VMWare
# und Boot CDs)

all: $(OBJDIR)/hhuOS.iso hdd.img

# --------------------------------------------------------------------------
# Regeln zur Erzeugung der Abhaengigkeitsdateien

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

$(DEPDIR)/%.d : gitversion.c %.c
	@echo "DEP		$@"
	@if test \( ! \( -d $(@D) \) \) ;then mkdir -p $(@D);fi
	$(VERBOSE) $(CC) $(CFLAGS) -MM -MT $(OBJDIR)/$*.o -MF $@ $<

$(DEPDIR)/%.d : %.cc
	@echo "DEP		$@"
	@if test \( ! \( -d $(@D) \) \) ;then mkdir -p $(@D);fi
	$(VERBOSE) $(CXX) $(CXXFLAGS) -MM -MT $(OBJDIR)/$*.o -MF $@ $<

# --------------------------------------------------------------------------
# Regeln zur Erzeugung der Objektdateien

$(OBJDIR)/%.o : %.c
	@echo "CC		$@"
	@if test \( ! \( -d $(@D) \) \) ;then mkdir -p $(@D);fi
	$(VERBOSE) $(CC) -c $(CFLAGS) -o $@ $<

$(OBJDIR)/%.o : %.cc
	@echo "CXX		$@"
	@if test \( ! \( -d $(@D) \) \) ;then mkdir -p $(@D);fi
	$(VERBOSE) $(CXX) -c $(CXXFLAGS) -o $@ $<

$(OBJDIR)/_%.o : %.asm
	@echo "ASM		$@"
	@if test \( ! \( -d $(@D) \) \) ;then mkdir -p $(@D);fi
	$(VERBOSE) $(ASM) $(ASMFLAGS) -o $@ $<

# --------------------------------------------------------------------------
# Gelinktes System

$(OBJDIR)/system: $(FIRST_OBJECT) $(OBJPRE)
	@echo "LD		$@"
	@if test \( ! \( -d $(@D) \) \) ;then mkdir -p $(@D);fi
	$(VERBOSE) $(CXX) $(CXXFLAGS) -static -T sections -o $(OBJDIR)/system $(FIRST_OBJECT) $(OBJPRE)

# --------------------------------------------------------------------------
# 'system.img' enthaelt die zu ladenden Sections des eigentlichen Systems

$(OBJDIR)/system.img : $(OBJDIR)/system
	@echo "OBJCOPY		$@"
	@if test \( ! \( -d $(@D) \) \) ;then mkdir -p $(@D);fi
	$(VERBOSE) objcopy -O binary $< $@

# --------------------------------------------------------------------------
# 'hhuOS.iso' besteht aus dem 'system', welches mit grub gestartet wird

$(BOOTDIR)/hhuOS.bin : $(OBJDIR)/system
	@echo "COPY		$@"
	@if test \( ! \( -d $(@D) \) \) ;then mkdir -p $(@D);fi
	$(VERBOSE) $(COPY) $< $@

$(OBJDIR)/hhuOS.iso : $(BOOTDIR)/hhuOS.bin
	@echo "BUILD		$@"
	@if test \( ! \( -d $(@D) \) \) ;then mkdir -p $(@D);fi
	$(VERBOSE) $(GRUB_BIN) -o $@ $(LOADERDIR)

# --------------------------------------------------------------------------
# 'clean' loescht das generierte System, die Objektdateien und die
# Abhaengigkeitsdateien

clean:
	@echo "RM		$(OBJDIR)"
	$(VERBOSE) rm -rf $(OBJDIR)
	@echo "RM		$(DEPDIR)"
	$(VERBOSE) rm -rf $(DEPDIR)
	@echo "RM		$(BOOTDIR)/hhuOS.bin"
	$(VERBOSE) rm -f  $(BOOTDIR)/hhuOS.bin
	rm -f gitversion.c

bootdisk: $(OBJDIR)/hhuOS.iso
	sudo dd if=$< of=$(of) bs=512K && sync

bootdisk-commit:
	git checkout $(commit)
	$(MAKE) clean
	$(MAKE)
	sudo dd if=$(OBJDIR)/hhuOS.iso of=$(of) bs=512K && sync
	git checkout master


# --------------------------------------------------------------------------
# 'qemu' ruft den qemu-Emulator mit dem System auf.

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

# --------------------------------------------------------------------------
# 'qemu-gdb' ruft den qemu-Emulator mit aktiviertem GDB-Stub mit dem System
# auf, sodass es per GDB oder DDD inspiziert werden kann.

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

# --------------------------------------------------------------------------
# 'gdb' startet den GDB-Debugger und verbindet sich mit dem GDB-Stub des vorher
# gestarteten Qemu.

gdb:
	gdb -x /tmp/gdbcommands.$(shell id -u) $(OBJDIR)/system

# --------------------------------------------------------------------------
# 'ddd' startet den DDD-Debugger und verbindet sich mit dem GDB-Stub des vorher
# gestarteten Qemu.

ddd:
	ddd --gdb -x /tmp/gdbcommands.$(shell id -u) $(OBJDIR)/system

# --------------------------------------------------------------------------
# Mount hdd.img on the host system

mount:
	udisksctl loop-setup --file ./hdd.img --offset 1048576 --size 126877696

# --------------------------------------------------------------------------
# Mount usb.img on the host system

mount-usb:
	udisksctl loop-setup --file ./usb.img --offset 1048576 --size 62914560

# --------------------------------------------------------------------------
# Einbindung der Abhaengigkeitsdateien

ifneq ($(MAKECMDGOALS),clean)
-include $(DEP_FILES)
endif

.PHONY: clean bootdisk gdb ddd
