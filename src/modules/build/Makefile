# Copyright (C) 2018 Burak Akguel, Christian Gesse, Fabian Ruhland, Filip Krakowski, Michael Schoettner
# Heinrich-Heine University
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

#-------------------------------------------#
# Utilities                                 #
#-------------------------------------------#
VERBOSE 			= @
DELETE 				= rm
COPY 				= cp
NM 					= nm
TAR 				= tar
MAKE 				= make

#-------------------------------------------#
# Source files                              #
#-------------------------------------------#
STARTUP_SOURCE		= ./startup.asm
CC_SOURCES 			= $(shell find $(M) -name "*.cpp")
C_SOURCES 			= $(shell find $(M) -name "*.c")
ASM_SOURCES 		= $(shell find $(M) -name "*.asm")

MODULE_NAME			= $(shell basename $(M))

#-------------------------------------------#
# Build files                               #
#-------------------------------------------#
ASM_OBJECTS 		= $(patsubst %.asm,_%.o, $(notdir $(ASM_SOURCES)))
C_OBJECTS 			= $(notdir $(C_SOURCES:.c=.o))
CC_OBJECTS 			= $(notdir $(CC_SOURCES:.cpp=.o))

DEPENDENCIES 		= $(addprefix $(M)/$(BUILDDIR)/,$(ASM_OBJECTS) $(C_OBJECTS) $(CC_OBJECTS))

#-------------------------------------------#
# Paths                                     #
#-------------------------------------------#
INCLUDEDIR			= ../../os
MODULEDIR			= ../../initrd/mod
BUILDDIR			= build

#-------------------------------------------#
# Compiler binaries                         #
#-------------------------------------------#
ASM 				= nasm
CC? 				= gcc
CXX? 				= g++

#-------------------------------------------#
# ASM Compiler settings                     #
#-------------------------------------------#
ASMOBJFORMAT 		= elf
ASMFLAGS 			= $(ASMFLAGS) -f $(ASMOBJFORMAT) -F stabs

#-------------------------------------------#
# C Compiler settings                       #
#-------------------------------------------#
CFLAGS 				= -m32 -march=i486 -nostdlib -no-pie -Wl,-r -fno-pic -Wno-write-strings -Werror \
					  -fno-stack-protector -ffreestanding -mno-sse -I. -I$(INCLUDEDIR)

#-------------------------------------------#
# C++ Compiler settings                     #
#-------------------------------------------#
CXXFLAGS 			= $(CFLAGS) -Wno-non-virtual-dtor -fno-threadsafe-statics \
								-fno-use-cxa-atexit -fno-rtti -fno-exceptions -std=c++11

#-------------------------------------------#
# Make targets                              #
#-------------------------------------------#
all: module

$(MODULEDIR)/$(MODULE_NAME).ko : $(DEPENDENCIES)
	@echo "MODULE		$(@F)"
	$(VERBOSE) $(CXX) $(CXXFLAGS) -o $@ $(CC_SOURCES)

module : $(MODULEDIR)/$(MODULE_NAME).ko

#-------------------------------------------#
# Object files                              #
#-------------------------------------------#
$(M)/$(BUILDDIR)/%.o : $(M)/%.c
	@echo "CC		$(@F)"
	@if test \( ! \( -d $(@D) \) \) ;then mkdir -p $(@D);fi
	$(VERBOSE) $(CC) -c $(CFLAGS) -o $@ $<

$(M)/$(BUILDDIR)/%.o : $(M)/%.cpp
	@echo "CXX		$(@F)"
	@if test \( ! \( -d $(@D) \) \) ;then mkdir -p $(@D);fi
	$(VERBOSE) $(CXX) -c $(CXXFLAGS) -o $@ $<

$(M)/$(BUILDDIR)/_%.o : $(M)/%.asm
	@echo "ASM		$(@F)"
	@if test \( ! \( -d $(@D) \) \) ;then mkdir -p $(@D);fi
	$(VERBOSE) $(ASM) $(ASMFLAGS) -o $@ $<

#-------------------------------------------#
# Clean                                     #
#-------------------------------------------#
clean:
	@echo "RM		$(M)/$(BUILDDIR)"
	$(VERBOSE) rm -rf $(M)/$(BUILDDIR)
	@echo "RM		$(MODULE_NAME).ko"
	$(VERBOSE) rm -f $(MODULEDIR)/$(MODULE_NAME).ko

