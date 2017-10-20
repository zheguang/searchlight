# use parent image
FROM ubuntu:14.04


# for cmake 3.2 on ubuntu 14.04, required by or-tools
RUN apt-get install software-properties-common -y
RUN add-apt-repository ppa:george-edison55/cmake-3.x -y
RUN add-apt-repository ppa:andrei-pozolotin/maven3 -y
RUN apt-get update

RUN apt-get install -y \
      build-essential autoconf automake libtool cmake \
      gcc g++ \
      git wget vim locate curl unzip maven3 openjdk-7-jdk

# or-tools deps
RUN apt-get install -y \
      libgflags-dev \
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
      libboost-all-dev \
      #protobuf-compiler libprotobuf-dev \
      libmpich-dev mpich \
      ant ant-contrib ant-contrib-cpptasks \
      junit
      #libprotobuf-java
