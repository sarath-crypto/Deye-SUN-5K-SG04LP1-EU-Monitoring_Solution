#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <syslog.h>
#include <queue>
#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include <vector>
#include <signal.h>
#include <fcntl.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <filesystem>
#include <fstream>
#include <sys/types.h>
#include <set>
#include <iomanip>
#include <bits/stdc++.h>
#include <stdlib.h>
#include <linux/input.h>
#include <sys/wait.h>
#include <stddef.h>
#include <stdint.h>
#include <math.h>
#include <algorithm>
#include <vector>
#include <fstream>
#include <map>
#include <sstream>
#include <errno.h>
#include <getopt.h>
#include <sys/param.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <bits/stdc++.h>
#include <sys/mman.h>
#include <wchar.h>
#include <time.h>
#include <termios.h>
#include <regex>
#include <alsa/asoundlib.h>
#include <chrono>
#include <cstdint>
#include <string>     
#include <cstddef> 
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <bits/stdc++.h>
#include <thread>
#include <chrono>
#include <wiringPi.h>
#include <wiringPiI2C.h>
#include <mutex>

#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/resultset.h>
#include <cppconn/statement.h>
#include <cppconn/prepared_statement.h>

#include "global.h"
#include "motiondetector.hpp"
#include "syscam.h"
#include "tcpc.h"
#include "amg8833.h"
#include "bmp180.h"
#include "ads1115.h"
#include "ADS1X15_TLA2024.h"
#include "amg8833.h"
#include "udps.h"

#define FPS_DURATION	400
#define BEACON_TH	4
#define DAY_SEC		86400
#define HR_SEC		3600
#define MAX_FERR        127
#define FILE_WRITE 	"/var/www/html/data/"
#define RUN_TIME	"/home/solar/app/access.txt"

#define RADAR_PIN       7
#define WLED_PIN        0
#define SWITCH_PIN      2

#define SCALE_BAT       0.000331125827815
#define SCALE_TMP       0.01210142143680368805224740683826
#define SOC_TH		20
#define GPWR_TH		5000
#define ULOAD_TH	5000
#define DQ_TO           900

#define NW_TO		4
#define FAULT_TH	16
#define DBMAX_SZ	96
#define POLL_TO		5
#define MOT_TO		5

/*
#define DEBUG_1		1
#define DEBUG_2		1
#define DEBUG_3		1
*/

namespace fs = std::filesystem;
using namespace std;
using namespace cv;
using namespace std::chrono;

mutex mx;
bool exit_main = false;
bool exit_dbproc = false;
bool exit_netproc = false;
bool radar_trigger = false;

typedef struct ipc{
	queue <frames> fq;
	queue <rtd> dq;	
	queue <iframes> iq;
	sql::Driver *pdriver;
        sql::Connection *pcon;

	uptme ut;
	bool db_wled;
	bool nw_state;
	bool db_state;
}ipc;

enum 	dbtype{DBNONE = 1,DBINT,DBSTRING};

void sort(map<unsigned int,string>& M){
        multimap<string,unsigned int> MM;
        for (auto& it : M) {
                MM.insert({ it.second, it.first });
        }
}

typedef struct param{
	string name;
	unsigned short pos;
}param;

typedef struct pixel{
	unsigned char b;
	unsigned char g;
	unsigned char r;
}pixel;

typedef struct mcell{
	unsigned short x;
	unsigned short y;
	unsigned short w;
	unsigned short h;

}mcell;

void  sighandler(int num){
        syslog(LOG_INFO,"pwrsysapp sighandler is exiting %d",num);
	exit_netproc = true;
	exit_dbproc = true;
        exit_main = true;
}

void get_pixel(pixel *p,unsigned pix){
	if(pix <= 196){
		p->b = 255;
		p->g = 0;
		p->r = 0;
	}
	if((pix > 196) && (pix < 248)){
		p->b = 0;
		p->g = pix + 7;
		p->r = pix + 7;
	}
	if(pix >= 248){
		p->b = 0;
		p->g = 0;
		p->r = 255;
	}
}

void radar_isr(void){
        radar_trigger = true;
#ifdef DEBUG_1
	printf("Radar ISR\n");
#endif
}

void clean_dir(unsigned char dir_max){
	map<unsigned int,string>sname;
	for (const auto & p : fs::directory_iterator(FILE_WRITE)){
		struct stat attrib;
		stat(p.path().string().c_str(), &attrib);
		unsigned int ts = mktime(gmtime(&attrib.st_mtime));
		string s = p.path().string();
		sname[ts] = s;
	}
	if(sname.size() > dir_max){
		sort(sname);
		fs::remove_all(sname.begin()->second);
	}
	sname.clear();
}


