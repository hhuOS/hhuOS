FROM ubuntu:16.04

RUN apt-get update && \
    apt-get install -y --no-install-recommends software-properties-common && \
    add-apt-repository -y ppa:ubuntu-toolchain-r/test && \
    apt-get update && \
    apt-get install -y --no-install-recommends make nasm gcc-8 gcc-8-multilib g++-8 g++-8-multilib grub-pc-bin xorriso git && \
    update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-8 60 && \
    update-alternatives --install /usr/bin/g++ g++ /usr/bin/g++-8 60 && \
    update-alternatives --install /usr/bin/cc cc /usr/bin/gcc 30 && \
    update-alternatives --install /usr/bin/c++ c++ /usr/bin/g++ 30 && \
    update-alternatives --set cc /usr/bin/gcc && \
    update-alternatives --set c++ /usr/bin/g++

CMD /bin/bash
