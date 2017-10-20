FROM zheguang/searchlight-deps

WORKDIR /searchlight
RUN mkdir -p build
WORKDIR /searchlight/build
RUN cmake ..
RUN make
RUN make install
