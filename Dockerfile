# use parent image
FROM ubuntu:14.04

# set working directory
WORKDIR /searchlight

# copy project files from host to working directory in container
ADD . /searchlight

# for cmake 3.2 on ubuntu 14.04, required by or-tools
RUN apt-get install software-properties-common -y
RUN add-apt-repository ppa:george-edison55/cmake-3.x -y
RUN apt-get update

RUN apt-get install -y \
      build-essential autoconf automake libtool cmake \
      gcc g++ \
      git wget vim locate curl unzip

# or-tools deps
RUN apt-get install -y \
      libgflags-dev \
      #libprotoc8 protobuf-compiler libprotobuf-dev \
      libgoogle-glog-dev \
      coinor-cbc coinor-libcbc-dev \
      coinor-libcoinutils3 coinor-libcoinutils-dev \
      coinor-libosi1 coinor-libosi-dev \
      coinor-clp coinor-libclp-dev \
      coinor-libcgl1 coinor-libcgl-dev \
      libsparsehash-dev \
      zlib1g-dev \
      texinfo help2man subversion


# scidb deps
RUN apt-get install -y \
      gfortran \
      pkg-config \
      liblog4cxx10 liblog4cxx10-dev \
      doxygen \
      libpqxx3-dev \
      libcppunit-dev \
      swig \
      flex bison \
      libbz2-dev \
      libreadline-dev \
      libblas-dev \
      liblapack-dev \
      openjdk-6-jdk \
      libboost-all-dev \
      protobuf-compiler libprotobuf-dev \
      libmpich-dev mpich \
      ant ant-contrib ant-contrib-cpptasks \
      junit \
      libprotobuf-java

# libcsv
WORKDIR /searchlight/third_party/libcsv
RUN cp /usr/share/automake-1.14/config.guess .
RUN cp /usr/share/automake-1.14/config.sub .
RUN ./configure
RUN make
RUN make install

# or-tools
WORKDIR /searchlight/third_party/or-tools
RUN git checkout -b v6.5 v6.5
# https://developers.google.com/optimization/introduction/installing.html#basic_install_unix
RUN make third_party
# cpp
RUN make cc

# scidb
WORKDIR /searchlight/third_party/scidb-14.12.0.9095
RUN mkdir -p build
WORKDIR /searchlight/third_party/scidb-14.12.0.9095/build
RUN cmake ..
RUN make
RUN make install


# reset working directory
WORKDIR /searchlight
