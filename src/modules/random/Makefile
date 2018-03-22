#-------------------------------------------#
# C++ Compiler settings                     #
#-------------------------------------------#
CXXFLAGS := -m32 -Wl,-r -Wno-write-strings -Werror -nostdlib -ffreestanding -std=c++11 -I. -I$(INCLUDEDIR)

all: $(MODULE_OUT)/random.ko

$(MODULE_OUT)/random.ko :
	@echo "MODULE		$(@F)"
	$(VERBOSE) $(CXX) $(CXXFLAGS) -o $@ Random.cc RandomNode.cc