void file_write(char *pdata,unsigned long len,char type){
        string fn = FILE_WRITE;
        switch(type){
                case(IMG):{
                        gettimestamp(fn,IMG);
                        fn = fn+"jpg";
                        break;
                }
                case(IRI):{
                        gettimestamp(fn,IRI);
                        fn = fn+"jpg";
                        break;
                }
                case(TMR):{
                        gettimestamp(fn,TMR);
                        fn = fn+"tmr";
                        break;
                }
		case(LOG):{
                        gettimestamp(fn,LOG);
			fn = fn +"log";
			break;
		}
        }
        fs::path fp = fn;
        if(!fs::is_directory(fp.parent_path()))fs::create_directory(fp.parent_path());
        int fd = open (fn.c_str(),O_CREAT|O_WRONLY,0406);
        if(len)write(fd,pdata,len);
        close(fd);
}


void access_dbase(string &cmd,ipc *ip,unsigned char type){
	string token = cmd;
	if(!cmd.compare("dir_max"))cmd = "select dir_max from cfg";
	else if(!cmd.compare("rb"))cmd = "select rb from cfg";
	else if(!cmd.compare("access"))cmd = "select access from cfg";
	else if(!cmd.compare("wled"))cmd = "select wled from cfg";
	else if(!cmd.compare("wlo"))cmd = "select wlo from cfg";
	else if(!cmd.compare("whi"))cmd = "select whi from cfg";
	else if(!cmd.compare("sip")){
		cmd = "select sip from cfg";
		token = "sip";
	}else if(!cmd.compare("mip")){
		cmd = "select mip from cfg";
		token = "mip";
	}else if(!cmd.compare("pip")){
		cmd = "select pip from cfg";
		token = "pip";
	}else if(!cmd.compare("pkey")){
		cmd = "select pkey from cfg";
		token = "pkey";
	}else if(!cmd.compare("mkey")){
		cmd = "select mkey from cfg";
		token = "mkey";
	}else if(!cmd.compare("phr")){
		cmd = "select phr from cfg";
		token = "phr";
	}else if(!cmd.compare("sn")){
		cmd = "select sn from cfg";
		token = "sn";
	}else if(!cmd.compare("hrsz"))cmd = "select count(*) from hour";
	else if(!cmd.compare("tsout")){
        	cmd = "select ts from out_img";
		token = "ts";
	}else if(!cmd.compare("maskx")){
		cmd = "select x from mask";
		token = "x";
	}else if(!cmd.compare("masky")){
		cmd = "select y from mask";
		token = "y";
	}else if(!cmd.compare("maskw")){
		cmd = "select w from mask";
		token = "w";
	}else if(!cmd.compare("maskh")){
		cmd = "select h from mask";
		token = "h";
	}

	mx.lock();
	sql::Statement *stmt = ip->pcon->createStatement();
	switch(type){
	case(DBNONE):{
		stmt->execute(cmd.c_str());
		break;
	}
	case(DBINT):{
		sql::ResultSet  *res = stmt->executeQuery(cmd.c_str());
		if(res->next())cmd = to_string(res->getInt(1));
		delete res;
		break;
	}
	case(DBSTRING):{
		sql::ResultSet  *res = stmt->executeQuery(cmd.c_str());
		cmd.clear();
		while(res->next()){
			if(!cmd.length())cmd = res->getString(token.c_str());
			else cmd = cmd + " " + res->getString(token.c_str());
		}
		delete res;
		break;
	}
	}
	delete stmt;
	mx.unlock();
}


