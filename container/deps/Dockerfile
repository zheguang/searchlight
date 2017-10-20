# use parent image
FROM zheguang/searchlight-os

# copy project files from host to working directory in container
#ADD . /searchlight
RUN git clone --recursive https://github.com/zheguang/searchlight.git /searchlight

# libcsv for scidb
WORKDIR /searchlight/third_party/libcsv
RUN cp /usr/share/automake-1.14/config.guess .
RUN cp /usr/share/automake-1.14/config.sub .
RUN ./configure
RUN make
RUN make install
RUN ldconfig

# protobuf 2.6.1 for or-tools
WORKDIR /searchlight/third_party/protobuf-2.6.1
RUN ./configure
RUN make
RUN make install
RUN ldconfig
WORKDIR /searchlight/third_party/protobuf-2.6.1/java
RUN mvn package
RUN ln -s /searchlight/third_party/protobuf-2.6.1/java/target/protobuf-java-2.6.1.jar /usr/share/java/protobuf.jar

# or-tools
WORKDIR /searchlight/third_party/or-tools
# https://developers.google.com/optimization/introduction/installing.html#basic_install_unix
# cpp
RUN mkdir -p objs/constraint_solver
RUN mkdir -p objs/util
RUN mkdir -p objs/base
RUN mkdir -p objs/linear_solver
RUN mkdir -p objs/sat
RUN mkidr -p objs/algorithms
RUN mkdir -p objs/graph
RUN mkdir -p lib
RUN make cc
RUN ldconfig

# scidb
WORKDIR /searchlight/third_party/scidb-14.12
RUN mkdir -p build
WORKDIR /searchlight/third_party/scidb-14.12/build
RUN cmake ..
RUN make
RUN make install
RUN ldconfig

# reset working directory
WORKDIR /searchlight
