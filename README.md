# Deye-SUN-5K-SG04LP1-EU-Monitoring_Solution
This is Raspberry Pi3 based Deye Inverter monitoring solution. It has Web cam, thermal vision and many other sensors.

Commands for setting up the solutions.

sudo apt install libopencv-dev mariadb-server php cmake libmysqlcppconn-dev  libasound2-dev acl  libboost-dev syslog-ng  libcamera-dev apache2
sudo apt install php8.2-mysql php8.2-gd
git clone https://github.com/WiringPi/WiringPi.git
git pull origin

sudo chown  www-data:www-data -R *
sudo setfacl -m u:solar:rwx data

sudo mysql -u root
create database pwrsys;
use pwrsys;
create table out_img(data BLOB,ts TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP);
insert into out_img(data) values(LOAD_FILE('/home/seccam/backup/dbase/tv640.jpg'));
create table out_tim(data BLOB);
insert into out_tim(data) values(LOAD_FILE('/home/seccam/backup/dbase/tv640.jpg'));

create user 'userpwrsys'@'localhost' identified by 'pwrsys123';
grant all on pwrsys.* TO 'userpwrsys'@'localhost';

create table cfg(dir_max MEDIUMINT NULL,access CHAR(255) NOT NULL,ip CHAR(32) NOT NULL,sn CHAR(32) NOT NULL,rb TINYINT NULL,wled TINYINT NULL);
insert into cfg (dir_max,access,ip,sn,rb,wled) values(2,"password","192.168.1.100","3144567744",0,0);
create table hour(temp_a TINYINT NULL,temp_b TINYINT NULL,press SMALLINT NULL, alt SMALLINT NULL, aqi TINYINT NULL,bat TINYINT NULL,rdr TINYINT NULL,therm SMALLINT NULL, dprod SMALLINT NULL,l1 SMALLINT NULL,l2 SMALLINT NULL,soc TINYINT NULL,pv1 SMALLINT NULL,pv2 SMALLINT NULL,grid TINYINT NULL,ts TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP);

Tutorials for Deye Hybrid inverter
https://www.youtube.com/watch?v=NoryPpAXwr8
https://www.youtube.com/watch?v=l7fjZCPHtFo
https://www.youtube.com/watch?v=GldAgJ-jelY
https://www.youtube.com/watch?v=w0NL5UM8k84
https://www.youtube.com/watch?v=tA5JtGWDEzw
https://www.youtube.com/watch?v=Xxo1R9NiNAA
https://www.youtube.com/watch?v=zz29BhmQpUM
https://www.youtube.com/watch?v=q-wrqaYjZ0k

