# hhuOS

<p align="center">
  <img src="media/logo/logo_v3.svg">
</p>

<p align="center">
  <a href="https://github.com/hhuOS/hhuOS/actions/workflows/build.yml"><img src="https://github.com/hhuOS/hhuOS/actions/workflows/build.yml/badge.svg"></a>
  <img src="https://img.shields.io/badge/C%2B%2B-20-blue.svg">
  <img src="https://img.shields.io/badge/license-GPLv3-orange.svg">
</p>

## Introduction

Welcome to hhuOS, a **small operating system** written in C++ and Assembler for the x86-architecture. The main purpose of this project is to show how different aspects of operating systems theory can be implemented and linked together. The system is *not* aimed to be a full-featured operating system for daily use.

This is a project by the [Operating Systems group](https://www.cs.hhu.de/en/research-groups/operating-systems.html) at the *Heinrich Heine University Düsseldorf*.

<p align="center">
  <a href="https://www.uni-duesseldorf.de/home/en/home.html"><img src="media/logo/hhu.svg" width=300></a>
</p>

## Run without building

We provide nightly builds of our `master` and `development` branches via [GitHub Releases](https://github.com/hhuOS/hhuOS/releases). Execute the following commands to download and run the latest `master` build in QEMU (on Ubuntu 22.04):

```shell
sudo apt install wget qemu-system-x86
mkdir -p hhuOS
cd hhuOS
wget https://github.com/hhuOS/hhuOS/releases/download/nightly-master/hhuOS-master.tar.gz
tar -xzf hhuOS-master.tar.gz
./run.sh
```

The OS will boot into a shell with some UNIX-like commands. Run `ls /bin` to see all available applications. Try out `bug` and `battlespace` for some old-fashioned games!

<p align="center">
  <img src="media/screenshots/shell.png" width="600px">
</p>

If QEMU hangs on a black screen, try executing `./run.sh --bios true --file hhuOS-limine.iso`. There seems to be a problem with older QEMU versions and new OVMF images.



## Build from source

GCC (compatible with C++20), CMake (>=3.14) and some other dependencies are required to compile hhuOS. To install them, you can run the following command (on Ubuntu 22.04):

```shell
sudo apt install build-essential nasm gcc-multilib g++-multilib cmake libgl-dev python3 python3-distutils xorriso dosfstools mtools unzip wget ffmpeg git recode
```

Afterward, clone this repository and execute the included build-script:

```shell
git clone https://github.com/hhuOS/hhuOS.git
git submodule init
git submodule update
cd hhuOS
./build.sh
```

To test hhuOS in QEMU, simply execute the included run-script:

```shell
./run.sh
```

## What next?

Congratulations! If you have made it this far, you have successfully compiled and run hhuOS! If you have not done so yet, you should definitely try out the included games `bug`, `battlespace` and `player`, as well as the `demo` command, to see the capabilities of our user space game engine.

When you are done tinkering with the OS, why not try to build your own application for hhuOS? Our [wiki](https://github.com/hhuOS/hhuOS/wiki/) provides tutorials on how to set up a development environment and get started with app development for hhuOS.

## Screenshots

<table style="margin-left: auto; margin-right: auto">
    <tr>
        <td><img src="media/screenshots/shell.png" width="450px"></td>
        <td><img src="media/screenshots/network.png" width="450px"></td>
    </tr>
    <tr>
        <td><img src="media/screenshots/bug.png" width="450px"></td>
        <td><img src="media/screenshots/battlespace.png" width="450px"></td>
    </tr>
</table>

## Notes

Assets for the dino game have been taken from [itch.io](https://itch.io):
 - [Dino Characters](https://arks.itch.io/player-characters) by [*@ScissorMarks*](https://twitter.com/ScissorMarks) ([CC BY 4.0](https://creativecommons.org/licenses/by/4.0/legalcode))
 - [Dino Family](https://demching.itch.io/player-family) by [*DemChing*](https://demching.itch.io/) ([CC BY 4.0](https://creativecommons.org/licenses/by/4.0/legalcode))
 - [Pixel Adventure](https://pixelfrog-assets.itch.io/pixel-adventure-1) by [*Pixel Frog*](https://pixelfrog-assets.itch.io/) ([CC0 1.0](https://creativecommons.org/publicdomain/zero/1.0/legalcode))
 - [Pixel Platformer](https://kenney-assets.itch.io/pixel-platformer) by [*Kenney*](https://kenney-assets.itch.io/) ([CC0 1.0](https://creativecommons.org/publicdomain/zero/1.0/legalcode))
 - [Retro Cloud Tileset](https://ohnoponogames.itch.io/retro-cloud-tileset) by [*ohnoponogames*](https://ohnoponogames.itch.io/)

Assets for the bug defender game have been taken from [itch.io](https://itch.io):
- [Lunar Battle Pack](https://mattwalkden.itch.io/lunar-battle-pack) by [*MattWalkden*](https://mattwalkden.itch.io/) ([CC0 1.0](https://creativecommons.org/publicdomain/zero/1.0/legalcode))
- [Pixel Heart Animation](https://nicolemariet.itch.io/pixel-heart-animation-32x32-16x16-freebie) by [*Nicole Marie T*](https://nicolemariet.itch.io/)

Assets for the battlespace game have been taken from [itch.io](https://itch.io):
- [Lowpoly - 3D Space Assets Pack](https://ejgarner118.itch.io/spacepack) by [*ejgarner118*](https://ejgarner118.itch.io/)

3D-demo assets haven been taken from [itch.io](https://itch.io):
- [M1 Tank](https://alstrainfinite.itch.io/m1-tank) by [*Alstra Infinite*](https://alstrainfinite.itch.io/) ([CC BY 4.0](https://creativecommons.org/licenses/by/4.0/legalcode))
- [Shark](https://alstrainfinite.itch.io/fish) by [*Alstra Infinite*](https://alstrainfinite.itch.io/) ([CC BY 4.0](https://creativecommons.org/licenses/by/4.0/legalcode))
- [Plane](https://alstrainfinite.itch.io/planes) by [*Alstra Infinite*](https://alstrainfinite.itch.io/) ([CC BY 4.0](https://creativecommons.org/licenses/by/4.0/legalcode))
- [Coffee Maker](https://alstrainfinite.itch.io/kitchen-appliance-2) by [*Alstra Infinite*](https://alstrainfinite.itch.io/) ([CC BY 4.0](https://creativecommons.org/licenses/by/4.0/legalcode))

Assets for the mouse demo haven been taken from [Icons8](https://icons8.com/).

Music for the SoundBlaster demo has been taken from [Bensound](https://www.bensound.com/royalty-free-music):
- Ukulele license code: 3M7PXYPYNOTSIGNQ

Ported software:
- Doom has been ported to hhuOS using [doomgeneric](https://github.com/ozkl/doomgeneric) by [*ozkl*](https://github.com/ozkl/) ([GPLv2](https://github.com/ozkl/doomgeneric/blob/master/LICENSE))
- Quake has been ported to hhuOS using [quakegeneric](https://github.com/erysdren/quakegeneric) by [*erysdren*](https://github.com/erysdren/) ([GPLv2](https://github.com/erysdren/quakegeneric/blob/master/LICENSE))
- [TinyGL](https://github.com/C-Chads/tinygl) by [*C-Chads*](https://github.com/C-Chads/) ([License](https://github.com/C-Chads/tinygl/blob/main/LICENSE))