void *dbproc(void *p){
	ipc *ip = (ipc *)p;
	string cmd = "dir_max";
	access_dbase(cmd,ip,DBINT);
	unsigned char dir_max = stoi(cmd);

        cmd = "tsout";
	access_dbase(cmd,ip,DBSTRING);
	file_write(cmd.data(),cmd.length(),TMR);
	
	unsigned long long prev_ts = 0;
	unsigned long ts = time(NULL);
	
	long therm = 0;
	int tc = 0;

        syslog(LOG_INFO,"pwrsysapp started dbproc");
        while(!exit_dbproc){
        	pthread_setschedprio(pthread_self(),254);
		if(!ip->iq.empty()){
      			iframes f = ip->iq.front();
			therm += f.therm;
			tc++;

			mx.lock();
			sql::PreparedStatement *prep_stmt = NULL;
			try {
	       			prep_stmt = ip->pcon->prepareStatement("update out_tim set data=?");
            			throw -1;
    			}
    			catch (...) {
			}
                      	struct membuf : std::streambuf {
                                membuf(char* base, std::size_t n) {
                                        this->setg(base, base, base + n);
                                }
                        };
                        membuf mbufc((char*)f.ir_data,f.ir_len);
                        std::istream blobc(&mbufc);
                        prep_stmt->setBlob(1,&blobc);
                        prep_stmt->executeUpdate();
                        if(prep_stmt)delete prep_stmt;


			prep_stmt = NULL;
			try {
	       			prep_stmt = ip->pcon->prepareStatement("update out_tig set data=?");
            			throw -1;
    			}
    			catch (...) {
			}
                        membuf mbufg((char*)f.gr_data,f.gr_len);
                        std::istream blobg(&mbufg);
                        prep_stmt->setBlob(1,&blobg);
                        prep_stmt->executeUpdate();
                        if(prep_stmt)delete prep_stmt;
			mx.unlock();
                        ip->iq.pop();
			if(f.wr){
				clean_dir(dir_max);
				file_write((char *)f.ir_data,f.ir_len,IRI);
				file_write((char *)f.gr_data,f.gr_len,IRI);
			}
		}
		if(!ip->fq.empty()){
			mx.lock();
			sql::PreparedStatement *prep_stmt = NULL;
			try {
	       			prep_stmt = ip->pcon->prepareStatement("update out_img set data=?");
            			throw -1;
    			}
    			catch (...) {
			}

        		frames f = ip->fq.front();
                        struct membuf : std::streambuf {
                                membuf(char* base, std::size_t n) {
                                        this->setg(base, base, base + n);
                                }
                        };
                        membuf mbuf((char*)f.img_data,f.img_len);
                        std::istream blob(&mbuf);
                        prep_stmt->setBlob(1,&blob);
                        prep_stmt->executeUpdate();
                        if(prep_stmt)delete prep_stmt;
			mx.unlock();	
                        if((f.ts-prev_ts) <= FPS_DURATION){
				prev_ts = f.ts;
				f.wr = false;
			}
                        ip->fq.pop();
			if(f.wr){
				clean_dir(dir_max);
				file_write((char *)f.img_data,f.img_len,IMG);
			}
		}
		if(((time(NULL)-ts) >= DQ_TO) && (ip->dq.size())){
#ifdef	DEBUG_3
			cout << "DQ SIZE ~~~~~~~~~~~~~~~~~~~" << ip->dq.size() << endl;
#endif
			rtd rd;
			unsigned long tempa = 0;	
			unsigned long tempb = 0;	
			unsigned long pr = 0;
			unsigned long al = 0;
			unsigned long aqi = 0;
			unsigned long bat = 0;
			unsigned long wl = 0;
			unsigned long dprod = 0;
			unsigned long dload = 0;
			unsigned long dbuy = 0;
			unsigned long soc = 0;
			unsigned long uload = 0;
			unsigned long gload = 0;
			unsigned long gvolt = 0;
			unsigned long gdexp = 0;
			unsigned long gexp = 0;
			bool rdr = false;
			unsigned int sz = ip->dq.size();
			unsigned int wlsz = 0;
			for(unsigned int i = 0;i < sz;i++){
				rd = ip->dq.front();
				tempa += rd.tempa;
				tempb += rd.tempb;
				pr += rd.pr;
				al += rd.al;
				aqi += rd.aqi;
				bat += rd.bat;
				if(rd.wl){
					wlsz++;
					wl += rd.wl;
				}
				dprod += rd.dprod;
				dload += rd.dload;
				dbuy += rd.dbuy;
				soc += rd.soc;
				uload += rd.uload;
				gload += rd.gload;
				gvolt += rd.gvolt;
				gdexp += rd.gdexp;
				gexp += rd.gexp;
				if(rd.rdr)rdr = true;
				ip->dq.pop();
			}
			if(tempa)rd.tempa = (int)((float)tempa/(float)sz);
			if(tempb)rd.tempb = (int)((float)tempb/(float)sz);
			if(pr)rd.pr = (int)((float)pr/(float)sz);
			if(al)rd.al = (int)((float)al/(float)sz);
			if(aqi)rd.aqi = (int)((float)aqi/(float)sz);
			if(bat)rd.bat = (int)((float)bat/(float)sz);
			if(wl)rd.wl = (int)((float)wl/(float)wlsz);
			if(dprod)rd.dprod = (int)((float)dprod/(float)sz);
			if(dload)rd.dload = (int)((float)dload/(float)sz);
			if(dbuy)rd.dbuy = (int)((float)dbuy/(float)sz);
			if(soc)rd.soc = (int)((float)soc/(float)sz);
			if(uload)rd.uload = (int)((float)uload/(float)sz);
			if(gload)rd.gload = (int)((float)gload/(float)sz);
			if(gvolt)rd.gvolt = (int)((float)gvolt/(float)sz);
			if(gdexp)rd.gdexp = (int)((float)gdexp/(float)sz);
			if(gexp)rd.gexp = (int)((float)gexp/(float)sz);
			if(therm)therm = (int)((float)therm/(float)tc);
			rd.rdr = rdr;
			string cmd = "hrsz";
			access_dbase(cmd,ip,DBINT);
			sz = stoi(cmd);
			while(sz >= DBMAX_SZ){
#ifdef DEBUG_3
				cout <<"DB SZ "<<(int)sz <<endl;
#endif
				cmd = "hrsz";
				access_dbase(cmd,ip,DBINT);
				sz = stoi(cmd);
				cmd = "DELETE FROM hour WHERE ts IS NOT NULL order by ts asc LIMIT 1";
				access_dbase(cmd,ip,DBNONE);
				sleep(1);
			}

			cmd = to_string(rd.tempa)+" "+to_string(rd.tempb);
			cmd += " "+to_string(rd.pr)+" "+to_string(rd.al)+" "+to_string(rd.aqi)+" "+to_string(rd.bat)+" "+to_string(rd.rdr);
			cmd += " "+to_string(therm)+" "+to_string(rd.wl)+" "+to_string(rd.dprod)+" "+to_string(rd.dload);
			cmd += " "+to_string(rd.dbuy)+" "+to_string(rd.soc)+" "+to_string(rd.uload)+" "+to_string(rd.gload)+" "+to_string(rd.prod)+" "+to_string(rd.gvolt);
			cmd += " "+to_string(rd.gdexp)+" "+to_string(rd.gexp);
			file_write((char *)cmd.c_str(),cmd.length(),LOG);

			cmd = "insert into hour(temp_a,temp_b,press,alt,aqi,bat,rdr,therm,wl,dprod,dload,dbuy,soc,uload,gload,prod,gvolt,gdexp,gexp) values(";
			cmd += to_string(rd.tempa)+","+to_string(rd.tempb);
			cmd += ","+to_string(rd.pr)+","+to_string(rd.al)+","+to_string(rd.aqi)+","+to_string(rd.bat)+","+to_string(rd.rdr);
			cmd += ","+to_string(therm)+","+to_string(rd.wl)+","+to_string(rd.dprod)+","+to_string(rd.dload);
			cmd += ","+to_string(rd.dbuy)+","+to_string(rd.soc)+","+to_string(rd.uload)+","+to_string(rd.gload)+","+to_string(rd.prod)+","+to_string(rd.gvolt);
			cmd += ","+to_string(rd.gdexp)+","+to_string(rd.gexp)+")";

#ifdef DEBUG_3
			cout <<cmd << endl;
#endif
			access_dbase(cmd,ip,DBNONE);

			therm = 0;
			tc = 0;
			ts = time(NULL);
		}
      		ip->db_state = true;
        }
        syslog(LOG_INFO,"pwrsysapp dbproc stopped");
        return NULL;
}

