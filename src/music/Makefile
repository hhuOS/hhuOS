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
MAKE 				= make

#-------------------------------------------#
# Source files                              #
#-------------------------------------------#
BEEP_SOURCES 		= $(shell find -name "*.beepsrc")

#-------------------------------------------#
# Build files                               #
#-------------------------------------------#
BEEP_OBJECTS 		= $(notdir $(BEEP_SOURCES:.beepsrc=.beep))

DEPENDENCIES 		= $(addprefix $(BUILD_DIR)/,$(BEEP_OBJECTS))

#-------------------------------------------#
# Paths                                     #
#-------------------------------------------#
MUSIC_DIR			= ../initrd/music/beep
SRC_DIR				= src
BUILD_DIR			= build

#-------------------------------------------#
# Compiler binaries                         #
#-------------------------------------------#
BEEPC 				= ../tools/beepc/beepc

#-------------------------------------------#
# Make targets                              #
#-------------------------------------------#
all: $(DEPENDENCIES)
	@echo "COPY		$(BUILD_DIR)/ --> $(MUSIC_DIR)/"
	@if test \( ! \( -d $(MUSIC_DIR) \) \) ;then mkdir -p $(MUSIC_DIR);fi
	$(VERBOSE) $(COPY) $(BUILD_DIR)/*.beep $(MUSIC_DIR)

#-------------------------------------------#
# Object files                              #
#-------------------------------------------#
$(BUILD_DIR)/%.beep : $(SRC_DIR)/%.beepsrc
	@echo "BEEPC		$(@F)"
	@if test \( ! \( -d $(@D) \) \) ;then mkdir -p $(@D);fi
	$(VERBOSE) $(BEEPC) $< $@

#-------------------------------------------#
# Clean                                     #
#-------------------------------------------#
clean:
	@echo "RM		$(BUILD_DIR)"
	$(VERBOSE) rm -rf 	$(BUILD_DIR)
	@echo "RM		$(MUSIC_DIR)"
	$(VERBOSE) rm -rf 	$(MUSIC_DIR)

