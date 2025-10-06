#!/bin/bash
while true
do 
	cmd=$(pgrep pwrsysapp)
	if [ -z "$cmd" ]; then
		/home/solar/app/pwrsysapp
	fi
	sleep 50
	if [ ! -f /home/solar/app/access.txt ]; then
		logger pwrsysapp bash access file not found rebooting
    		sudo reboot
	fi
done
