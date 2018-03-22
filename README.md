<p align="center">
  <img src="media/logo/hhuOSLogo_small.png">
</p>

<p align="center">
  <a href="https://travis-ci.org/hhuOS/hhuOS"><img src="https://travis-ci.org/hhuOS/hhuOS.svg?branch=master"></a>
  <img src="https://img.shields.io/badge/license-GPL-orange.svg">
  <img src="https://img.shields.io/badge/C%2B%2B-11-blue.svg">
</p>

# Introduction

Welcome to hhuOS, a **small operating system** written in C++ and Assembler for
x86-architectures. The main purpose of this project is to show how different
aspects of operating systems theory can be implemented and linked together.
The system is *not* aimed to be a full-featured operating system for daily use.

Check out the [website](https://hhuos.github.io)!

# Compiling

Before the kernel can be compiled, some packages must be installed. To install them, you can run the following command.

```sh
sudo apt-get install make nasm gcc-7 gcc-7-multilib \
                     g++-7 g++-7-multilib grub-pc-bin xorriso
```

Some packages may be unavailable on Ubuntu 16.04. In this case the following commands can be executed before installing the required packages.

```sh
sudo add-apt-repository ppa:ubuntu-toolchain-r/test
sudo apt-get update
```

After installing all required packages the following make targets are available.

|   Target  | Location                       | Description                             |
|:---------:|--------------------------------|-----------------------------------------|
| `kernel`  | `src/loader/boot/hhuOS.bin`    | Builds the kernel image                 |
| `iso`     | `src/os/build/hhuOS.iso`       | Creates a bootable ISO image using GRUB |
| `initrd`  | `src/loader/boot/hhuOS.initrd` | Generates the initial ramdisk           |
| `modules` | `src/initrd/mod`               | Builds all modules                      |

# Usage

To test hhuOS quickly in QEMU, you can issue the following commands.

```sh
git clone https://github.com/hhuOS/hhuOS.git
cd hhuOS/src/os
make qemu
```
