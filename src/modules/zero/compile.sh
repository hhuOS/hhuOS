g++ -m32 -Wl,-r -Wno-write-strings -nostdlib -ffreestanding -std=c++11 -I. -I../../os -o ../../hdd/mod/zero.ko Zero.cc ZeroNode.cc
