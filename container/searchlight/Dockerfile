FROM zheguang/searchlight-deps

RUN apt-get install -y postgresql postgresql-contrib python-paramiko openssh-client openssh-server

# install postgres locale
RUN apt-get install language-pack-es-base
RUN locale-gen
RUN locale-gen en_US.UTF-8


WORKDIR /searchlight
RUN git pull origin

RUN mkdir -p build
WORKDIR /searchlight/build
RUN cmake -DSL_LIB_DIR=/opt/scidb/14.12/lib/scidb/plugins/ ..
RUN make -j
RUN make -j install
RUN ldconfig

# ports for cross-container communication
# postgres
EXPOSE 5432
# ssh
EXPOSE 22
# SciDB communicates from 1239 to 1250
EXPOSE 1200-1300

RUN useradd -G sudo -m -s /bin/bash sl
RUN echo "sl:sl" | chpasswd

RUN mkdir -p /data/sl

RUN chown -R sl:sl /searchlight
RUN chown -R sl:sl /opt/scidb
RUN chown -R sl:sl /data

# Automatically run ssh service
#CMD /etc/init.d/ssh restart && \
#  /etc/init.d/postgresql restart && \
#  /bin/bash

# copy postgres configs
RUN cp container/searchlight/postgresql.conf /etc/postgresql/9.3/main/
RUN cp container/searchlight/pg_hba.conf /etc/postgresql/9.3/main/

# create postgres user and test database
RUN service postgresql start
RUN createdb
RUN psql -c "create user sl with password 'sl' superuser"
RUN psql -c "create database test with owner = sl"

# create database sl
USER sl
RUN createdb
USER root

# create ssh keys
USER sl

RUN ssh-keygen -t rsa -N "" -f ~/.ssh/id_rsa
RUN cat ~/.ssh/id_rsa.pub >> ~/.ssh/authorized_keys

WORKDIR /searchlight

RUN cp container/searchlight/config.ini.two-node /opt/scidb/14.12/etc/config.ini

USER root
RUN mkdir /var/run/sshd
CMD ["/usr/sbin/sshd", "-D"]
