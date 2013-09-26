Operating-Systems-Labs
======================
Created an init script in /etc/init.d and appropriate /etc/rc*.d directories that launches your rootkit upon reboot. 

Hide the ssh process from the output of ps -eaf. 

Hide the rootkit module from the output of lsmod.

Hide the open socket on port 19999 from the output of sudo netstat -nap.

Hide all rootkit-related files after the rootkit is loaded. Commands including ls should not report that the file exists. 

I also added using ASCII to hide the name of the module, apt1, from strings. And demonstrated how anti-debugging with get_jiffies would work. 
