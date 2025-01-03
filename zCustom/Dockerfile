FROM amd64/ubuntu:22.04

ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update
RUN apt-get install -y build-essential
RUN apt-get install -y nasm
RUN apt-get install -y gcc-multilib
RUN apt-get install -y g++-multilib
RUN apt-get install -y cmake
RUN apt-get install -y python3
RUN apt-get install -y python3-pip
RUN apt-get install -y python3-distutils
RUN apt-get install -y xorriso
RUN apt-get install -y dosfstools
RUN apt-get install -y mtools
RUN apt-get install -y unzip
RUN apt-get install -y wget
RUN apt-get install -y ffmpeg
RUN apt-get install -y git
RUN apt-get install -y recode
RUN apt-get install -y fdisk
RUN apt-get install -y gdb
RUN apt-get install -y clang
RUN apt-get install -y gdbserver
RUN apt-get install -y libssl-dev
RUN apt-get install -y zlib1g-dev
RUN apt-get install -y pkg-config
RUN apt-get install -y libgl-dev

#install openssl1.1.1
RUN wget https://www.openssl.org/source/openssl-1.1.1.tar.gz -O /tmp/openssl-1.1.1.tar.gz
RUN tar -xzf /tmp/openssl-1.1.1.tar.gz -C /tmp
RUN rm /tmp/openssl-1.1.1.tar.gz
WORKDIR /tmp/openssl-1.1.1
RUN ./config && make && make install
RUN echo "/usr/local/lib64" > /etc/ld.so.conf.d/openssl-1.1.1.conf && ldconfig
RUN openssl version

RUN apt-get upgrade -y
RUN pip3 install requests
RUN apt-get clean
RUN rm -rf /var/lib/apt/lists/*

CMD ["/bin/bash"]

# build with:
# docker build -t os-build-env .
