#!/bin/bash -e

apt-get update
apt-get -y upgrade
apt-get -y install git devscripts

#getting/building/installing
cd ~
git clone https://git.code.sf.net/p/yuma123/git yuma123_2.10
cd yuma123_2.10
mk-build-deps  -i -t 'apt-get -y'                                 
git clean -f

cd ..
tar -czvf yuma123_2.10.orig.tar.gz --exclude .git yuma123_2.10
cd yuma123_2.10
debuild -us -uc
dpkg -i ../*.deb

#testing
ssh-keyscan -t rsa -H localhost >> ~/.ssh/known_hosts
ssh-keygen -t rsa -N "" -f ~/.ssh/id_rsa
cat ~/.ssh/id_rsa.pub >> ~/.ssh/authorized_keys

echo "Port 830" >> /etc/ssh/sshd_config
echo "Port 1830" >> /etc/ssh/sshd_config
echo 'Subsystem netconf "/usr/sbin/netconf-subsystem --ncxserver-sockname=830@/tmp/ncxserver.sock --ncxserver-sockname=1830@/tmp/ncxserver.1830.sock"' >> /etc/ssh/sshd_config
/etc/init.d/ssh restart

cd netconf/test/netconfd
apt-get -y install python-ncclient python-paramiko python-lxml

autoreconf -i -f
./configure --prefix=/usr
make
make install
make check

echo "Success!"
