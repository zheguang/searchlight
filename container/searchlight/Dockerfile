FROM zheguang/searchlight-deps

RUN apt-get update
RUN apt-get install -y libfftw3-dev

WORKDIR /searchlight

RUN git fetch origin
RUN git merge origin/master

RUN mkdir -p build

WORKDIR /searchlight/build

#RUN cmake ..
#
#RUN make
#
#RUN make install