void *netproc(void *p){
	ipc *ip = (ipc *)p;
    
	string cmd = "sip"; 
        access_dbase(cmd,ip,DBSTRING);
	string sip = cmd;

	cmd = "mip"; 
        access_dbase(cmd,ip,DBSTRING);
	string mip = cmd;

	cmd = "pip"; 
        access_dbase(cmd,ip,DBSTRING);
	string pip = cmd;

	cmd = "mkey"; 
        access_dbase(cmd,ip,DBSTRING);
	string mkey = cmd;

	cmd = "phr"; 
	access_dbase(cmd,ip,DBINT);
	unsigned char phr = stoi(cmd);

	cmd = "wlo"; 
	access_dbase(cmd,ip,DBINT);
	unsigned short wlo = stoi(cmd);

	cmd = "whi"; 
	access_dbase(cmd,ip,DBINT);
	unsigned short whi = stoi(cmd);
	
	cmd = "pkey"; 
        access_dbase(cmd,ip,DBSTRING);
	string pkey = cmd;
	
	cmd = "sn"; 
        access_dbase(cmd,ip,DBSTRING);
	unsigned int sn = stoul(cmd);
	unsigned char snb[4];
	memcpy(snb,(void *)&sn,4);

#ifdef DEBUG_1
	printf("Serial Number : %02x %02x %02x %02x\n",snb[0],snb[1],snb[2],snb[3]);
#endif	

	cmd = "update cfg set rb=0";
	access_dbase(cmd,ip,DBNONE);
	
	cmd = "update cfg set wled=0";
	access_dbase(cmd,ip,DBNONE);
	
	tcpc *pc = new tcpc(sip);
	if(!pc->state)syslog(LOG_INFO,"pwrsysapp netproc network tcpc failed");

	udps *pu = new udps(mip,pip);
	if(!pu->state)syslog(LOG_INFO,"pwrsysapp netproc network udps failed");

	unsigned char fault = 0;
	long tsp = time(NULL);
	long tsm = time(NULL);
	long ts_prev = time(NULL);

	double pr,tm;
        bmp180 bmp;
        bmp.calibration();
        ADS1115 ads;
        ads.setGain(GAIN_ONE);
        ads.setSps(SPS_128);
        wiringPiSetup () ;
        pinMode(RADAR_PIN,INPUT);
        wiringPiISR(RADAR_PIN, INT_EDGE_BOTH, &radar_isr);
        pinMode(SWITCH_PIN,INPUT);
        pinMode(WLED_PIN,OUTPUT);
        digitalWrite(WLED_PIN,HIGH);
	
	amg8833 amg;
	amg.init();


	vector <param> deye;
	param pv;

	pv.name = "DAILY_PROD";
	pv.pos = 126;
	deye.push_back(pv);

	pv.name = "DAILY_LOAD";
	pv.pos = 78;
	deye.push_back(pv);

	pv.name = "DAILY_BUY";
	pv.pos = 62;
	deye.push_back(pv);

	pv.name = "BAT_SOC";
	pv.pos = 96;
	deye.push_back(pv);

	pv.name = "U_LOAD";
	pv.pos = 78;
	deye.push_back(pv);

	pv.name = "G_LOAD";
	pv.pos = 66;
	deye.push_back(pv);

	pv.name = "PROD";
	pv.pos = 100;
	deye.push_back(pv);

	pv.name = "G_VOLT";
	pv.pos = 28;
	deye.push_back(pv);


	vector <int> x,y,w,h;
	cmd = "maskx";
	access_dbase(cmd,ip,DBSTRING);
	stringstream ss(cmd.c_str());
	string sp;
	while(getline(ss,sp,' '))x.push_back(stoi(sp));
	cmd = "masky";
	access_dbase(cmd,ip,DBSTRING);
	ss = stringstream(cmd.c_str());
	sp.clear();
	while(getline(ss,sp,' '))y.push_back(stoi(sp));
	cmd = "maskw";
	access_dbase(cmd,ip,DBSTRING);
	ss = stringstream(cmd.c_str());
	sp.clear();
	while(getline(ss,sp,' '))w.push_back(stoi(sp));
	cmd = "maskh";
	access_dbase(cmd,ip,DBSTRING);
	ss = stringstream(cmd.c_str());
	sp.clear();
	while(getline(ss,sp,' '))h.push_back(stoi(sp));

	float wfl = whi++;
	bool mrun = false;
	pdu rtu;
	rtd rd;
	memset(&rd,0,sizeof(rtd));

      	syslog(LOG_INFO,"pwrsysapp waiting for network connection");
	while(!pc->con)pc->recv();
      	syslog(LOG_INFO,"pwrsysapp started netproc");
	while(!exit_netproc){
        	pthread_setschedprio(pthread_self(),253);
		pu->recv();

		if(pu->rxfifo.size()){
			pdu p = pu->rxfifo[0];
			string msg((char *)&p.data,p.len);
#ifdef 	DEBUG_2	
			printf("Recv UDP<-----%s\n",msg.c_str());
#endif
			stringstream ss(msg);
			vector<string> word;
			string w;
			while (getline(ss,w,' '))word.push_back(w);
			if(!word[0].compare(pkey)){
#ifdef 	DEBUG_2	
				printf("PUMP UDP :%s\n",word[1].c_str());
#endif
				wfl = stof(word[1]);
				rd.wl = (unsigned short)wfl;
			}
			if(!word[0].compare(mkey)){
#ifdef 	DEBUG_2	
				printf("MOTOR UDP :%s\n",word[1].c_str());
#endif
			}
#ifdef 	DEBUG_2	
			printf("Water level:%.02f %d\n",wfl,rd.wl);
#endif
			pu->rxfifo.erase(pu->rxfifo.begin());
		}

		if((time(NULL)-tsm) >= MOT_TO){
			pdu p;
			string m_msg = mkey + " ";
			string p_msg = pkey;
			time_t t;
			struct tm *ptm;
			time(&t);
			ptm = localtime(&t);
			char ts[4];
			strftime(ts,24,"%H",ptm);
			unsigned char chr = atoi(ts);
			if(phr == chr){
				if(wfl <= wlo)mrun = true;
				if(wfl >= whi)mrun = false;
			}
			if(mrun)m_msg += "ON";
			else m_msg += "OFF";

			p.len = m_msg.length();
			memcpy(p.data,m_msg.c_str(),p.len);
			pu->txfifo_m.push_back(p);
			p.len = p_msg.length();
			memcpy(p.data,p_msg.c_str(),p.len);
			pu->txfifo_p.push_back(p);

			pu->send();
#ifdef DEBUG_2
			printf("MOTOR Send UDP----->%s %d %d %d\n",m_msg.c_str(),(int)chr,(int)phr,mrun);
			printf("PUMP  Send UDP----->%s %d %d %d\n",p_msg.c_str(),(int)chr,(int)phr,mrun);
#endif
			tsm = time(NULL);
		}

		if((time(NULL)-tsp) >= POLL_TO){
#ifdef DEBUG_1
			printf("Polling started\n");
#endif
	
			if(fault >= FAULT_TH){
				if(pc)delete pc;
				pc = new tcpc(sip);
				syslog(LOG_INFO,"pwrsysapp waiting for network connection due to fault");
				while(!pc->con)pc->recv();
				syslog(LOG_INFO,"pwrsysapp connected after fault detection");
			}
			for(int i = 0; i < NW_TO;i++,fault++){
				char data_a[] = {0xa5,0x17,0x00,0x10,0x45,0x00,0x00,snb[0],snb[1],snb[2],snb[3],0x02,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x03,0x00,0x3b,0x00,0x36,0xb4,0x11,0x09,0x15};
				rtu.len = sizeof(data_a);
				memcpy((void *)&rtu.data,data_a,rtu.len);
				pc->txfifo.push_back(rtu);
				pc->send();
				sleep(1);
				pc->recv();
				if(pc->rxfifo.size()){
					fault = 0;;
					break;
				}
			}
			if(fault){
#ifdef DEBUG_1
				printf("Branching up 1\n");
#endif
				continue;
			}
			memcpy(rtu.data,pc->rxfifo[0].data,pc->rxfifo[0].len);
			pc->rxfifo.erase(pc->rxfifo.begin());
			char buf[128];
			sprintf(buf,"%02x%02x",rtu.data[deye[0].pos],rtu.data[deye[0].pos+1]);
			rd.dprod  = stoi(buf,nullptr,16);
			sprintf(buf,"%02x%02x",rtu.data[deye[1].pos],rtu.data[deye[1].pos+1]);
			rd.dload  = stoi(buf,nullptr,16);
			sprintf(buf,"%02x%02x",rtu.data[deye[2].pos],rtu.data[deye[2].pos+1]);
			rd.dbuy  = stoi(buf,nullptr,16);


#ifdef DEBUG_3
			printf ("%s:%d\n",deye[0].name.c_str(),rd.dprod);
			printf ("%s:%d\n",deye[1].name.c_str(),rd.dload);
			printf ("%s:%d\n",deye[2].name.c_str(),rd.dbuy);
#endif
			if(rd.dprod > 99)continue;	
			if(rd.dprod > 99)continue;
			if(rd.dbuy > 99)continue;	

			for(int i = 0; i < NW_TO;i++,fault++){
				char data_a[] = {0xa5,0x17,0x00,0x10,0x45,0x00,0x00,snb[0],snb[1],snb[2],snb[3],0x02,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x03,0x00,0x96,0x00,0x2e,0x25,0xfa,0xb6,0x15};
				rtu.len = sizeof(data_a);
				memcpy((void *)&rtu.data,data_a,rtu.len);
				pc->txfifo.push_back(rtu);
				pc->send();
				sleep(1);
				pc->recv();
				if(pc->rxfifo.size()){
					fault = 0;;
					break;
				}
			}
			if(fault)continue;
			
			memcpy(rtu.data,pc->rxfifo[0].data,pc->rxfifo[0].len);
			pc->rxfifo.erase(pc->rxfifo.begin());
			sprintf(buf,"%02x%02x",rtu.data[deye[3].pos],rtu.data[deye[3].pos+1]);
			rd.soc  = stoi(buf,nullptr,16);
			if(rd.soc < SOC_TH)fault = true;
			sprintf(buf,"%02x%02x",rtu.data[deye[4].pos],rtu.data[deye[4].pos+1]);
			rd.uload  = stoi(buf,nullptr,16);
		       	if(rd.uload > ULOAD_TH)fault = true;
			sprintf(buf,"%02x%02x",rtu.data[deye[5].pos],rtu.data[deye[5].pos+1]);
			rd.gload  = stoi(buf,nullptr,16);
			if(rd.gload > GPWR_TH)fault = true;
			sprintf(buf,"%02x%02x",rtu.data[deye[6].pos],rtu.data[deye[6].pos+1]);
			rd.prod  = stoi(buf,nullptr,16);
			sprintf(buf,"%02x%02x",rtu.data[deye[7].pos],rtu.data[deye[7].pos+1]);
			rd.gvolt  = stoi(buf,nullptr,16);
			if(fault){
#ifdef DEBUG_1
				printf("Branching up 4\n");
#endif
				continue;
			}	

#ifdef DEBUG_3
			printf ("%s:%d\n",deye[3].name.c_str(),rd.soc);
			printf ("%s:%d\n",deye[4].name.c_str(),rd.uload);
			printf ("%s:%d\n",deye[5].name.c_str(),rd.gload);
			printf ("%s:%d\n",deye[6].name.c_str(),rd.prod);
			printf ("%s:%d\n",deye[7].name.c_str(),rd.gvolt);
#endif
	
// Gexp and Dgexp has to be implemented
			bmp.gettemperature(&tm);
			rd.tempa = round(tm);
			bmp.getpressure(&pr);
			rd.pr = round(pr);
			rd.al = round(bmp.altitude(pr));
			if(radar_trigger){
				rd.rdr = true;
				radar_trigger = false;
			}else rd.rdr = false;

			int ch0 = ads.readADC_SingleEnded(0);
			if(ch0 >= 1024)ch0 = 0;
			rd.aqi = ch0;
			int ch1 = ads.readADC_SingleEnded(1)*SCALE_BAT;
			if(ch1 >= 65535)ch1 = 0;
			rd.bat = ch1;
			int ch2 = ads.readADC_SingleEnded(2)*SCALE_TMP;
			if(ch2 >= 65535)ch2 = 0;
			rd.tempb = ch2;

			ip->dq.push(rd);

			rtd rd;
			memset(&rd,0,sizeof(rtd));

			string cmd = "wled";
			access_dbase(cmd,ip,DBINT);
			ip->db_wled = stoi(cmd);

			cmd = "rb";
			access_dbase(cmd,ip,DBINT);
			bool rb = stoi(cmd);
			if(rb){
				syslog(LOG_INFO,"pwrsysapp reboot via web");
				cmd = "update cfg set rb=0";
				access_dbase(cmd,ip,DBNONE);
				system("reboot");
				sighandler(100);
			}
#ifdef DEBUG_1
			printf ("Reboot\t:%d\n",rb);
			printf ("Temperature\t:%d C\n",rd.tempa);
			printf ("Pressure\t:%d hPa/mB\n",rd.pr);
			printf ("Altitude\t:%d ft\n",rd.al);
			printf ("AQI\t\t:%d\n",rd.aqi);
			printf ("Battery\t\t:%d\n",rd.bat);
			printf ("Temperature\t:%d\n",rd.tempb);
			printf ("Radar trigger\t:%d\n",rd.rdr);
			printf ("Dq %d\n",ip->dq.size());
#endif
			tsp = time(NULL);
			fault = 0;
		}
		
		if(time(NULL) != ts_prev){
			iframes f;
			f.therm  = amg.readThermistor();
#ifdef DEBUG_1
			printf("Therm %d\n",f.therm);
#endif
			amg.readGrid(&f.ir_data[0]);

			Mat fi(8,8,CV_32FC1,f.ir_data);
			double minVal; 
			double maxVal; 
			Point minLoc; 
			Point maxLoc;
			minMaxLoc(fi, &minVal, &maxVal, &minLoc, &maxLoc );
			fi = fi/(float)maxVal;
			minMaxLoc(fi, &minVal, &maxVal, &minLoc, &maxLoc );
			fi = fi-minVal;
			minMaxLoc(fi, &minVal, &maxVal, &minLoc, &maxLoc );
			double scale = (double)255.0/maxVal;
			fi.convertTo(fi,CV_8UC1,scale);
	
			Mat fmask = Mat::zeros(360,360,CV_8UC1);
			for(unsigned int i = 0;i < h.size();i++)fmask(Rect(x[i],y[i],w[i],h[i])) = 255;
			bitwise_not(fmask,fmask);

			threshold(fi,fi,192,254,3);
			resize(fi,fi,Size(360,360),INTER_LINEAR);

			Mat mfi;
			bitwise_and(fi,fmask,mfi);
			Mat cf(fi.size(),CV_8UC3,Scalar(255,0,0));

			for(int x = 0;x < 360;x++){
				for(int y = 0;y < 360;y++){
					unsigned char gp = mfi.at<uchar>(x,y);
					pixel p;
					get_pixel(&p,gp);
					cf.at<Vec3b>(Point(x,y)) = Vec3b(p.b,p.g,p.r);
				}
			}
			transpose(cf,cf);

			Mat3b bgr = cf;
    			Mat3b bgr_inv = ~bgr;
    			Mat3b hsv_inv;
    			cvtColor(bgr_inv, hsv_inv, COLOR_BGR2HSV);
    			Mat1b mask; 
    			inRange(hsv_inv, Scalar(90 - 10, 70, 50), Scalar(90 + 10, 255, 255), mask); 
			if(countNonZero(mask)){
				putText(cf,"THM",Point(2,18),FONT_HERSHEY_TRIPLEX,0.75,Scalar(0,255,0),1);
				f.wr = true;
			}else f.wr = false;
			bgr.release();
			bgr_inv.release();
			hsv_inv.release();
			mask.release();
			
			unsigned char q = 80;
                	vector<unsigned char>buf;
                	vector<int>param(2);
                	param[0] = IMWRITE_JPEG_QUALITY;
                	do{
                		buf.clear();
                        	param[1] = q;
                        	imencode(".jpg",cf,buf,param);
                        	q--;
               		}while(buf.size() > FRAME_SZ);
                	f.ir_len = buf.size();
                	memcpy(f.ir_data,buf.data(),f.ir_len);

			q = 80;
                	do{
                		buf.clear();
                        	param[1] = q;
                        	imencode(".jpg",fi,buf,param);
                        	q--;
               		}while(buf.size() > FRAME_SZ);
                	f.gr_len = buf.size();
                	memcpy(f.gr_data,buf.data(),f.gr_len);
			ip->iq.push(f);
			fi.release();
			cf.release();

			ip->iq.push(f);
			ts_prev = time(NULL);
		}
      		ip->nw_state = true;
        }
 	if(pc)delete pc;
	if(pu)delete pu;

        syslog(LOG_INFO,"pwrsysapp netproc stopped");
        return NULL;
}


