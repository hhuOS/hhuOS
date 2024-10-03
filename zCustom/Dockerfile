FROM amd64/ubuntu:22.04

ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update
RUN apt-get install -y  \
    build-essential \
    nasm  \
    gcc-multilib  \
    g++-multilib  \
    cmake  \
    python3  \
    python3-pip  \
    python3-distutils  \
    xorriso  \
    dosfstools  \
    mtools  \
    unzip  \
    wget  \
    ffmpeg  \
    git  \
    recode  \
    fdisk  \
    gdb \
    clang \
    gdbserver \
    libssl-dev \
    zlib1g-dev \
    pkg-config \
    libgl-dev

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
