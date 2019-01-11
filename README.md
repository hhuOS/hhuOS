<p align="center">
  <img src="media/logo/logo_v3.svg">
</p>

<p align="center">
  <a href="https://travis-ci.org/hhuOS/hhuOS"><img src="https://travis-ci.org/hhuOS/hhuOS.svg?branch=master"></a>
  <img src="https://img.shields.io/badge/license-GPL-orange.svg">
  <img src="https://img.shields.io/badge/C%2B%2B-17-blue.svg">
</p>

# Introduction

Welcome to hhuOS, a **small operating system** written in C++ and Assembler for the
x86-architecture. The main purpose of this project is to show how different
aspects of operating systems theory can be implemented and linked together.
The system is *not* aimed to be a full-featured operating system for daily use.

Check out our [website](https://hhuos.github.io)!

# Compiling

GCC 7 or a newer version of GCC is required to compile hhuOS.  
On Ubuntu 16.04 you can install GCC with the following commands:

```sh
sudo add-apt-repository ppa:ubuntu-toolchain-r/test
sudo apt update
sudo apt install gcc-8 gcc-8-multilib g++-8 g++-8-multilib
```

Before the kernel can be compiled, some packages must be installed. To install them, you can run the following command.

```sh
sudo apt install make nasm gcc-multilib g++-multilib grub-pc-bin grub-efi-ia32-bin mtools xorriso
```

After installing all required packages the following make targets are available for building:

| Target    | Location                       | Description                              |
|:---------:|--------------------------------|------------------------------------------|
| `kernel`  | `src/loader/boot/hhuOS.bin`    | Builds the kernel image.                 |
| `iso`     | `src/os/build/hhuOS.iso`       | Creates a bootable ISO image using GRUB. |
| `initrd`  | `src/loader/boot/hhuOS.initrd` | Generates the initial ramdisk.           |
| `modules` | `src/initrd/mod`               | Builds all modules.                      |

To run hhuOS in QEMU, the following make targets can be used:

| Target    | Description                                                                                                                                                                                                                               |
|:---------:|-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
| `qemu`    |  Runs hhuOS in a normal QEMU-session.                                                                                                                                                                                                     |
| `kvm`     |  Runs hhuOS in QEMU using KVM for better performance.                                                                                                                                                                                     |
| `qemu-efi`|  Runs hhuOS in QEMU using an EFI-BIOS. You need to set the EFI_BIOS-variable in the Makefile. We recommend using `OVMF`-image based on the `EDK II`. See https://github.com/tianocore/tianocore.github.io/wiki/OVMF for more information. |
| `kvm-efi` |  Runs hhuOS in QEMU using an EFI-BIOS and KVM.                                                                                                                                                                                            |


# Usage

To test hhuOS quickly in QEMU, you can issue the following commands.

```sh
git clone https://github.com/hhuOS/hhuOS.git
cd hhuOS/src/os
make qemu
```

If you are running  Ubuntu 16.04 and have installed GCC 8 with the above commands, you can use the following command to compile and run hhuOS.

```sh
CC=gcc-8 CXX=g++8 make
make qemu
```

If you want to test hhuOS on an EFI-based system, you can use `make qemu-efi`.

# Kernel parameters

hhuOS can be configured via kernel parameters, that are passed to the system by the bootloader. The following parameters are available:

- `root` is used to set the name and filesystem of the root-partition (e.g. `root=hdd0p1,FatDriver`).
- `linear_frame_buffer` is used to set the framebuffer-implementation, that should be used to draw on the screen. The default implementation is named `LinearFrameBuffer` and should always work (as long as GRUB finds a video mode).
- `text_driver` is used to set the driver for text mode. The default implementation is named `LfbText` and should always work (as long as GRUB finds a video mode).
- `resolution` is used to set the desired resolution for hhuOS. When `LinearFrameBuffer` and `LfbText` are used as graphics drivers, there are no other resolution available than the one set by GRUB (800x600x32 in most cases). However, when the `vesa`-module is loaded on a BIOS-based system, there may be more resolutions available.
- `bios_enhancements` can be set to `true` to activate support for BIOS-calls. This is needed, when the `vesa`- or `cga`-module is loaded. CAUTION: Enabling this option will cause hhuOS to not boot on most EFI-based systems.
- `pci_names` can be set to `true` to enable parsing of the PCI-IDs file. This will result in a longer boot-time, but is rewarded by showing the names of the PCI-devices installed in the machine.
- `log_level` is used to the log-level. Available levels are (sorted by granularity from fine to rough) `trace`, `debug`, `info`, `warn` and `error`. The default level is `trace`.
- `log_devices` is used to specify ports to which the log output can be written. At the moment, hhuOS has drivers for the serial COM-ports and parallel LPT-ports (when the `serial`-/`parallel`-module is loaded). An example configuration could be `log_devices=com1,lpt1` to write the log to the first COM- and LPT-port. However, writing to LPT-ports is not recommended, as it is slow and will drastically hinder the system performance.
- `splash` can be set to `true` to enable a graphical bootscreen. Otherwise, the system will boot in text mode and show the log output.
- `after_initrd_mod_hook` is used to specify a list of modules to load right after the initial ramdisk has been mounted. For example, this option can be used to load a graphics driver early in the boot process.
- `after_pci_scan_mod_hook` is used to specify a list of modules to load right after the PCI bus has been scanned. This parameter can primarily be used to load drivers for PCI-based devices.
- `after_fs_init_mod_hook` is used to specify a list of modules to load right after the filesystem has been initialized completely. Any modules, that are not needed early in the boot process should be loaded here.
