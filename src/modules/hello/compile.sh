g++ -m32 -Wl,-r -Wno-write-strings -no-pie -nostdlib -ffreestanding -std=c++11 -I. -I../../os -o empty.ko empty.cc
