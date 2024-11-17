FROM ubuntu:16.04

RUN apt-get update && \
    apt-get install -y \
    cmake \
    gcc \
    g++ \
    git && \
    cd /opt && \
    git clone https://cmiclab.cs.ucl.ac.uk/mmodat/niftyreg.git niftyreg && \
    cd /opt/niftyreg && \
    mkdir build && \
    cd build && \
    cmake -D CMAKE_BUILD_TYPE=Release /opt/niftyreg && \
    make && \
    make install && \
    rm -r /opt/niftyreg && \
    apt-get remove -y \
    cmake \
    git && \
    apt-get autoremove -y && \
    apt-get clean && rm -rf /var/lib/apt/lists/* /tmp/* /var/tmp/*

ADD run.sh /usr/local/bin
