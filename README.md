# Deye-SUN-5K-SG04LP1-EU-Monitoring_Solution
Battery is Deye make model SE-G5.3
This is Raspberry Pi3 based Deye Inverter monitoring solution. It has Web cam, thermal vision and many other sensors.

Commands for setting up the solutions.

sudo apt install libopencv-dev mariadb-server php cmake libmysqlcppconn-dev  libasound2-dev acl  libboost-dev syslog-ng  libcamera-dev apache2
sudo apt install php8.2-mysql php8.2-gd i2c-tools

git clone https://github.com/WiringPi/WiringPi.git
git pull origin

git clone https://github.com/sanjuruk/ADS1X15_TLA2024_Linux

sudo chown  www-data:www-data -R *
sudo setfacl -m u:solar:rwx data

sudo mysql -u root
create database pwrsys;
use pwrsys;
create table out_img(data BLOB,ts TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP);
insert into out_img(data) values(LOAD_FILE('/home/seccam/backup/dbase/tv640.jpg'));
create table out_tim(data BLOB);
insert into out_tim(data) values(LOAD_FILE('/home/seccam/backup/dbase/tv640.jpg'));
create table out_tig(data BLOB);
insert into out_tig(data) values(LOAD_FILE('/home/seccam/backup/dbase/tv640.jpg'));
create table mask(id int NOT NULL AUTO_INCREMENT,x CHAR(255) NOT NULL,y CHAR(255) NOT NULL,w CHAR(255) NOT NULL,h CHAR(255) NOT NULL,PRIMARY KEY (id));

create user 'userpwrsys'@'localhost' identified by 'pwrsys123';
grant all on pwrsys.* TO 'userpwrsys'@'localhost';

create table cfg(dir_max MEDIUMINT NULL,access CHAR(255) NOT NULL,sip CHAR(32) NOT NULL,sn CHAR(32) NOT NULL,rb TINYINT NULL,wled TINYINT NULL,pkey CHAR(32) NOT NULL,mkey CHAR(32) NOT NULL,mip CHAR(32) NOT NULL,pip CHAR(32) NOT NULL,phr MEDIUMINT NULL);

create table hour(temp_a TINYINT NULL,temp_b TINYINT NULL,press SMALLINT NULL, alt SMALLINT NULL, aqi TINYINT NULL,bat TINYINT NULL,rdr TINYINT NULL,therm SMALLINT NULL,wl SMALLINT NULL,dprod SMALLINT NULL,dload SMALLINT NULL,dbuy SMALLINT NULL,soc TINYINT NULL,uload SMALLINT NULL,gload SMALLINT NULL,prod SMALLINT NULL,gvolt SMALLINT NULL,gdexp SMALLINT NULL,gexp SMALLINT NULL,ts TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP);

raspi-config timezone , autologin , enable i2c

