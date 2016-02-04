FROM ubuntu:latest
MAINTAINER Stephen Lane-Walsh <sdl.slane@gmail.com>

RUN apt-get update -y && apt-get upgrade -y && apt-get dist-upgrade -y
RUN apt-get install -y python-software-properties software-properties-common
RUN add-apt-repository -y ppa:ubuntu-toolchain-r/test && apt-get update -y
RUN apt-get install -y gcc-5 g++-5 make

RUN ln -s /usr/bin/gcc-5 /usr/bin/gcc && \
    ln -s /usr/bin/g++-5 /usr/bin/g++

ADD . /Coeus/
WORKDIR /Coeus/

RUN make -B
