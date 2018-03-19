FROM zheguang/searchlight:latest

USER sl

WORKDIR /searchlight
RUN git pull origin

RUN mkdir -p build
WORKDIR /searchlight/build
RUN cmake -DSL_LIB_DIR=/opt/scidb/14.12/lib/scidb/plugins/ ..
RUN make -j
RUN make -j install
USER root
RUN ldconfig