int main(void){
	ipc ip;
	ip.pdriver = get_driver_instance();
        ip.pcon = ip.pdriver->connect("tcp://127.0.0.1:3306", "userpwrsys", "pwrsys123");
        ip.pcon->setSchema("pwrsys");

	if((ip.pdriver == NULL) || (ip.pcon == NULL)){
		cout << "Connector C++ MySql failed" << endl;
		return 0;
	}
#ifndef DEBUG_3
        pid_t process_id = 0;
        pid_t sid = 0;

        process_id = fork();
        if(process_id < 0)exit(1);
        if (process_id > 0)exit(0);

        umask(0);
        sid = setsid();
        if(sid < 0)exit(1);
#endif
        signal(SIGINT|SIGABRT,sighandler);
        openlog("pwrsysapp",LOG_CONS | LOG_PID | LOG_NDELAY, LOG_USER);
        syslog (LOG_NOTICE, "pwrsysapp started with uid %d", getuid ());

#ifndef DEBUG_3
        close(STDIN_FILENO);
        close(STDOUT_FILENO);
        close(STDERR_FILENO);
#endif
	string cmd;
	ip.db_wled = false;
	ip.db_state = false;
	ip.nw_state = false;

	syscam *pcam = new syscam("HDcamera:HDcamera");
	MotionDetector detector(1,0.2,20,0.1,5,10,2);
	unsigned char ferror = 0;
	unsigned long msec = 0;

        pthread_t th_netproc_id;
        pthread_t th_dbproc_id;

	pthread_create(&th_netproc_id,NULL,netproc,(void *)&ip);
	while(!ip.nw_state);

	pthread_create(&th_dbproc_id,NULL,dbproc,(void *)&ip);
	while(!ip.db_state);

	ip.ut.d = 0;
	ip.ut.h = 0;
	ip.ut.m = 0;
	ip.ut.uts = time(NULL);
	Mat frame;
	Mat dframe;
        std::list<cv::Rect2d>boxes;
	bool db = false;
	bool wled = false;

	syslog(LOG_INFO,"pwrsysapp initialized");
        while(!exit_main){
#ifdef 	DEBUG_1
		printf("Camera type :%d\n",pcam->type);
#endif
		if(pcam->type ==  NO_CAMERA){
			syslog(LOG_INFO,"pwrsysapp camera type failed and rebooting");
			system("reboot");
			sighandler(200);
		}
		if(!pcam->get_frame(frame)){
			ferror++;
			if(ferror >= MAX_FERR){
				ferror = 0;
				syslog(LOG_INFO,"pwrsysapp ferror camera failed and rebooting");
				system("reboot");
				sighandler(300);
			}
			continue;	
		}else{
                	msec = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
			ferror = 0;
			
		       	frame.copyTo(dframe);
                        boxes = detector.detect(dframe);
			dframe.release();
			if(boxes.size())for(auto i = boxes.begin(); i != boxes.end(); ++i)rectangle(frame,*i,Scalar(0,69,255),2,LINE_AA); 
			resize(frame,frame,Size(480,360),INTER_LINEAR);
 	 
			time_t t;
                        struct tm *ptm;
                        time(&t);
                        ptm = localtime(&t);
			char ts[16];
                        strftime(ts,16,"%H%M%S",ptm);
			string header(ts);
			getuptime(&ip.ut);
			header += "@"+to_string(ip.ut.d)+":"+to_string(ip.ut.h)+":"+to_string(ip.ut.m);

  			frames f;
                        if(boxes.size()){
				f.wr = true;
				header += " IMG";
			}
                        else f.wr = false;
			boxes.clear();
			f.ts = msec;

                        putText(frame,header.c_str(),Point(2,18),FONT_HERSHEY_TRIPLEX,0.75,Scalar(0,0,255),1);
                        
			unsigned char q = 80;
                        vector<unsigned char>buf;
                        vector<int>param(2);
                        param[0] = IMWRITE_JPEG_QUALITY;
                        do{
                                buf.clear();
                                param[1] = q;
                                imencode(".jpg",frame,buf,param);
                                q--;
                        }while(buf.size() > FRAME_SZ);

                        memcpy(f.img_data,buf.data(),buf.size());
                        f.img_len = buf.size();
                        ip.fq.push(f);
        	}
		if(access(RUN_TIME,F_OK))creat(RUN_TIME,0666);


		bool sw_wled = !digitalRead(SWITCH_PIN);
#ifdef DEBUG_1
		printf("Wled  DB:%d SW:%d %d\n",ip.db_wled,sw_wled,wled);
#endif
		if(!wled){
			if(ip.db_wled || sw_wled){
				if(ip.db_wled)db = true;
				digitalWrite(WLED_PIN,LOW);
				wled = true;
			}
		}else{
			if(db){
				if(!ip.db_wled){
					db = false;
					digitalWrite(WLED_PIN,HIGH);
					wled = false;
				}
			}else if(sw_wled){
				digitalWrite(WLED_PIN,HIGH);
				wled = false;
			}
		}
		sleep(1);
        }
	delete pcam;
       	delete ip.pcon;

	pthread_join(th_netproc_id,NULL);
	pthread_join(th_dbproc_id,NULL);
        
	syslog(LOG_INFO,"pwrsysapp stopped");
        closelog();
        return 0;
}
