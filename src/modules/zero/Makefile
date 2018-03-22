#-------------------------------------------#
# C++ Compiler settings                     #
#-------------------------------------------#
CXXFLAGS := -m32 -Wl,-r -Wno-write-strings -Werror -nostdlib -ffreestanding -std=c++11 -I. -I$(INCLUDEDIR)

all: $(MODULE_OUT)/zero.ko

$(MODULE_OUT)/zero.ko :
	@echo "MODULE		$(@F)"
	$(VERBOSE) $(CXX) $(CXXFLAGS) -o $@ Zero.cc ZeroNode.cc