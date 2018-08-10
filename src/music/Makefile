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
MUSIC_DIR			= ../initrd/music
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

