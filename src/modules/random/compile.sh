g++ -m32 -Wl,-r -Wno-write-strings -nostdlib -ffreestanding -std=c++11 -I. -I../../os -o ../../hdd/mod/random.ko Random.cc RandomNode.cc
