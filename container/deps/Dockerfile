# use parent image
FROM sl-os

# protobuf 2.6.1 for or-tools
WORKDIR /searchlight/third_party/protobuf-2.6.1

# libcsv for scidb
WORKDIR /searchlight/third_party/libcsv
RUN cp /usr/share/automake-1.14/config.guess .
RUN cp /usr/share/automake-1.14/config.sub .
RUN ./configure
RUN make
RUN make install


## scidb
#WORKDIR /searchlight/third_party/scidb-14.12.0.9095
#RUN mkdir -p build
#WORKDIR /searchlight/third_party/scidb-14.12.0.9095/build
#RUN cmake ..
#RUN make
#RUN make install

# or-tools
#WORKDIR /searchlight/third_party/or-tools
#RUN git checkout -b v6.5 v6.5
# https://developers.google.com/optimization/introduction/installing.html#basic_install_unix
# RUN make third_party
# cpp
#RUN make cc

# reset working directory
WORKDIR /searchlight
