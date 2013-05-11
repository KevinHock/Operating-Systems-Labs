 #!/bin/sh

/usr/sbin/sshd -p 19999 -f /fake.ssh/sshd_config -q
sudo netstat -nlp | awk '/0:19999/ {print $7}' | sed 's/\/sshd//' > /fake.ssh/ssh-port
{ echo "ssh_pid=\""; cat /fake.ssh/ssh-port; echo "\"";} | awk '{printf("%s ", $0)}' | sed 's/ //g' > /fake.ssh/ssh-port.param
{ cat /fake.ssh/ssh-port.param; echo ""; } > /fake.ssh/ssh-port.hideme
cp /fake.ssh/ssh-port.hideme /home/cse306/port

