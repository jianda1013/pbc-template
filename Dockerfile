FROM debian:10

RUN apt update -y
RUN apt-get install \
    gcc \
    vim \
    wget \
    xz-utils \
    gzip \
    m4 \
    flex \
    bison \
    make \
    -y

WORKDIR /usr/etc

RUN wget https://gmplib.org/download/gmp/gmp-6.2.1.tar.xz && \
    tar Jxvf gmp-6.2.1.tar.xz && \
    cd gmp-6.2.1 && \
    ./configure && \
    make && \
    make check install

RUN wget https://crypto.stanford.edu/pbc/files/pbc-0.5.14.tar.gz && \
    tar xvf pbc-0.5.14.tar.gz && \
    cd pbc-0.5.14 && \
    ./configure && \
    make && \
    make install

ENV LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/usr/local/lib 

WORKDIR /usr/src


CMD ["sleep", "infinity"]