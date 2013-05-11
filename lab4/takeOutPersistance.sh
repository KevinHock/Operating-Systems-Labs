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