cd /usr/include/libcamera
sudo cp libcamera/*.* . -r
sudo cp libcamera/ipa/ . -r
sudo cp libcamera/base/ . -r

Add below lines to .bashrc and enable autologin in pi3

app="ACTIVE"
#app=""
if [ -n "$app" ]; then
        rm /home/solar/app/access.txt -f
        cmd=$(pgrep pwrsysapp)
        if [ -z "$cmd" ]; then
                /home/solar/app/keep_alive.sh &
        fi
fi

alias ll="ls -al"


Tutorials for Deye Hybrid inverter
https://www.youtube.com/watch?v=NoryPpAXwr8
https://www.youtube.com/watch?v=l7fjZCPHtFo
https://www.youtube.com/watch?v=GldAgJ-jelY
https://www.youtube.com/watch?v=w0NL5UM8k84
https://www.youtube.com/watch?v=tA5JtGWDEzw
https://www.youtube.com/watch?v=Xxo1R9NiNAA
https://www.youtube.com/watch?v=zz29BhmQpUM
https://www.youtube.com/watch?v=q-wrqaYjZ0k

Output of tool
$ python InverterData.py

SN  240 123 59 187
SN  0xf0 0x7b 0x3b 0xbb
data len 140
0   59   0x003B-Running Status:2 28.0 30.0
0   63   0x003F-Total Grid Production:43.68000000000001kwh 36.0 38.0
0   64   0x0040-Total Grid Production:0.0kwh 38.0 40.0
0   76   0x004C-Daily Energy Bought:0.27999999999999997kwh 62.0 64.0
0   77   0x004D-Daily Energy Sold:0.0kwh 64.0 66.0
0   78   0x004E-Total Energy Bought:1.6300000000000001kwh 66.0 68.0
0   80   0x0050-Total Energy Bought:0.0kwh 70.0 72.0
0   81   0x0051-Total Energy Sold:0.04000000000000001kwh 72.0 74.0
0   82   0x0052-Total Energy Sold:0.0kwh 74.0 76.0
0   84   0x0054-Daily Load Consumption:0.45KWH 78.0 80.0
0   85   0x0055-Total Load Consumption:45.28KWH 80.0 82.0
0   86   0x0056-Total Load Consumption:0.0KWH 82.0 84.0
0   96   0x0060-Total Production:56.79KWH 102.0 104.0
0   97   0x0061-Total Production:0.0KWH 104.0 106.0
0   101   0x0065-Alert:0 112.0 114.0
0   102   0x0066-Alert:0 114.0 116.0
0   103   0x0067-Alert:0 116.0 118.0
0   104   0x0068-Alert:0 118.0 120.0
0   105   0x0069-Alert:0 120.0 122.0
0   106   0x006A-Alert:0 122.0 124.0
0   108   0x006C-Daily Production:0.18000000000000002KWH 126.0 128.0
0   109   0x006D-PV1 Voltage:24.36V 128.0 130.0
0   110   0x006E-PV1 Current:0.27A 130.0 132.0
0   111   0x006F-PV2 Voltage:0.09000000000000001V 132.0 134.0
0   112   0x0070-PV2 Current:0.010000000000000002A 134.0 136.0
SN  240 123 59 187
SN  0xf0 0x7b 0x3b 0xbb
data len 124
1   150   0x0096-Grid Voltage L1:22.840000000000003V 28.0 30.0
1   151   0x0097-Grid Voltage L2:0.0V 30.0 32.0
1   157   0x009D-Load Voltage:23.0V 42.0 44.0
1   164   0x00A4-Current L1:0.011000000000000001A 56.0 58.0
1   165   0x00A5-Current L2:0.0A 58.0 60.0
1   166   0x00A6-Micro-inverter Power:64W 60.0 62.0
1   166   0x00A6-Gen-connected Status:64 60.0 62.0
1   166   0x00A6-Gen Power:64W 60.0 62.0
1   167   0x00A7-Internal CT L1 Power:10W 62.0 64.0
1   168   0x00A8-Internal CT L2 Power:0W 64.0 66.0
1   169   0x00A9-Grid Status:10 66.0 68.0
1   169   0x00A9-Total Grid Power:10W 66.0 68.0
1   170   0x00AA-External CT L1 Power:0W 68.0 70.0
1   171   0x00AB-External CT L2 Power:0W 70.0 72.0
1   173   0x00AD-Inverter L1 Power:170W 74.0 76.0
1   174   0x00AE-Inverter L2 Power:0W 76.0 78.0
1   175   0x00AF-Total Power:170W 78.0 80.0
1   176   0x00B0-Load L1 Power:180W 80.0 82.0
1   177   0x00B1-Load L2 Power:0W 82.0 84.0
1   178   0x00B2-Total Load Power:180W 84.0 86.0
1   183   0x00B7-Battery Voltage:0.5352V 94.0 96.0
1   184   0x00B8-Battery SOC:61% 96.0 98.0
1   186   0x00BA-PV1 Power:710W 100.0 102.0
1   187   0x00BB-PV2 Power:0W 102.0 104.0
1   190   0x00BE-Battery Status:-483 108.0 110.0
1   190   0x00BE-Battery Power:-483W 108.0 110.0
1   191   0x00BF-Battery Current:-0.09029999999999999A 110.0 112.0
1   194   0x00C2-Grid-connected Status:1 116.0 118.0
1   195   0x00C3-SmartLoad Enable Status:17 118.0 120.0


