while true
do
	tail -1000 /var/log/syslog | grep pwrsysapp
	ps -alx | grep pwrsysapp
        sleep 1
done

