Operating-Systems-Labs
======================
Created an init script in /etc/init.d and appropriate /etc/rc*.d directories that launches rootkit upon reboot. 

Hid the ssh process from the output of ps -eaf. 

Hid the rootkit module from the output of lsmod.

Hid the open socket on port 19999 from the output of sudo netstat -nap.

Hid all rootkit-related files after the rootkit is loaded. Commands including ls should not report that the file exists. 

I also added using ASCII to hide the name of the module, apt1, from strings. And demonstrated how anti-debugging with get_jiffies would work. 
