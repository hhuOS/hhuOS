<p align="center">
  <img src="media/logo/logo_v3.svg">
</p>

<p align="center">
  <a href="https://travis-ci.org/hhuOS/hhuOS"><img src="https://travis-ci.org/hhuOS/hhuOS.svg?branch=master"></a>
  <img src="https://img.shields.io/badge/C%2B%2B-17-blue.svg">
  <img src="https://img.shields.io/badge/license-GPLv3-orange.svg">
</p>

# Introduction

Welcome to hhuOS, a **small operating system** written in C++ and Assembler for the
x86-architecture. The main purpose of this project is to show how different
aspects of operating systems theory can be implemented and linked together.
The system is *not* aimed to be a full-featured operating system for daily use.

Check out our [website](https://hhuos.github.io)!

# Compiling

GCC 7 and CMake 3.7 or newer versions of GCC and CMake are required to compile hhuOS.  
Before the kernel can be compiled, some packages must be installed. To install them, you can run the following command.

```sh
sudo apt install cmake make nasm gcc-multilib g++-multilib grub-pc-bin grub-efi-ia32-bin mtools xorriso
```

# Usage

To test hhuOS quickly in QEMU, you can issue the following commands.

```sh
git clone https://github.com/hhuOS/hhuOS.git
cd hhuOS/
./build.sh
./run.sh
```

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

# Kernel modules

hhuOS can be enhanced at runtime by loading kernel modules. Kernel modules can be loaded at boot time via the `hook`-parameters, or later via the shell command `insmod`. The following kernel modules are available:

- `cga` is a driver for the CGA graphics standard. CGA offers a frame buffer mode with a resolution of either 320x200 pixels with 4 colors or 640x200 pixels with 2 colors and a text mode with either 80x25 or 40x25 characters and 16 colors. This module requires BIOS enhancements to be turned on.
- `vesa` is a driver for the VESA BIOS Extensions, which offer high resolution frame buffers with up to 32-bit color colorDepth (depending on your graphics card and monitor). Text modes are simulated by drawing character sprites on the frame buffer. This module requires BIOS enhancements to be turned on.
- `fat` adds support for FAT12/16/32 partitions to the filesystem.
- `fs_memory` adds nodes, which hold information about the memory management, to `/dev/memory/`.
- `fs_video` adds nodes, which hold information about the current graphics driver, to `/dev/video/`.
- `fs_util` adds nodes, which offer new functions (e.g random), to `/dev/`.
- `floppy` is a driver for floppy drives. Attached floppy drives will be represented by nodes in `/dev/storage/`.
- `serial` is a driver for serial ports. Serial ports can be configured and used with the nodes in `/dev/ports/serialX/`.
- `parallel` is a driver for parallel ports. Parallel ports can be configured and used with the nodes in `/dev/ports/parallelX/`.
- `soundblaster` is a driver for the ISA based SoundBlaster cards `SoundBlaster 1`, `SoundBlaster 2`, `SoundBlaster Pro`, `SoundBlaser 16` and `SoundBlaster AWE 32` (untested). The shell command `wavplay` can be used to play WAV-files via a SoundBlaster card.
- `static_heap` is a simple memory manager, which does not support the operations `realloc`, `aligned alloc` and `free`, but is extremely fast. It can be tested with the `Memory Manager Demo` from the main menu.
- `hello` is a simple test module, which will just print a message, after being loaded.
