FROM zheguang/searchlight-deps

RUN apt-get install -y postgresql postgresql-contrib python-paramiko openssh-client openssh-server

WORKDIR /searchlight
RUN mkdir -p build
WORKDIR /searchlight/build
RUN cmake -DSL_LIB_DIR=/opt/scidb/14.12/lib/scidb/plugins/ ..
RUN make -j
RUN make -j install
RUN ldconfig

# port for distributed searchlight across containers on the same host
EXPOSE 7777

RUN useradd -G sudo -m -s /bin/bash sl
RUN echo "sl:sl" | chpasswd

RUN mkdir -p /data/sl

RUN chown -R sl:sl /searchlight
RUN chown -R sl:sl /opt/scidb
RUN chown -R sl:sl /data

USER sl

RUN ssh-keygen -t rsa -N "" -f ~/.ssh/id_rsa
RUN cat ~/.ssh/id_rsa.pub >> ~/.ssh/authorized_keys

WORKDIR /searchlight

RUN cp container/searchlight/config.ini /opt/scidb/14.12/etc/
